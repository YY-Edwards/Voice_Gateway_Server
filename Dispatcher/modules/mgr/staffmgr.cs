using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using Dispatcher.Service;
namespace Dispatcher.Modules
{
    public class CStaffMgr : CResource
    {
        private List<CElement> m_Departments;
        private bool m_IsNeedAssgin = false;
        private Dictionary<string, long> m_Assign;
        private Dictionary<long, long> m_Detach;

        public CStaffMgr()
            : base
                (RequestOpcode.staff)
        {
            List.Add(new CStaff() { Name = "test" });
        }

       

        public override CElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CStaff>(json);
            }
            catch
            {
                return null;
            }
        }

        public override void ListUpdate()
        {
            if (m_IsNeedAssgin && m_Assign != null)
            {
                foreach (var dic in m_Assign)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.assignUser.ToString());

                    CElement staff = List.Find(p => ((CStaff)p).Name == dic.Key);
                    if (staff == null) continue;
                    param.Add("user", staff.ID);

                    CElement department = m_Departments.Find(p => ((CDepartment)p).GroupID == dic.Value);
                    if (department == null) continue;
                    param.Add("department", department.ID);

                    Request(RequestOpcode.department, OperateType.assignUser, param);

                    m_Assign.Remove(dic.Key);
                }
                if (m_Assign.Count <= 0)
                {
                    m_IsNeedAssgin = false;
                    Query();
                }                               
            }            
        }

        public void AssignDetach(List<CElement> departments)
        {
            if (m_Detach != null)
            {
                foreach (var dic in m_Detach)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.detachUser.ToString());
                    param.Add("user", dic.Key);
                    param.Add("department", dic.Value);

                    Request(RequestOpcode.department, OperateType.detachUser, param);
                }
                m_Detach.Clear();
            }

            if (m_Assign != null && m_Assign.Count > 0)
            {
                m_Departments = departments;
                m_IsNeedAssgin = true;
                Query();
            }
        }


        public void Assign(CStaff staff, long groupid)
        {
            if (m_Detach == null) m_Detach = new Dictionary<long, long>();
            if (m_Detach.ContainsKey(staff.ID))
            {
                m_Detach.Remove(staff.ID);
            }

            if (m_Assign == null) m_Assign = new Dictionary<string, long>();
            if (m_Assign.ContainsKey(staff.Name))
            {
                m_Assign[staff.Name] = groupid;
            }
            else
            {
                m_Assign.Add(staff.Name, groupid);
            }          
        }

        public void Detach(CStaff staff, long departmentid)
        {
            if (staff == null) return;
            if (m_Assign == null) m_Assign = new Dictionary<string, long>();
            if (m_Assign.ContainsKey(staff.Name))
            {
                m_Assign.Remove(staff.Name);               
            }

            if (m_Detach == null) m_Detach = new Dictionary<long, long>();
            if (m_Detach.ContainsKey(staff.ID))
            {
                m_Detach[staff.ID] = departmentid;
            }
            else
            {
                m_Detach.Add(staff.ID, departmentid);
            }           
        }
      

    }

    public enum StaffType
    {
        Staff = 0,
        Vehicle = 1,
    };

    public class CStaff : CElement
    {

        [JsonProperty(PropertyName = "name")]
        public string Name { get; set; }

        [JsonProperty(PropertyName = "type")]
        public StaffType Type
        {
            get
            {
                return IsValid ? StaffType.Staff : StaffType.Vehicle;
            }
            set
            {
                IsValid = value == StaffType.Staff ? true : false;
            }
        }

        [JsonProperty(PropertyName = "phone")]
        public string PhoneNumber { get; set; }

        [JsonProperty(PropertyName = "valid")]
        public int Valid;


        [JsonIgnore]
        public bool IsValid
        {
            set { Valid = value ? 1 : 0; }
            get { return Valid == 0 ? false : true; }
        }

        [JsonProperty(PropertyName = "department")]
        public int DepartmentID{get;set;}


        public CStaff()
        {
            Valid = 1;
            PhoneNumber = string.Empty;
        }
    
    }
}
