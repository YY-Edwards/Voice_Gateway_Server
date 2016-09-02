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
        public int id{set; get;}
        public string name { set; get; } 
    }

    [Serializable]
    public class CVehicle
    {
        public int id { set; get; }
        public string number { set; get; }
    }

    [Serializable]
    public class CGroup
    {
        public int id { set; get; }
        public int group_id{ set; get; }
        public string name { set; get; } 
    }


    public enum RadioType
    {
        RADIO,
        RIDE
    };

    [Serializable]
    public class CRadio
    {
        public int id { set; get; }
        public int radio_id { set; get; }
        public bool is_online { set; get; }
        public RadioType type { set; get; }
    }

    public class CRelationship
    {
        public int group { set; get; }
        public int employee { set; get; }
        public int vehicle { set; get; }
        public int radio { set; get; }
    }

    public enum OrgItemType
    {
        Type_Group,
        Type_Employee,
        Type_Vehicle,
        Type_Radio,
        Type_Ride,
    };

    [Serializable]
    public class CRelationShipObj
    {
        public OrgItemType key { set; get; }
        public CGroup group{ set; get; }
        public CEmployee employee { set; get; }
        public CVehicle vehicle { set; get; }
        public CRadio radio { set; get; }

        public CRelationShipObj() { }
        public CRelationShipObj(OrgItemType k, CGroup grp, CEmployee emp, CVehicle veh, CRadio rad)
        {
            key = k;
            group  = grp;
            employee = emp;
            vehicle = veh;
            radio = rad;
        }

        public string KeyName
        {
            get {
                switch (key)
                {
                    case OrgItemType.Type_Employee:
                        return employee.name;
                    case OrgItemType.Type_Vehicle:
                        return vehicle.number;
                    case OrgItemType.Type_Group:
                        return group.name;
                    case OrgItemType.Type_Radio:
                    case OrgItemType.Type_Ride:
                        return radio.type == RadioType.RADIO ? "手持台:" :"车载台:" + radio.radio_id.ToString();
                    default:
                        return "";
                }
            }
        }
        public string Header
        {
            get{return GetHeader(); }
        }
        public string KeyHeader
        {
            get
            {
                switch (key)
                {
                    case OrgItemType.Type_Employee:
                        return GetHeaderByEmployee();
                    case OrgItemType.Type_Vehicle:
                        return GetHeaderByVehicle();
                    default:
                        break;
                }
                return GetHeader();
            }
        }

        public string HeaderWithoutGroup
        {
            get { return GetHeaderWithoutGroup(); }
        }

        public string KeyHeaderWithoutGroup
        {
            get
            {
                switch (key)
                {
                    case OrgItemType.Type_Employee:
                        return GetHeaderByEmployeeWithoutGroup();
                    case OrgItemType.Type_Vehicle:
                        return GetHeaderByVehicleWithoutGroup();
                    default:
                        break;
                }
                return GetHeaderWithoutGroup();
            }
        }

        private string GetHeader()
        {
            if (null == employee)
            {
                if (null == vehicle)
                {
                    if (null == radio)
                    {
                        if (null == group) return "未分组";
                        else return group.name + "（ID：" + group.group_id.ToString() + "）";
                    }
                    else
                    {
                        if (null == group) return "ID：" + radio.radio_id.ToString();
                        else return "ID：" + radio.radio_id.ToString() + "（" + group.name + "：" + group.group_id.ToString() + "）";
                    }
                }
                else
                {
                    if (null == radio)
                    {
                        if (null == group) return vehicle.number;
                        else return vehicle.number + "（" + group.name + "：" + group.group_id.ToString() + "）";
                    }
                    else
                    {
                        if (null == group) return vehicle.number + "（RadioID：" + radio.radio_id.ToString() + "）";
                        else return vehicle.number + "（" + group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "）";
                    }
                }
            }
            else
            {
                if (null == vehicle)
                {
                    if (null == radio)
                    {
                        if (null == group) return employee.name;
                        else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "）";
                    }
                    else
                    {
                        if (null == group) return employee.name + "（RadioID：" + radio.radio_id.ToString() + "）";
                        else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "）";
                    }
                }
                else
                {
                    if (null == radio)
                    {
                        if (null == group) return employee.name + "（" + vehicle.number + "）";
                        else return employee.name + "（" + group.name + "：" + group.group_id.ToString() + "，" + vehicle.number +"）";
                    }
                    else
                    {
                        if (null == group) return employee.name + "（" + vehicle.number + "，RadioID：" + radio.radio_id.ToString() + "）";
                        else return employee.name + "（"+ group.name + "：" + group.group_id.ToString() + "，RadioID：" + radio.radio_id.ToString() + "，" + vehicle.number + "）";
                    }
                }
            }
        }
        private string GetHeaderByEmployee()
        {
            if (null == employee)
            {
                return GetHeader();
            }
            else
            {
                string header = employee.name;
                bool hasheader = false;

                if ((null == group) && (null == vehicle) && (null == radio)) return header;

                header += "（";

                if (null != group)
                {
                    header += group.name + "：" + group.group_id.ToString();
                    hasheader = true;
                }

                if (null != vehicle)
                {
                    if (true == hasheader) header += "，";
                    header += vehicle.number;
                }

                if (null != radio)
                {
                    if (true == hasheader) header += "，";
                    header += "RadioID：" + radio.radio_id.ToString();
                }

                return header + "）";
            }
        }
        private string GetHeaderByVehicle()
        {
            if (null == vehicle)
            {
                return GetHeader();
            }
            else
            {
                string header = vehicle.number;
                bool hasheader = false;

                if ((null == group) && (null == employee) && (null == radio)) return header;

                header += "（";

                if (null != group)
                {
                    header += group.name + "：" + group.group_id.ToString();
                    hasheader = true;
                }

                if (null != employee)
                {
                    if (true == hasheader) header += "，";
                    header += employee.name;
                }

                if (null != radio)
                {
                    if (true == hasheader) header += "，";
                    header += "RadioID：" + radio.radio_id.ToString();
                }

                return header + "）";
            }
        }

        private string GetHeaderWithoutGroup()
        {
            if (null == employee)
            {
                if(null == vehicle)
                {
                    if(null == radio) return "";                    
                    else return "ID：" + radio.radio_id.ToString();
                }
                else
                {
                    if (null == radio)return vehicle.number;
                    else return vehicle.number + "（RadioID：" + radio.radio_id.ToString()+ "）";
                }
            }
            else
            {
                if (null == vehicle)
                {
                    if (null == radio) return employee.name;
                    else return employee.name + "（RadioID：" + radio.radio_id.ToString() + "）";

                }
                else
                {
                    if (null == radio) return employee.name + "（" + vehicle.number + "）";
                    else return employee.name + "（"+ vehicle.number + "，RadioID：" + radio.radio_id.ToString() + "）";
                }
            }
        }
        public string GetHeaderByEmployeeWithoutGroup()
        {
            if (null == vehicle)
            {
                return GetHeaderWithoutGroup();
            }
            else
            {
                string header = employee.name;
                bool hasheader = false;

                if ((null == vehicle) && (null == radio)) return header;

                header += "（";

                if (null != vehicle)
                {
                    header += vehicle.number;
                    hasheader = true;
                }

                if (null != radio)
                {
                    if (true == hasheader) header += "，";
                    header += "RadioID：" + radio.radio_id.ToString();
                }

                return header + "）";
            }
        }
        public string GetHeaderByVehicleWithoutGroup()
        {
            if (null == vehicle)
            {
                return GetHeaderWithoutGroup();
            }
            else
            {
                string header = vehicle.number;
                bool hasheader = false;

                if ((null == employee) && (null == radio)) return header;

                header += "（";

                if (null != employee)
                {
                    header += employee.name;
                    hasheader = true;
                }

                if (null != radio)
                {
                    if (true == hasheader) header += "，";
                    header += "RadioID：" + radio.radio_id.ToString();
                }

                return header + "）";
            }
        }
    }

    public class COrganization
    {
        public int index { set; get; }
        public CRelationShipObj target { set; get; }

        public bool is_exp { set; get; }
    }

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
        + "{'id':1, 'group_id':203, 'name':'调度组'},"
        + "{'id':2, 'group_id':314, 'name':'保洁组'},"
        + "{'id':3, 'group_id':425, 'name':'地勤组'},"
        + "{'id':4, 'group_id':536, 'name':'餐厅'},"
        + "{'id':5, 'group_id':647, 'name':'安保组'},"
        + "{'id':6, 'group_id':758, 'name':'候机厅'},"
        + "]";

        string radio = "["
        + "{'id':1, 'radio_id':65536, 'is_online':true, 'type':'RIDE'},"
        + "{'id':2, 'radio_id':112},"
        + "{'id':3, 'radio_id':113},"
        + "{'id':4, 'radio_id':114},"
        + "{'id':5, 'radio_id':115},"
        + "{'id':6, 'radio_id':116},"
        + "{'id':7, 'radio_id':117},"
        + "{'id':8, 'radio_id':118},"
        + "{'id':9, 'radio_id':119},"
        + "{'id':10, 'radio_id':120},"
        + "{'id':11, 'radio_id':121},"
        + "{'id':12, 'radio_id':122},"
        + "{'id':13, 'radio_id':123},"
        + "{'id':14, 'radio_id':124},"
        + "{'id':15, 'radio_id':125},"
        + "{'id':16, 'radio_id':126},"
        + "{'id':17, 'radio_id':127},"
        + "{'id':18, 'radio_id':128},"
        + "{'id':19, 'radio_id':129},"
        + "{'id':20, 'radio_id':130},"
        + "{'id':21, 'radio_id':131},"
        + "{'id':22, 'radio_id':132},"
        + "{'id':23, 'radio_id':133},"
        + "{'id':24, 'radio_id':134}"
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
     
        public TargetMgr()
        {

        }

        public void UpdateTragetList()
        {
            CEmployee[] emarr = JsonConvert.DeserializeObject(employee, typeof(CEmployee[])) as CEmployee[];
            foreach(CEmployee em in emarr)g_EmployeeList.Add(em.id, em);
            g_EmployeeList.Add(-1, null);

            CVehicle[] vearr = JsonConvert.DeserializeObject(vehicle, typeof(CVehicle[])) as CVehicle[];
            foreach (CVehicle ve in vearr) g_VehicleList.Add(ve.id, ve);
            g_VehicleList.Add(-1, null);

            CGroup[] gparr = JsonConvert.DeserializeObject(group, typeof(CGroup[])) as CGroup[];
            foreach (CGroup gp in gparr) g_GroupList.Add(gp.id, gp);
            g_GroupList.Add(-1, null);

            CRadio[] rdarr = JsonConvert.DeserializeObject(radio, typeof(CRadio[])) as CRadio[];
            foreach (CRadio rd in rdarr) g_RadioList.Add(rd.id, rd);
            g_RadioList.Add(-1, null);

            g_RelationshipList = JsonConvert.DeserializeObject(relationship, typeof(List<CRelationship>)) as List<CRelationship>;
        }

        public Dictionary<COrganization,List<COrganization>> GetOrgList()
        {
            Dictionary<COrganization, List<COrganization>> OrgList = new Dictionary<COrganization, List<COrganization>>();
            Dictionary<int, COrganization> GroupList = new Dictionary<int,COrganization>();

            int count = 0;

            foreach(var group in g_GroupList)
            {
                if (null == group.Value) continue;

                COrganization org = new COrganization{
                    index = count++,
                    target = new CRelationShipObj(OrgItemType.Type_Group, g_GroupList[group.Value.id], null, null, null),
                    is_exp = true,
                    };

                GroupList.Add(group.Value.id, org);
                OrgList.Add(org, new List<COrganization>());
            }

            foreach (CRelationship rela in g_RelationshipList)
            {
                if (-1 == rela.group)
                {                  
                    COrganization org = new COrganization{
                         index = count++,
                         target = new CRelationShipObj(OrgItemType.Type_Group, null, null, null, null),
                         is_exp = true,
                    };

                    GroupList.Add(-1, org);
                    OrgList.Add(org, new List<COrganization>());
                    break;
                }
            }

            foreach(var employee in g_EmployeeList)
            {
                bool isundefinedgroup = false;

                if (null == employee.Value) continue;
                foreach(CRelationship rela in g_RelationshipList)
                {
                    if (employee.Value.id == rela.employee)
                    {
                       isundefinedgroup = true;

                       CRelationShipObj target = new CRelationShipObj(OrgItemType.Type_Employee,g_GroupList[rela.group], g_EmployeeList[rela.employee], g_VehicleList[rela.vehicle], g_RadioList[rela.radio]);

                       OrgList[GroupList[rela.group]].Add(new COrganization
                       {
                            index = count++,
                            target = target,
                            is_exp = true,
                       });

                       break;
                    }                                          
                }

                if (false == isundefinedgroup)
                {
                    CRelationShipObj target = new CRelationShipObj(OrgItemType.Type_Employee, null, g_EmployeeList[employee.Value.id], null, null);

                    OrgList[GroupList[-1]].Add(new COrganization
                    {
                        index = count++,
                        target = target,
                        is_exp = true,
                    });
                }
            }


            foreach (var vehicle in g_VehicleList)
            {
                bool isundefinedgroup = false;

                if (null == vehicle.Value) continue;
                foreach (CRelationship rela in g_RelationshipList)
                {                   
                    if (vehicle.Value.id == rela.vehicle)
                    {
                        isundefinedgroup = true;

                        CRelationShipObj target = new CRelationShipObj(OrgItemType.Type_Vehicle,g_GroupList[rela.group], g_EmployeeList[rela.employee], g_VehicleList[rela.vehicle], g_RadioList[rela.radio]);

                        OrgList[GroupList[rela.group]].Add(new COrganization
                        {
                            index = count++,
                            target = target,
                            is_exp = true,
                        });

                        break;
                    }
                }

                if (false == isundefinedgroup)
                {
                    CRelationShipObj target = new CRelationShipObj(OrgItemType.Type_Vehicle,null, null, g_VehicleList[vehicle.Value.id], null);
                    OrgList[GroupList[-1]].Add(new COrganization
                    {
                        index = count++,
                        target = target,
                        is_exp = true,
                    });
                }
            }

            foreach (var radio in g_RadioList)
            {
                bool isundefinedgroup = false;

                if (null == radio.Value) continue;
                foreach (CRelationship rela in g_RelationshipList)
                {
                    if (radio.Value.id == rela.radio)
                    {
                        isundefinedgroup = true;
                        if ((-1 != rela.vehicle) || (-1 != rela.employee)) break;

                        CRelationShipObj target = new CRelationShipObj(g_RadioList[rela.radio].type == RadioType.RADIO ? OrgItemType.Type_Radio : OrgItemType.Type_Ride, g_GroupList[rela.group], g_EmployeeList[rela.employee], g_VehicleList[rela.vehicle], g_RadioList[rela.radio]);
                        OrgList[GroupList[rela.group]].Add(new COrganization
                        {
                            index = count++,
                            target = target,
                            is_exp = true,
                        });

                        break;
                    }
                }

                if (false == isundefinedgroup)
                {
                    CRelationShipObj target = new CRelationShipObj(g_RadioList[radio.Value.id].type == RadioType.RADIO ? OrgItemType.Type_Radio : OrgItemType.Type_Ride, null, null, null, g_RadioList[radio.Value.id]);

                    OrgList[GroupList[-1]].Add(new COrganization
                    {
                        index = count++,
                        target = target,
                        is_exp = true,
                    });
                }
            }

            return OrgList;
        }
    }
}
