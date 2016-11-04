using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    //[Serializable]
    //public class CEmployee
    //{
    //    public int ID{set; get;}
    //    public string Name { set; get; } 
    //}

    //[Serializable]
    //public class CVehicle
    //{
    //    public int ID { set; get; }
    //    public string Number { set; get; }
    //}

    //[Serializable]
    //public class CGroup
    //{
    //    public int ID { set; get; }
    //    public int GroupID{ set; get; }
    //    public string Name { set; get; } 
    //}

    //[Serializable]
    //public class CRadio
    //{
    //    public int ID { set; get; }
    //    public int RadioID { set; get; }
    //    public bool IsOnline { set; get; }
    //    public RadioType Type { set; get; }
    //}

    //public class CRelationship
    //{
    //    public int Group { set; get; }
    //    public int Employee { set; get; }
    //    public int Vehicle { set; get; }
    //    public int Radio { set; get; }
    //}


    [Serializable]
    public struct TargetSimple
    {
        public TargetType Type;
        public long ID;

        public CMember ToMember()
        {
            if (TargetType.Private != Type)
            {
                foreach (var group in TargetMgr.TargetList.Group)
                if (group.Value.Group.GroupID == ID)
                {
                    return new CMember(MemberType.Group, group.Value.Group, null, null);
                }
                return new CMember(MemberType.Group, new Department() { ID = -1, GroupID = ID, Name = "组：" + ID.ToString() },null, null);
            }


            foreach (var staff in TargetMgr.TargetList.Staff)
                if ((null != staff.Value.Radio) && (staff.Value.Radio.RadioID == ID)) return staff.Value;


            foreach (var radio in TargetMgr.TargetList.Radio)
                if ((null != radio.Value.Radio) && (radio.Value.Radio.RadioID == ID)) return radio.Value;

            return new CMember(MemberType.Radio, new Department() { ID = -1, GroupID = -1, Name = "未分组" }, null, new Radio() { ID = -1, RadioID = ID, IsOnline = true });
       
        }
    }

    public enum MemberType
    {
        Group,
        Radio,
        Staff,
    };

    [Serializable]
    public class CMember
    {
        public MemberType Type { set; get; }
        public Department Group { set; get; }

        public Staff Staff { set; get; }
        public Radio Radio { set; get; }

        public CMember() { }
        public CMember(MemberType type, Department group, Staff staff, Radio radio)
        {
            Type = type;
            Group = group;
            Staff = staff;
            Radio = radio;
        }

        public bool IsEqual(CMember member)
        {
            if (Type != member.Type) return false;

            if (((null == member.Group) && (null == Group)) || ((null != member.Group) && (null != Group) && (Group.ID == member.Group.ID))){}//group is equal;
            else return false;

            if (((null == member.Staff) && (null == Staff)) || ((null != member.Staff) && (null != Staff) && (Staff.ID == member.Staff.ID))) { }//Employee is equal;
            else return false;

            if (((null == member.Radio) && (null == Radio)) || ((null != member.Radio) && (null != Radio) && (Radio.ID == member.Radio.ID))){}//Radio is equal;
            else return false;

            return true;      
        }


        public bool IsLike(CMember member)
        {
            if (null == member) return false;
            if (MemberType.Group == member.Type)
            {
                if (Type != member.Type) return false;
                if (((null == member.Group) && (null == Group)) || ((null != member.Group) && (null != Group) && (Group.ID == member.Group.ID))) return true;//group is equal;
                else return false;
            }
            else
                if (((null == member.Radio) && (null == Radio)) || ((null != member.Radio) && (null != Radio) && (Radio.ID == member.Radio.ID))) return true;//Radio is equal;
                else return false;       
        }

        

        public CMultMember SingleToMult()
        {
            CMultMember member = new CMultMember() { Type = SelectionType.Single, Target = new List<CMember> ()};
            member.Target.Add(this);

            return member;
        }
        //for display
        public string Name
        {
            get
            {
                if(MemberType.Group == Type)
                {
                    return (null == Group) ? "" : Group.Name;
                }
                else if (MemberType.Staff == Type)
                {
                    return (null == Staff) ? "" : Staff.Name;
                }
                else if (MemberType.Radio == Type)
                {
                    if (null == Radio) return "";
                    else
                    {
                        return ((RadioType.Radio == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
                    }
                }
                else
                {
                    return "";
                }
            }
        }

        public string SimpleName
        {
            get
            {
                if (MemberType.Group == Type)
                {
                    return (null == Group) ? "" : Group.Name;
                }
                else if (MemberType.Staff == Type)
                {
                    return (null == Staff) ? "" : Staff.Name;
                }
                else if (MemberType.Radio == Type)
                {
                    if (null == Radio) return "";
                    else
                    {
                        return ((RadioType.Radio == Radio.Type) ? "手持台" : "车载台");
                    }
                }
                else
                {
                    return "";
                }
            }
        }

        public string InformationWithoutGroup
        {
            get
            {
                if (MemberType.Staff == Type)
                {
                    if(null == Radio) return "";
                    else return ((RadioType.Radio == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();

                }
                else if(MemberType.Radio == Type)
                {
                    if(null == Staff) return "";
                    else return Staff.Name;
                }
                else
                {
                    return "";
                }
            }

        }

        public string Information
        {
            get
            {
                if (MemberType.Group == Type)
                {
                    return ((null == Group) || (-1 == Group.ID)) ? "" : ("ID："+Group.GroupID.ToString());
                }
                else
                {
                    return ((null == Group) || (-1 == Group.ID)) ? InformationWithoutGroup : (Group.Name + "：" + Group.GroupID.ToString() + ((InformationWithoutGroup == "") ? "" : "，") + InformationWithoutGroup);
                }              
            }
        }

        public string NameInfo
        {
            get
            {
                return Name + (("" == Information)?"":"：") + Information;
            }
        }
    }


    [Serializable]
    public enum TargetType
    {
        None = 0,
        All = 1,
        Group = 2,
        Private = 3,
    };

    [Serializable]
    public enum SelectionType
    {
        Null,
        All,
        Multiple,
        Single
    };

    public delegate void DisableFuncDel();

    [Serializable]
    public class CMultMember
    {
        public SelectionType Type { set; get; }
        public List<CMember> Target { set; get; }

        public CMultMember Clone()
        {
            if (null == this) return null;
            string tmp = JsonConvert.SerializeObject(this);
            return JsonConvert.DeserializeObject<CMultMember>(tmp);
        }


        public void DisableFunc(
            DisableFuncDel all,
            DisableFuncDel selectall,
            DisableFuncDel selectmlt,
            DisableFuncDel group,
            DisableFuncDel pri,
            DisableFuncDel nogps,
            DisableFuncDel noscreen,
            DisableFuncDel nokey
            )
        {
            if(Type == SelectionType.All)
            {
              if(selectall != null) selectall();
            }
            else if (Type != SelectionType.Null) 
            {
                if (Type != SelectionType.Multiple)
                {
                    if(selectmlt != null) selectmlt();
                }

                if(Target == null || Target.Count <= 0 ) 
                {
                    if(all != null) all();
                }
                else{

                     if(Target[0].Type == MemberType.Group)
                     {
                         if(group != null) group();

                         if(Target[0].Group == null|| Target[0].Group.GroupID <= 0 )
                         {
                             if(all != null) all();
                         }
                     }
                     else
                     {
                         if(pri != null) pri();
                         
                         if(Target[0].Radio == null|| Target[0].Radio.RadioID <= 0 )
                         {
                             if(all != null) all();
                         } 
                         else
                         {
                            if(!Target[0].Radio.HasGPS)
                            {
                                 if(nogps != null) nogps();
                            } 
                          
                            if(!Target[0].Radio.HasScreen)
                            {
                                 if(noscreen != null) noscreen();
                            } 

                            if(!Target[0].Radio.HasKeyboard)
                            {
                                 if(nokey != null) nokey();
                            }
                         }
                     

                     }
                }


            }
            else
            {
                if(all != null) all();
            }                  
        }
             

        public bool IsEqual(CMultMember member)
        {
            if((null == member)  ||(Type != member.Type))return false;

            if ((null == Target) && (null == member.Target)) return true;
            if((null == Target) || (null == member.Target) || (Target.Count != member.Target.Count))return false;

            for(int i = 0; i < Target.Count; i++)
            {
                if (!Target[i].IsEqual(member.Target[i])) return false;
            }
            return true;
        }

        public bool IsLike(CMultMember member)
        {
            if ((null == member) || (Type != member.Type)) return false;
            if ((null == Target) && (null == member.Target)) return true;
            if ((null == Target) || (null == member.Target) || (Target.Count != member.Target.Count)) return false;

            for (int i = 0; i < Target.Count; i++)
            {
                if (!Target[i].IsLike(member.Target[i])) return false;
            }
            return true;
        }

        public CMember MultToSingle()
        {
            if((null != Target) ||(0 == Target.Count))return null;
            return Target[0];
        }

        //display
        public string Name
        {
            get
            {
                if (SelectionType.Null == Type) return "";
                if (SelectionType.All == Type) return "全部终端";
                else if (SelectionType.Multiple == Type)
                {
                    if (Target.Count > 1) return Target[0].Name + "、" + Target[1].Name + "...";
                    else return Target[0].Name;

                }
                else return Target[0].Name;
            }
        }

        public string SimpleName
        {
            get
            {
                if (SelectionType.Null == Type) return "";
                if (SelectionType.All == Type) return "全部终端";
                else if (SelectionType.Multiple == Type)
                {
                    if (Target.Count > 1) return Target[0].SimpleName + "、" + Target[1].SimpleName + "...";
                    else return Target[0].SimpleName;

                }
                else return Target[0].SimpleName;
            }
        }

        public string Information
        {
            get
            {
                if ((SelectionType.Null == Type) || (SelectionType.All == Type)) return "";
                else if ((SelectionType.Multiple == Type) && (Target.Count > 1)) return "";
                else if (Target.Count == 1) return Target[0].Information;
                return "";
            }
        }

        public string NameInfo
        {
            get
            {
                return Name + (("" == Information) ? "" : "：") + Information;
            }
        }

        public string Information_First
        {
            get
            {
                if ((SelectionType.Null == Type)||(SelectionType.All == Type)) return "";
                else if ((SelectionType.Multiple == Type) && (Target.Count > 1)) return "";
                else if(Target.Count == 1){
                    //switch (Target[0].Type)
                    //{
                    //    case MemberType.Group:
                    //        return ((null == Target[0].Group) || (-1 == Target[0].Group.ID) || (-1 == Target[0].Group.GroupID)) ? "" : ("ID：" + Target[0].Group.GroupID.ToString());                           
                    //    case MemberType.Employee:
                    //        return (null == Target[0].Vehicle) ? "" : Target[0].Vehicle.Number;
                    //    case MemberType.Vehicle:
                    //        return (null == Target[0].Employee) ? "" : Target[0].Employee.Name;
                    //    case MemberType.Radio:
                    //        return (null == Target[0].Radio) ? "" : ("ID：" + Target[0].Radio.RadioID.ToString());
                    //    default:
                    //        return "";
                    //}
                }
                return "";
            }
        }

        public string Information_Second
        {
            get
            {
                if ((SelectionType.Null == Type) || (SelectionType.All == Type)) return "";
                else if ((SelectionType.Multiple == Type) && (Target.Count > 1)) return "";
                else  if(Target.Count == 1)
                {
                    //switch (Target[0].Type)
                    //{
                    //    case MemberType.Group:
                    //        return "";                      
                    //    case MemberType.Employee:
                    //    case MemberType.Vehicle:
                    //    case MemberType.Radio:
                    //        return ((null == Target[0].Group) || (-1 == Target[0].Group.ID) || (-1 == Target[0].Group.GroupID)) ? "" : (Target[0].Group.Name + " ：" + Target[0].Group.GroupID.ToString());
                    //    default:
                    //        return "";
                    //}
                }
                return "";
            }
        }

        public string Information_Third
        {
            get
            {
                if ((SelectionType.Null == Type) || (SelectionType.All == Type)) return "";
                else if ((SelectionType.Multiple == Type) && (Target.Count > 1)) return "";
                else if (Target.Count == 1)
                {
                    //switch (Target[0].Type)
                    //{
                    //    case MemberType.Employee:
                    //    case MemberType.Vehicle:
                    //        return (null == Target[0].Radio) ? "" : (((Target[0].Radio.Type == RadioType.RADIO) ? "手持台：" : "车载台：") + Target[0].Radio.RadioID.ToString());
                    //    case MemberType.Group:
                    //    case MemberType.Radio:
                    //        return "";
                    //    default:
                    //        return "";
                    //}
                }
                return "";
            }
        }


        //public static Dictionary<int, List<int>> MultipleTndexStore = new Dictionary<int, List<int>>();
        //public static int MultipleTargetCount = 0;
        //public static int GetMultipleTargetIndex(List<COrganization> target)
        //{
        //    if (1 == target.Count) return target[0].index;

        //    target.Sort((x, y) => x.index.CompareTo(y.index));

        //    foreach (var item in MultipleTndexStore)
        //    {
        //        if (item.Value.Count == target.Count)
        //        {
        //            item.Value.Sort((x, y) => x.CompareTo(y));
        //            bool broken = false;
        //            for (int i = 0; i < item.Value.Count; i++)
        //            {
        //                if (item.Value[i] != target[i].index)
        //                {
        //                    broken = true;
        //                    break;
        //                }
        //            }

        //            if (false == broken)
        //            {
        //                //is exist
        //                return item.Key;
        //            }
        //        }
        //    }

        //    //not exist
        //    MultipleTargetCount += 1;
        //    List<int> indexlist = new List<int>();
        //    foreach (COrganization tgt in target) indexlist.Add(tgt.index);
        //    MultipleTndexStore.Add(-MultipleTargetCount, indexlist);
        //    return -MultipleTargetCount;
        //}

    };

    [Serializable]
    public struct CTargetRes
    {
        public Dictionary<int, CMember> Group;
        public Dictionary<int, CMember> Radio;
        public Dictionary<int, CMember> Staff;
    };

    public class TargetMgr
    {
        private static Dictionary<int, Staff> m_StaffList = new Dictionary<int, Staff>();
        private static Dictionary<int, Department> m_DepartmentList = new Dictionary<int, Department>();
        private static Dictionary<int, Radio> m_RadioList = new Dictionary<int, Radio>();
        private static List<Belong> m_BelongList = new List<Belong>();

        private static CTargetRes m_TargetList = new CTargetRes();

        private static bool m_IsChange = false;

        public TargetMgr()
        {

        }

        public void UpdateTragetList()
        {
            ResourceMgr res = new ResourceMgr();

            List<Staff> stafftemp = res.Get(ResType.Staff) as List<Staff>;
            foreach (Staff it in stafftemp) m_StaffList.Add(it.ID, it);
            m_StaffList.Add(-1, null);

            List<Department> depttemp = res.Get(ResType.Department) as List<Department>;
            foreach (Department it in depttemp) m_DepartmentList.Add(it.ID, it);
            m_DepartmentList.Add(-1, new Department() { ID = -1, GroupID = -1, Name = "未分组"});

            List<Radio> radiotemp = res.Get(ResType.Radio) as List<Radio>;
            foreach (Radio it in radiotemp) m_RadioList.Add(it.ID, it);
            m_RadioList.Add(-1, null);

            List<Belong> belongtemp = res.Get(ResType.Belong) as List<Belong>;
            foreach (Belong it in belongtemp) m_BelongList.Add(it);

           

            m_IsChange = true;
        }

        public static CTargetRes TargetList
        {
            get {
                if (m_IsChange) BulidTargetList();
                m_IsChange = false;

                string tmp = JsonConvert.SerializeObject(m_TargetList);
                return JsonConvert.DeserializeObject<CTargetRes>(tmp);
            }
        }

        public static void UpdateTarget(CTargetRes lst)
        {
            //m_IsChange = true;
            string tmp = JsonConvert.SerializeObject(lst);
            m_TargetList =  JsonConvert.DeserializeObject<CTargetRes>(tmp);
        }

        private static void BulidTargetList()
        {
            m_TargetList = new CTargetRes();           
            if (null == m_TargetList.Group) m_TargetList.Group = new Dictionary<int, CMember>();
            if (null == m_TargetList.Staff) m_TargetList.Staff = new Dictionary<int, CMember>();
            if (null == m_TargetList.Radio) m_TargetList.Radio = new Dictionary<int, CMember>();

            foreach (var team in m_DepartmentList)
            {
                if (null == team.Value) continue;                
                m_TargetList.Group.Add(team.Value.ID, new CMember(MemberType.Group, team.Value, null, null));
            }

            foreach (var staff in m_StaffList)
            {
                if (null == staff.Value) continue;

                bool hasinbelong = false;
                foreach (Belong bing in m_BelongList)
                {
                    if (staff.Value.ID == bing.Staff)//has employee
                    {
                        m_TargetList.Staff.Add(staff.Value.ID, new CMember(MemberType.Staff, m_DepartmentList[bing.Department], staff.Value, m_RadioList[bing.Radio]));
                        hasinbelong = true;
                        break;
                    }
                }

                if (false == hasinbelong)
                {
                    m_TargetList.Staff.Add(staff.Value.ID, new CMember(MemberType.Staff, new Department() { ID = -1, GroupID = -1, Name = "未分组" }, staff.Value, null));        
                }
            }

            foreach (var radio in m_RadioList)
            {
                if (null == radio.Value) continue;

                bool hasinbelong = false;
                foreach (Belong bing in m_BelongList)
                {
                    if (radio.Value.ID == bing.Radio)
                    {
                        hasinbelong = true;
                        m_TargetList.Radio.Add(radio.Value.ID, new CMember(MemberType.Radio, m_DepartmentList[bing.Department], m_StaffList[bing.Staff], radio.Value));                              
                        break;
                    }
                }

                if (false == hasinbelong)
                {
                    m_TargetList.Radio.Add(radio.Value.ID, new CMember(MemberType.Radio, new Department() { ID = -1, GroupID = -1, Name = "未分组" }, null, radio.Value));      
                }
            }
        }

        public void Update(Radio radio)
        {
            if (!m_RadioList.ContainsKey(radio.ID))
            {
                for(int i = m_RadioList.Count; i <= m_RadioList.Count + 100; i++)
                {
                    if (!m_RadioList.ContainsKey(i))
                    {
                        radio.ID = i;
                        m_RadioList.Add(radio.ID, radio);
                        m_IsChange = true;
                        return ;
                    }
                }               
            }
            else
            {
                m_RadioList[radio.ID] = radio;
                m_IsChange = true;
                return;
            }
            return;           
        }





        public CMember SimpleToMember(TargetSimple target )
        {
            if (TargetType.Private != target.Type)
            {
                foreach (var group in TargetList.Group)
                    if (group.Value.Group.GroupID == target.ID)
                    {
                        return new CMember(MemberType.Group, group.Value.Group, null, null);
                    }
                return new CMember(MemberType.Group, new Department() { ID = -1, GroupID = target.ID, Name = "组：" + target.ID.ToString() },null, null);
            }


            foreach (var staff in TargetList.Staff)
                if ((null != staff.Value.Radio) && (staff.Value.Radio.RadioID == target.ID)) return staff.Value;


            foreach (var radio in TargetList.Radio)
                if ((null != radio.Value.Radio) && (radio.Value.Radio.RadioID == target.ID)) return radio.Value;

            return new CMember(MemberType.Radio, new Department() { ID = -1, GroupID = -1, Name = "未分组" }, null, new Radio() { ID = -1, RadioID = target.ID, IsOnline = true });
        }
    }
}
