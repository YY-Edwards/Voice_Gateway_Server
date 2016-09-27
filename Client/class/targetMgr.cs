using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    [Serializable]
    public class CEmployee
    {
        public int ID{set; get;}
        public string Name { set; get; } 
    }

    [Serializable]
    public class CVehicle
    {
        public int ID { set; get; }
        public string Number { set; get; }
    }

    [Serializable]
    public class CGroup
    {
        public int ID { set; get; }
        public int GroupID{ set; get; }
        public string Name { set; get; } 
    }


    public enum RadioType
    {
        RADIO,
        RIDE
    };

    [Serializable]
    public class CRadio
    {
        public int ID { set; get; }
        public int RadioID { set; get; }
        public bool IsOnline { set; get; }
        public RadioType Type { set; get; }
    }

    public class CRelationship
    {
        public int Group { set; get; }
        public int Employee { set; get; }
        public int Vehicle { set; get; }
        public int Radio { set; get; }
    }


    [Serializable]
    public struct TargetSimple
    {
        public TargetType Type;
        public int ID;
    }

    public enum MemberType
    {
        Group,
        Employee,
        Vehicle,
        Radio,
    };

    [Serializable]
    public class CMember
    {
        public MemberType Type { set; get; }
        public CGroup Group { set; get; }
        public CEmployee Employee { set; get; }
        public CVehicle Vehicle { set; get; }
        public CRadio Radio { set; get; }

        public CMember() { }
        public CMember(MemberType type, CGroup group, CEmployee employee, CVehicle vehicle, CRadio radio)
        {
            Type = type;
            Group = group;
            Employee = employee;
            Vehicle = vehicle;
            Radio = radio;
        }

        public bool IsEqual(CMember member)
        {
            if (Type != member.Type) return false;

            if (((null == member.Group) && (null == Group)) || ((null != member.Group) && (null != Group) && (Group.ID == member.Group.ID))){}//group is equal;
            else return false;

            if (((null == member.Employee) && (null == Employee)) || ((null != member.Employee) && (null != Employee) && (Employee.ID == member.Employee.ID))){}//Employee is equal;
            else return false;

            if (((null == member.Vehicle) && (null == Vehicle)) || ((null != member.Vehicle) && (null != Vehicle) && (Vehicle.ID == member.Vehicle.ID))){}//Vehicle is equal;
            else return false;

            if (((null == member.Radio) && (null == Radio)) || ((null != member.Radio) && (null != Radio) && (Radio.ID == member.Radio.ID))){}//Radio is equal;
            else return false;

            return true;      
        }


        public bool IsLike(CMember member)
        {
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
                else if (MemberType.Employee == Type)
                {
                    return (null == Employee) ? "" : Employee.Name;
                }
                else if(MemberType.Vehicle == Type)
                {
                    return (null == Vehicle) ? "" : Vehicle.Number;
                }
                else if (MemberType.Radio == Type)
                {
                    if (null == Radio) return "";
                    else
                    {
                        return ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
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
                else if (MemberType.Employee == Type)
                {
                    return (null == Employee) ? "" : Employee.Name;
                }
                else if (MemberType.Vehicle == Type)
                {
                    return (null == Vehicle) ? "" : Vehicle.Number;
                }
                else if (MemberType.Radio == Type)
                {
                    if (null == Radio) return "";
                    else
                    {
                        return ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台");
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
                if (MemberType.Employee == Type)
                {
                    if ((null == Vehicle) && (null == Radio)) return "";
                    else if (null == Radio) return Vehicle.Number;
                    else if (null == Vehicle) return ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
                    else return Vehicle.Number + "，" + ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
                }
                else if (MemberType.Vehicle == Type)
                {
                    if ((null == Employee) && (null == Radio)) return "";
                    else if (null == Radio) return Employee.Name;
                    else if (null == Employee) return ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
                    else return Employee.Name + "，" + ((RadioType.RADIO == Radio.Type) ? "手持台" : "车载台") + "：" + Radio.RadioID.ToString();
                }
                else if (MemberType.Radio == Type)
                {
                    if ((null == Employee) && (null == Vehicle)) return "";
                    else if (null == Vehicle) return Employee.Name;
                    else if (null == Employee) return Vehicle.Number;
                    else return Employee.Name + "，" + Vehicle.Number;
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

        //public string KeyName
        //{
        //    get {
        //        switch (Key)
        //        {
        //            case OrgItemType.Employee:
        //                return Employee.Name;
        //            case OrgItemType.Vehicle:
        //                return Vehicle.Number;
        //            case OrgItemType.Group:
        //                if (null == Group) return "未分组";
        //                return Group.Name;
        //            case OrgItemType.Radio:
        //            case OrgItemType.Ride:
        //                return (Radio.Type == RadioType.RADIO) ? "手持台" : "车载台";
        //            default:
        //                return "";
        //        }
        //    }
        //}

        //public string SubOne
        //{
        //    get
        //    {
        //        switch (Key)
        //        {
        //            case OrgItemType.Employee:
        //                return (null == Vehicle) ? "" : Vehicle.Number;
        //            case OrgItemType.Vehicle:
        //                return (null == Employee) ? "" : Employee.Name;
        //            case OrgItemType.Group:
        //                return (null == Group) ? "" : ("ID：" + Group.GroupID.ToString());
        //            case OrgItemType.Radio:
        //            case OrgItemType.Ride:
        //                return (null == Radio) ? "" : ("ID：" + Radio.RadioID.ToString());
        //            default:
        //                return "";
        //        }
        //    }
        //}

        //public string SubSecond
        //{
        //    get
        //    {
        //        switch (Key)
        //        {
        //            case OrgItemType.Employee:
        //            case OrgItemType.Vehicle:
        //                return (null == Group) ? "" : (Group.Name + " ：" + Group.GroupID.ToString());
        //            case OrgItemType.Group:
        //                return "";
        //            case OrgItemType.Radio:
        //            case OrgItemType.Ride:
        //                return (null == Group) ? "" : (Group.Name + " ：" + Group.GroupID.ToString());
        //            default:
        //                return "";
        //        }
        //    }
        //}

        //public string SubThird
        //{
        //    get
        //    {
        //        switch (Key)
        //        {
        //            case OrgItemType.Employee:
        //            case OrgItemType.Vehicle:
        //                return ((Radio.Type == RadioType.RADIO) ? "手持台：" : "车载台：") + Radio.RadioID.ToString();
        //            case OrgItemType.Group:
        //                return "";
        //            case OrgItemType.Radio:
        //            case OrgItemType.Ride:
        //                return "";
        //            default:
        //                return "";
        //        }
        //    }
        //}

        //public string Header
        //{
        //    get{return GetHeader(); }
        //}
        //public string KeyHeader
        //{
        //    get
        //    {
        //        switch (Key)
        //        {
        //            case OrgItemType.Employee:
        //                return GetHeaderByEmployee();
        //            case OrgItemType.Vehicle:
        //                return GetHeaderByVehicle();
        //            default:
        //                break;
        //        }
        //        return GetHeader();
        //    }
        //}

        //public string HeaderWithoutKey
        //{
        //    get 
        //    {
        //        switch (Key)
        //        {
        //            case OrgItemType.Group:
        //                return GetHeaderWithoutGroup();
        //            case OrgItemType.Employee:
        //                return GetHeaderWithOutEmployee();
        //            case OrgItemType.Vehicle:
        //                return GetHeaderWithOutVehicle();
        //            case OrgItemType.Radio:
        //            case OrgItemType.Ride:
        //                return GetHeaderWithOutRadio();
        //            default:
        //                break;
        //        }
        //        return GetHeader();
        //    }
        //}

        //public string HeaderWithoutGroup
        //{
        //    get { return GetHeaderWithoutGroup(); }
        //}

        //public string KeyHeaderWithoutGroup
        //{
        //    get
        //    {
        //        switch (key)
        //        {
        //            case OrgItemType.Type_Employee:
        //                return GetHeaderByEmployeeWithoutGroup();
        //            case OrgItemType.Type_Vehicle:
        //                return GetHeaderByVehicleWithoutGroup();
        //            default:
        //                break;
        //        }
        //        return GetHeaderWithoutGroup();
        //    }
        //}

        //private string GetHeader()
        //{
        //    if (null == employee)
        //    {
        //        if (null == vehicle)
        //        {
        //            if (null == radio)
        //            {
        //                if (null == group) return "未分组";
        //                else return group.name + "（ID：" + group.group_id.ToString() + "）";
        //            }
        //            else
        //            {
        //                if (null == group) return "ID：" + radio.radio_id.ToString();
        //                else return "ID：" + radio.radio_id.ToString() + "（" + group.name + "：" + group.group_id.ToString() + "）";
        //            }
        //        }
        //        else
        //        {
        //            if (null == radio)
        //            {
        //                if (null == group) return vehicle.number;
        //                else return vehicle.number + "（" + group.name + "：" + group.group_id.ToString() + "）";
        //            }
        //            else
        //            {
        //                if (null == group) return vehicle.number + "（RadioID：" + radio.radio_id.ToString() + "）";
        //                else return vehicle.number + "（" + group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "）";
        //            }
        //        }
        //    }
        //    else
        //    {
        //        if (null == vehicle)
        //        {
        //            if (null == radio)
        //            {
        //                if (null == group) return employee.name;
        //                else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "）";
        //            }
        //            else
        //            {
        //                if (null == group) return employee.name + "（RadioID：" + radio.radio_id.ToString() + "）";
        //                else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "）";
        //            }
        //        }
        //        else
        //        {
        //            if (null == radio)
        //            {
        //                if (null == group) return employee.name + "（" + vehicle.number + "）";
        //                else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "，" + vehicle.number +"）";
        //            }
        //            else
        //            {
        //                if (null == group) return employee.name + "（" + vehicle.number + "，RadioID：" + radio.radio_id.ToString() + "）";
        //                else return employee.name + "（"+ group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "，" + vehicle.number + "）";
        //            }
        //        }
        //    }
        //}


        //private string GetHeaderWithOutEmployee()
        //{
        //    if (null == employee)
        //    {
        //        return "";
        //    }
        //    else
        //    {
        //        string header = "";
        //        bool hasheader = false;

        //        //if ((null == group) && (null == vehicle) && (null == radio)) return header;


        //        if (null != group)
        //        {
        //            header += group.name + "：" + group.group_id.ToString();
        //            hasheader = true;
        //        }
        //         else
        //        {
        //            header += "未分组";
        //            hasheader = true;
        //        }

        //        if (null != vehicle)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += vehicle.number;
        //        }

        //        if (null != radio)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += "RadioID：" + radio.radio_id.ToString();
        //        }

        //        return header;
        //    }
        //}


        //private string GetHeaderByEmployee()
        //{
        //    if (null == employee)
        //    {
        //        return GetHeader();
        //    }
        //    else
        //    {
        //        return employee.name + (("" == GetHeaderWithOutEmployee()) ? "" :( "(" + GetHeaderWithOutEmployee() + ")"));
        //    }
        //}

        //private string GetHeaderWithOutVehicle()
        //{
        //    if (null == vehicle)
        //    {
        //        return "";
        //    }
        //    else
        //    {
        //        string header = "";
        //        bool hasheader = false;

        //        //if ((null == group) && (null == employee) && (null == radio)) return header;

        //        if (null != group)
        //        {
        //            header += group.name + "：" + group.group_id.ToString();
        //            hasheader = true;
        //        }
        //        else
        //        {
        //            header += "未分组";
        //            hasheader = true;
        //        }

        //        if (null != employee)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += employee.name;
        //            hasheader = true;
        //        }

        //        if (null != radio)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += "RadioID：" + radio.radio_id.ToString();
        //        }

        //        return header;
        //    }
        //}
        //private string GetHeaderByVehicle()
        //{
        //    if (null == vehicle)
        //    {
        //        return GetHeader();
        //    }
        //    else
        //    {
        //        return vehicle.number + (("" == GetHeaderWithOutVehicle()) ? "" : ("(" + GetHeaderWithOutVehicle() + ")"));
        //    }
        //}

        //public string HeaderWithOutRadio
        //{
        //    get
        //    {
        //        return GetHeaderWithOutRadio();
        //    }
        //}

        //private string GetHeaderWithOutRadio()
        //{
        //    if (null == radio)
        //    {
        //        return "";
        //    }
        //    else
        //    {
        //        string header = "";
        //        bool hasheader = false;

        //        //if ((null == group) && (null == employee) && (null == vehicle)) return header;

        //        if (null != group)
        //        {
        //            header += group.name + "：" + group.group_id.ToString();
        //            hasheader = true;
        //        }
        //        else
        //        {
        //            header += "未分组";
        //            hasheader = true;
        //        }

        //        if (null != employee)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += employee.name;
        //            hasheader = true;
        //        }

        //        if (null != vehicle)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += vehicle.number;
        //        }

        //        return header;
        //    }
        //}
        //private string GetHeaderByRadio()
        //{
        //    if (null == radio)
        //    {
        //        return GetHeader();
        //    }
        //    else
        //    {
        //        return ((radio.type == RadioType.RADIO) ? "手持台:" : "车载台:") + radio.radio_id.ToString() + (("" == GetHeaderWithOutVehicle()) ? "" : ("(" + GetHeaderWithOutVehicle() + ")"));
        //    }
        //}

        //private string GetHeaderWithoutGroup()
        //{
        //    if (null == Employee)
        //    {
        //        if(null == Vehicle)
        //        {
        //            if(null == Radio) return "";                    
        //            else return "ID：" + Radio.RadioID.ToString();
        //        }
        //        else
        //        {
        //            if (null == radio) return Vehicle.number;
        //            else return Vehicle.number + "（RadioID：" + radio.radio_id.ToString() + "）";
        //        }
        //    }
        //    else
        //    {
        //        if (null == Vehicle)
        //        {
        //            if (null == radio) return employee.name;
        //            else return employee.name + "（RadioID：" + radio.radio_id.ToString() + "）";

        //        }
        //        else
        //        {
        //            if (null == radio) return employee.name + "（" + Vehicle.number + "）";
        //            else return employee.name + "（" + Vehicle.number + "，RadioID：" + radio.radio_id.ToString() + "）";
        //        }
        //    }
        //}
        //public string GetHeaderByEmployeeWithoutGroup()
        //{
        //    if (null == Vehicle)
        //    {
        //        return GetHeaderWithoutGroup();
        //    }
        //    else
        //    {
        //        string header = employee.name;
        //        bool hasheader = false;

        //        if ((null == Vehicle) && (null == radio)) return header;

        //        header += "（";

        //        if (null != Vehicle)
        //        {
        //            header += Vehicle.number;
        //            hasheader = true;
        //        }

        //        if (null != radio)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += "RadioID：" + radio.radio_id.ToString();
        //        }

        //        return header + "）";
        //    }
        //}
        //public string GetHeaderByVehicleWithoutGroup()
        //{
        //    if (null == Vehicle)
        //    {
        //        return GetHeaderWithoutGroup();
        //    }
        //    else
        //    {
        //        string header = Vehicle.number;
        //        bool hasheader = false;

        //        if ((null == employee) && (null == radio)) return header;

        //        header += "（";

        //        if (null != employee)
        //        {
        //            header += employee.name;
        //            hasheader = true;
        //        }

        //        if (null != radio)
        //        {
        //            if (true == hasheader) header += "，";
        //            header += "RadioID：" + radio.radio_id.ToString();
        //        }

        //        return header + "）";
        //    }
        //}


        //public static bool Compare(CRelationShipObj src, CRelationShipObj dest)
        //{
        //    if (OrgItemType.Type_Group == src.key)
        //    {
        //        if ((null != src.group) && (null != src.group))
        //            if (src.group.id == dest.group.id)
        //            {
        //                return true;
        //            }
        //    }
        //    else
        //    {
        //        if ((null != src.radio) && (null != dest.radio))
        //            if (src.radio.id == dest.radio.id)
        //            {
        //                return true;
        //            }
        //    }

        //    return false;
        //}
    }


    [Serializable]
    public enum TargetType
    {
        All, 
        Group,
        Private
    };

    [Serializable]
    public enum SelectionType
    {
        Null,
        All,
        Multiple,
        Single
    };

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
                    switch (Target[0].Type)
                    {
                        case MemberType.Group:
                            return ((null == Target[0].Group) || (-1 == Target[0].Group.ID) || (-1 == Target[0].Group.GroupID)) ? "" : ("ID：" + Target[0].Group.GroupID.ToString());                           
                        case MemberType.Employee:
                            return (null == Target[0].Vehicle) ? "" : Target[0].Vehicle.Number;
                        case MemberType.Vehicle:
                            return (null == Target[0].Employee) ? "" : Target[0].Employee.Name;
                        case MemberType.Radio:
                            return (null == Target[0].Radio) ? "" : ("ID：" + Target[0].Radio.RadioID.ToString());
                        default:
                            return "";
                    }
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
                    switch (Target[0].Type)
                    {
                        case MemberType.Group:
                            return "";                      
                        case MemberType.Employee:
                        case MemberType.Vehicle:
                        case MemberType.Radio:
                            return ((null == Target[0].Group) || (-1 == Target[0].Group.ID) || (-1 == Target[0].Group.GroupID)) ? "" : (Target[0].Group.Name + " ：" + Target[0].Group.GroupID.ToString());
                        default:
                            return "";
                    }
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
                    switch (Target[0].Type)
                    {
                        case MemberType.Employee:
                        case MemberType.Vehicle:
                            return (null == Target[0].Radio) ? "" : (((Target[0].Radio.Type == RadioType.RADIO) ? "手持台：" : "车载台：") + Target[0].Radio.RadioID.ToString());
                        case MemberType.Group:
                        case MemberType.Radio:
                            return "";
                        default:
                            return "";
                    }
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
        public Dictionary<int, CMember> Employee;
        public Dictionary<int, CMember> Vehicle;
        public Dictionary<int, CMember> Group;
        public Dictionary<int, CMember> Radio;
    };

    public class TargetMgr
    {

        //test json
        string employee = "["
        + "{'id':1, 'name':'张三'},"
        + "{'id':2, 'name':'李四'},"
        + "{'id':3, 'name':'Jim'},"
        + "{'id':4, 'name':'John'},"
        + "{'id':5, 'name':'二麻子'},"
        + "{'id':6, 'name':'崔二胯子'},"
        + "{'id':7, 'name':'曾弓北'},"
        + "{'id':8, 'name':'萧剑南'},"
        + "{'id':9, 'name':'张国忠'},"
        + "{'id':10, 'name':'张国义'},"
        + "{'id':11, 'name':'张毅成'},"
        + "{'id':12, 'name':'柳萌萌'},"
        + "{'id':13, 'name':'秦戈'}"
        +"]";

        string vehicle = "["
        + "{'id':1, 'number':'川A12345'},"
        + "{'id':2, 'number':'贵B23456'},"
        + "{'id':3, 'number':'云C34567'},"
        + "{'id':4, 'number':'琼D45678'},"
        + "{'id':5, 'number':'京E56789'},"
        + "{'id':6, 'number':'津F67890'},"
        + "{'id':7, 'number':'陕G78901'},"
        + "{'id':8, 'number':'鲁H89012'}"
        + "]";


        string group = "["
        + "{'id':1, 'groupid':203, 'name':'调度组'},"
        + "{'id':2, 'groupid':314, 'name':'保洁组'},"
        + "{'id':3, 'groupid':425, 'name':'地勤组'},"
        + "{'id':4, 'groupid':536, 'name':'餐厅'},"
        + "{'id':5, 'groupid':647, 'name':'安保组'},"
        + "{'id':6, 'groupid':758, 'name':'候机厅'},"
        + "]";

        string radio = "["
        + "{'id':1, 'radioid':65536, 'isonline':true, 'type':'RIDE'},"
        + "{'id':2, 'radioid':112, 'isonline':true},"
        + "{'id':3, 'radioid':113, 'isonline':true},"
        + "{'id':4, 'radioid':114, 'isonline':true},"
        + "{'id':5, 'radioid':115, 'isonline':true},"
        + "{'id':6, 'radioid':116, 'isonline':true},"
        + "{'id':7, 'radioid':117, 'isonline':true},"
        + "{'id':8, 'radioid':118, 'isonline':true},"
        + "{'id':9, 'radioid':119, 'isonline':true},"
        + "{'id':10, 'radioid':120, 'isonline':true},"
        + "{'id':11, 'radioid':121, 'isonline':true},"
        + "{'id':12, 'radioid':122, 'isonline':true},"
        + "{'id':13, 'radioid':123, 'isonline':true},"
        + "{'id':14, 'radioid':124, 'isonline':true},"
        + "{'id':15, 'radioid':125, 'isonline':true},"
        + "{'id':16, 'radioid':126, 'isonline':true},"
        + "{'id':17, 'radioid':127, 'isonline':true},"
        + "{'id':18, 'radioid':128, 'isonline':true},"
        + "{'id':19, 'radioid':129, 'isonline':true},"
        + "{'id':20, 'radioid':130, 'isonline':true},"
        + "{'id':21, 'radioid':131, 'isonline':true},"
        + "{'id':22, 'radioid':132, 'isonline':true},"
        + "{'id':23, 'radioid':133, 'isonline':true},"
        + "{'id':24, 'radioid':134, 'isonline':true}"
        + "]";

        string relationship = "["
        + "{'group': 1, 'employee': 1, 'vehicle': 5, 'radio': 3 },"
        + "{'group': 1, 'employee': 3, 'vehicle': -1, 'radio': 4 },"
        + "{'group': 1, 'employee': 5, 'vehicle': -1, 'radio': 5 },"
        + "{'group': 1, 'employee': -1, 'vehicle': 1, 'radio': 6 },"
        + "{'group': 1, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 1, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 1, 'employee': -1, 'vehicle': -1, 'radio': 23 },"
        + "{'group': 1, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 2, 'employee': 4, 'vehicle': -1, 'radio': 7 },"
        + "{'group': 2, 'employee': 2, 'vehicle': -1, 'radio': 8 },"
        + "{'group': 2, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 3, 'employee': 6, 'vehicle': -1, 'radio': 9 },"
        + "{'group': 3, 'employee': 7, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 3, 'employee': -1, 'vehicle': -1, 'radio': 10 },"
        + "{'group': 3, 'employee': -1, 'vehicle': 2, 'radio': 11 },"
        + "{'group': 3, 'employee': -1, 'vehicle': -1, 'radio': 12 },"
        + "{'group': 4, 'employee': 8, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 4, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 4, 'employee': -1, 'vehicle': 3, 'radio': 14 },"
        + "{'group': 4, 'employee': -1, 'vehicle': 4, 'radio': -1 },"
        + "{'group': 5, 'employee': 10, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 5, 'employee': 11, 'vehicle': -1, 'radio': 15 },"
        + "{'group': 5, 'employee': 12, 'vehicle': -1, 'radio': 16 },"
        + "{'group': 5, 'employee': -1, 'vehicle': -1, 'radio': 17 },"
        + "{'group': 5, 'employee': -1, 'vehicle': -1, 'radio': 18 },"
        + "{'group': 5, 'employee': -1, 'vehicle': -1, 'radio': 19 },"
        + "{'group': 5, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 6, 'employee': 9, 'vehicle': 6, 'radio': 24 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': 20 },"
        + "{'group': 6, 'employee': -1, 'vehicle': 8, 'radio': -1 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': 6, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': -1, 'employee': 13, 'vehicle': -1, 'radio': 21 },"
        + "{'group': -1, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': -1, 'employee': -1, 'vehicle': -1, 'radio': 22 },"
        + "{'group': -1, 'employee': -1, 'vehicle': -1, 'radio': -1 },"
        + "{'group': -1, 'employee': -1, 'vehicle': 7, 'radio': -1 },"
        + "]";


        public Dictionary<int, CEmployee> g_EmployeeList = new Dictionary<int, CEmployee>();
        public Dictionary<int, CVehicle> g_VehicleList = new Dictionary<int, CVehicle>();
        public Dictionary<int, CGroup> g_GroupList = new Dictionary<int, CGroup>();
        public Dictionary<int, CRadio> g_RadioList = new Dictionary<int, CRadio>();


        public List<CRelationship> g_RelationshipList = new List<CRelationship>();


        CTargetRes m_TargetList = new CTargetRes();

        private bool m_IsChange = false;

        public TargetMgr()
        {

        }

        public void UpdateTragetList()
        {
            CEmployee[] employeearr = JsonConvert.DeserializeObject(employee, typeof(CEmployee[])) as CEmployee[];
            foreach (CEmployee em in employeearr) g_EmployeeList.Add(em.ID, em);
            g_EmployeeList.Add(-1, null);

            CVehicle[] vehiclearr = JsonConvert.DeserializeObject(vehicle, typeof(CVehicle[])) as CVehicle[];
            foreach (CVehicle ve in vehiclearr) g_VehicleList.Add(ve.ID, ve);
            g_VehicleList.Add(-1, null);

            CGroup[] grouparr = JsonConvert.DeserializeObject(group, typeof(CGroup[])) as CGroup[];
            foreach (CGroup gp in grouparr) g_GroupList.Add(gp.ID, gp);
            g_GroupList.Add(-1, new CGroup() { Name = "未分组", GroupID = -1, ID = -1 });

            CRadio[] radioarr = JsonConvert.DeserializeObject(radio, typeof(CRadio[])) as CRadio[];
            foreach (CRadio rd in radioarr) g_RadioList.Add(rd.ID, rd);
            g_RadioList.Add(-1, null);

            g_RelationshipList = JsonConvert.DeserializeObject(relationship, typeof(List<CRelationship>)) as List<CRelationship>;
            m_IsChange = true;
        }

        public CTargetRes TargetList
        {
            get {
                if (m_IsChange) BulidTargetList();
                m_IsChange = false;
                return m_TargetList; }
        }

        private void BulidTargetList()
        {
            m_TargetList = new CTargetRes();           
            if (null == m_TargetList.Group) m_TargetList.Group = new Dictionary<int, CMember>();
            if (null == m_TargetList.Employee) m_TargetList.Employee = new Dictionary<int, CMember>();
            if (null == m_TargetList.Vehicle) m_TargetList.Vehicle = new Dictionary<int, CMember>();
            if (null == m_TargetList.Radio) m_TargetList.Radio = new Dictionary<int, CMember>();

            foreach (var team in g_GroupList)
            {
                if (null == team.Value) continue;                
                m_TargetList.Group.Add(team.Value.ID, new CMember(MemberType.Group, team.Value, null, null, null));
            }

            foreach (var employee in g_EmployeeList)
            {
                if (null == employee.Value) continue;

                bool hasinrela = false;
                foreach (CRelationship rela in g_RelationshipList)
                {
                    if (employee.Value.ID == rela.Employee)//has employee
                    {
                        m_TargetList.Employee.Add(employee.Value.ID, new CMember(MemberType.Employee, g_GroupList[rela.Group], employee.Value, g_VehicleList[rela.Vehicle], g_RadioList[rela.Radio]));        
                        hasinrela = true;
                        break;
                    }
                }

                if (false == hasinrela)
                {
                    m_TargetList.Employee.Add(employee.Value.ID, new CMember(MemberType.Employee, new CGroup() {ID = -1, GroupID = -1, Name="未分组" }, employee.Value, null, null));        
                }
            }

            foreach (var vehicle in g_VehicleList)
            {
                if (null == vehicle.Value) continue;

                bool hasinrela = false;
                foreach (CRelationship rela in g_RelationshipList)
                {
                    if (vehicle.Value.ID == rela.Vehicle)//has vehicle
                    {
                        m_TargetList.Vehicle.Add(vehicle.Value.ID, new CMember(MemberType.Vehicle, g_GroupList[rela.Group], g_EmployeeList[rela.Employee], vehicle.Value, g_RadioList[rela.Radio]));        
                        hasinrela = true;
                        break;
                    }
                }

                if (false == hasinrela)
                {
                    m_TargetList.Vehicle.Add(vehicle.Value.ID, new CMember(MemberType.Vehicle, new CGroup() { ID = -1, GroupID = -1, Name = "未分组" }, null, vehicle.Value, null));        
                }
            }

            foreach (var radio in g_RadioList)
            {
                if (null == radio.Value) continue;

                bool hasinrela = false;
                foreach (CRelationship rela in g_RelationshipList)
                {
                    if (radio.Value.ID == rela.Radio)
                    {
                        hasinrela = true;                
                        //if ((-1 != rela.Vehicle) || (-1 != rela.Employee)) break;
                        m_TargetList.Radio.Add(radio.Value.ID, new CMember(MemberType.Radio, g_GroupList[rela.Group], g_EmployeeList[rela.Employee], g_VehicleList[rela.Vehicle], radio.Value));                              
                        break;
                    }
                }

                if (false == hasinrela)
                {
                    m_TargetList.Radio.Add(radio.Value.ID, new CMember(MemberType.Radio, new CGroup() { ID = -1, GroupID = -1, Name = "未分组" }, null, null, radio.Value));      
                }
            }
        }

        public void Update(CRadio radio)
        {
            if (!g_RadioList.ContainsKey(radio.ID))
            {
                g_RadioList.Add(radio.ID, radio);
            }
            else
            {
                g_RadioList[radio.ID] = radio;
            }

            m_IsChange = true;
        }

        public CMember SimpleToMember(TargetSimple target )
        {            
            if(TargetType.Private != target.Type)
            {
                foreach (var group in TargetList.Group)
                if(group.Value.Group.GroupID == target.ID)
                {
                    return new CMember(MemberType.Group, group.Value.Group, null,null, null);
                }
                return new CMember(MemberType.Group, new CGroup() { ID = -1, GroupID = target.ID, Name = "组：" + target.ID.ToString()}, null, null, null);
            }

            foreach (var employee in TargetList.Employee)
                if ((null != employee.Value.Radio) &&(employee.Value.Radio.RadioID == target.ID)) return employee.Value;

            foreach (var vehicle in TargetList.Vehicle)
                if ((null != vehicle.Value.Radio) && (vehicle.Value.Radio.RadioID == target.ID)) return vehicle.Value;

            foreach (var radio in TargetList.Radio)
                if ((null != radio.Value.Radio) && (radio.Value.Radio.RadioID == target.ID)) return radio.Value;

            return new CMember(MemberType.Radio, new CGroup() { ID = -1, GroupID = -1, Name = "未分组" }, null, null, new CRadio() { ID = -1, RadioID = target.ID, IsOnline = true});
        }
    }
}
