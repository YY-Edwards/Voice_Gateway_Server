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

namespace Manager
{
    public class CRadioMgr : CResource
    {
        private List<CRElement> m_Departments;
        private List<CRElement> m_Staffs;

        private bool m_IsNeedAssgin = false;

        private Dictionary<long, long> m_AssignDept;//radioid, groupid
        private Dictionary<long, long> m_DetachDept;//id, id

        private Dictionary<long, string> m_AssignStaff;//radioid, staffname
        private Dictionary<long, long> m_DetachStaff;//id, id

        public CRadioMgr()
            : base
                (RequestOpcode.radio)
        {
            List.Add(new CRadio() { RadioID = 3,Type = RadioType.Ride});
        }

        public override CRElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CRadio>(json);
            }
            catch
            {
                return null;
            }
        }

        public override void ListUpdate()
        {
            if (!m_IsNeedAssgin) return;

            if (m_AssignDept != null && m_AssignDept.Count > 0)
                foreach (var dic in m_AssignDept)
                {


                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.assignRadio.ToString());

                    CRElement radio = List.Find(p => ((CRadio)p).RadioID == dic.Key);
                    if (radio == null) continue;

                    m_AssignDept.Remove(dic.Key);

                    param.Add("radio", radio.ID);

                    CRElement department = m_Departments.Find(p => ((CDepartment)p).GroupID == dic.Value);
                    if (department == null) continue;
                    param.Add("department", department.ID);

                    Request(RequestOpcode.department, OperateType.assignRadio, param);
                }


            if (m_AssignStaff != null && m_AssignStaff.Count > 0)
                foreach (var dic in m_AssignStaff)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.assignRadio.ToString());

                    CRElement radio = List.Find(p => ((CRadio)p).RadioID == dic.Key);
                    if (radio == null) continue;
                    m_AssignDept.Remove(dic.Key);

                    param.Add("radio", radio.ID);

                    CRElement staff = m_Staffs.Find(p => ((CStaff)p).Name == dic.Value);
                    if (staff == null) continue;
                    param.Add("user", staff.ID);

                    Request(RequestOpcode.user, OperateType.assignRadio, param);

                }


            if (m_AssignDept != null && m_AssignDept.Count <= 0 && m_AssignStaff != null && m_AssignStaff.Count <= 0)
            {
                m_IsNeedAssgin = false;
                List.Clear();
            }
                        
                     
        }

        public void AssignDetach(List<CRElement> departments, List<CRElement> staffs)
        {

            if (m_DetachDept != null)
            {
                foreach (var dic in m_DetachDept)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.detachRadio.ToString());
                    param.Add("radio", dic.Key);
                    param.Add("department", dic.Value);

                    Request(RequestOpcode.department, OperateType.detachRadio, param);
                }
                m_DetachDept.Clear();
            }


            if (m_DetachStaff != null)
            {
                foreach (var dic in m_DetachStaff)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", OperateType.detachRadio.ToString());
                    param.Add("user", dic.Key);
                    param.Add("radio", dic.Value);

                    Request(RequestOpcode.user, OperateType.detachRadio, param);
                }
                m_DetachStaff.Clear();
            }
            

            if ((m_AssignDept != null && m_AssignDept.Count > 0) || (m_AssignStaff != null && m_AssignStaff.Count > 0))
            {
                m_Departments = departments;
                m_Staffs = staffs;
                m_IsNeedAssgin = true;
                Query();
            }
        }


        public void AssignDept(CRadio radio, long groupid)
        {
            if (radio == null) return;
            if (m_DetachDept == null) m_DetachDept = new Dictionary<long, long>();
            if (m_DetachDept.ContainsKey(radio.ID))
            {
                m_DetachDept.Remove(radio.ID);
            }

            if (m_AssignDept == null) m_AssignDept = new Dictionary<long, long>();
            if (m_AssignDept.ContainsKey(radio.RadioID))
            {
                m_AssignDept[radio.RadioID] = groupid;
            }
            else
            {
                m_AssignDept.Add(radio.RadioID, groupid);
            }          
        }

        public void DetachDept(CRadio radio, long departmentid)
        {
            if (radio == null) return;
            if (m_AssignDept == null) m_AssignDept = new Dictionary<long, long>();
            if (m_AssignDept.ContainsKey(radio.RadioID))
            {
                m_AssignDept.Remove(radio.RadioID);               
            }

            if (m_DetachDept == null) m_DetachDept = new Dictionary<long, long>();
            if (m_DetachDept.ContainsKey(radio.ID))
            {
                m_DetachDept[radio.ID] = departmentid;
            }
            else
            {
                m_DetachDept.Add(radio.ID, departmentid);
            }           
        }

        public void AssignStaff(CRadio radio, string staffname)
        {
            if (radio == null) return;
            if (m_DetachStaff == null) m_DetachStaff = new Dictionary<long, long>();
            if (m_DetachStaff.ContainsKey(radio.ID))
            {
                m_DetachStaff.Remove(radio.ID);
            }

            if (m_AssignStaff == null) m_AssignStaff = new Dictionary<long, string>();
            if (m_AssignStaff.ContainsKey(radio.RadioID))
            {
                m_AssignStaff[radio.RadioID] = staffname;
            }
            else
            {
                m_AssignStaff.Add(radio.RadioID, staffname);
            }
        }

        public void DetachStaff(CRadio radio, long id)
        {
            if (radio == null) return;
            if (m_AssignStaff == null) m_AssignStaff = new Dictionary<long, string>();
            if (m_AssignStaff.ContainsKey(radio.RadioID))
            {
                m_AssignStaff.Remove(radio.RadioID);
            }

            if (m_DetachStaff == null) m_DetachStaff = new Dictionary<long, long>();
            if (m_DetachStaff.ContainsKey(radio.ID))
            {
                m_DetachStaff[radio.ID] = id;
            }
            else
            {
                m_DetachStaff.Add(radio.ID, id);
            }
        }
    }

    public enum RadioType
    {
        Radio = 0,
        Ride = 1
    };

    [Serializable]
    public class CRadio : CRElement
    {

        [JsonProperty(PropertyName = "radio_id")]
        public long RadioID { set; get; }
        [JsonProperty(PropertyName = "type")]
        public RadioType Type { set; get; }

        [JsonProperty(PropertyName = "screen")]
        public int Screen;

        [JsonIgnore]
        public bool HasScreen
        {
            set { Screen = value ? 1 : 0; }
            get { return Screen == 0 ? false : true; }
        }

        [JsonProperty(PropertyName = "gps")]
        public int GPS;

        [JsonIgnore]
        public bool HasGPS
        {
            set
            {
                if (value) GPS |= 1;
                else GPS &= ~1;
            }
            get { return (GPS & 1) == 1 ? true : false ; }
        }

        [JsonIgnore]
        public bool HasLocationInDoor
        {
            set {
                if (value) GPS |= 2;
                else GPS &= ~2;           
            }
            get { return (GPS & 2) == 2 ? true : false; }
        }

        [JsonProperty(PropertyName = "keyboard")]
        public int Keyboard;

        [JsonIgnore]
        public bool HasKeyboard
        {
            set { Keyboard = value ? 1 : 0; }
            get { return Keyboard == 0 ? false : true; }
        }

        [JsonProperty(PropertyName = "sn")]
        public string SN { set; get; }

        [JsonProperty(PropertyName = "valid")]
        public int Valid;


        [JsonProperty(PropertyName = "department")]
        public long DepartmentID { get; set; }

        [JsonProperty(PropertyName = "staff")]
        public long StaffID { get; set; }


        [JsonIgnore]
        public bool IsValid
        {
            set { Valid = value ? 1 : 0; }
            get { return Valid == 0 ? false : true; }
        }

        public CRadio()
        {
            Type = RadioType.Radio;
            SN = string.Empty;
        }

        public CRadio Copy()
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, this);
            stream.Seek(0, SeekOrigin.Begin);
            return (CRadio)new BinaryFormatter().Deserialize(stream);
        }
    } 
}
