using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

namespace TrboX
{
   
    public enum StaffType
    {
        Staff = 0,
        Vehicle = 1,
    };

    [Serializable]
    public class Staff
    {
        [DefaultValue((int)0), JsonProperty(PropertyName = "id")]
        public int ID;
        [JsonProperty(PropertyName = "name")]
        public string Name{ get; set; }

        [JsonProperty(PropertyName = "type")]
        public StaffType Type { 
            get
            {
                return IsValid ? StaffType.Staff : StaffType.Vehicle;  }
            set
            {
                IsValid = value == StaffType.Staff ? true : false;
            }
        }

        [JsonProperty(PropertyName = "phone")]
        public string PhoneNumber{ get; set; }

         [JsonIgnore]
        public int User;

        [JsonProperty(PropertyName = "valid")]
        public int Valid;


        [JsonIgnore]
        public bool IsValid
        {
            set { Valid = value ? 1 : 0; }
            get { return Valid == 0 ? false : true; }
        }

        [JsonIgnore]
        public string NameInfo
        {
            get
            { return Name + (Type == StaffType.Vehicle ? "(车辆)": "(人员)"); }
        }

        private static Staff Copy(Staff dept)
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, dept);
            stream.Seek(0, SeekOrigin.Begin);
            return (Staff)new BinaryFormatter().Deserialize(stream);
        }

        public Staff()
        { }

        public long Add()
        {
            return StaffMgr.Add(Copy(this));
        }

        public void Modify()
        {
            StaffMgr.Modify(ID, Copy(this));
        }

        public void Delete()
        {
            StaffMgr.Delete(ID);
        }
    }
    
    public class UpdatesStaff
    {
        public long id;
        public Staff staff;
    }

    public class StaffMgr
    {
        private static long OrginIndex = 0;
        private static long CurrentIndex = 0;

        public static Dictionary<long, Staff> s_Add = new Dictionary<long, Staff>();
        private static List<long> s_Del = new List<long>();

        private static List<UpdatesStaff> s_Update = new List<UpdatesStaff>();

        public static List<Staff> SatffList = new List<Staff>();
        private static bool IsNeedUpdate = true;

        public static int Count()
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.count.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = -1,
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.staff.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return 0;
            }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

        public static object ParseCount(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, int> Dic = JsonConvert.DeserializeObject<Dictionary<string, int>>(JsonConvert.SerializeObject(rep.contents));
                return Dic["count"];
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

        public static List<Staff> List()
        {
            if (!IsNeedUpdate) return SatffList;
            IsNeedUpdate = false;
            int count = Count();
            if (count < 1) { SatffList = null; return null; };
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = count
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.staff.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                SatffList = null;
                return null;
            }


            try
            {

                SatffList = LogServer.Call(str, ParseList) as List<Staff>;

                if (SatffList == null) {
                    SatffList = null; return null;
                };
                OrginIndex = SatffList.Select(w => w.ID).Max();
                CurrentIndex = OrginIndex;

                s_Add.Clear();
                s_Del.Clear();
                s_Update.Clear();

                return SatffList;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                SatffList = null;
                return null;
            }
        }
        public static object ParseList(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<Staff>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<Staff>>>(JsonConvert.SerializeObject(rep.contents));
                List<Staff> res = Dic["records"];

                return res;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        public static long Add(Staff staff)
        {
            staff.ID = 0;
            s_Add.Add(++CurrentIndex, staff);
            return CurrentIndex;
        }

        public static void Delete(long Id)
        {
            try
            {

                if (Id > OrginIndex)
                {
                    s_Add.Remove(Id);
                }
                else
                {
                    if (!s_Del.Contains(Id)) s_Del.Add(Id);
                }
            }
            catch
            {

            }
        }

        public static void Modify(long Id,Staff staff)
        {
            try
            {
                if (Id > OrginIndex)
                {
                    staff.ID = 0;
                    s_Add[Id] = staff;
                }
                else
                {
                    staff.ID = 0;
                    s_Update.Add(new UpdatesStaff() { id = Id, staff = staff });
                }
            }
            catch
            {

            }
        }

        public static void Save()
        {

            if (s_Del.Count > 0)
            {
                Dictionary<string, object> delparam = new Dictionary<string, object>();
                delparam.Add("operation", OperateType.del.ToString());
                delparam.Add("staffs", s_Del);

                LogServerRequest delreq = new LogServerRequest()
                {
                    call = RequestType.staff.ToString(),
                    callId = LogServer.CallId,
                    param = delparam
                };
                string delstr = JsonConvert.SerializeObject(delreq);
                LogServer.Call(delstr);
            }

            if (s_Update.Count > 0)
            {
                Dictionary<string, object> updateparam = new Dictionary<string, object>();
                updateparam.Add("operation", OperateType.update.ToString());
                updateparam.Add("staffs", s_Update);

                LogServerRequest updatereq = new LogServerRequest()
                {
                    call = RequestType.staff.ToString(),
                    callId = LogServer.CallId,
                    param = updateparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string updatestr = JsonConvert.SerializeObject(updatereq, Formatting.Indented, jsetting);

                LogServer.Call(updatestr);
            }


            if (s_Add.Count > 0)
            {
                List<Staff> addlist = new List<Staff>();
                foreach (var item in s_Add)
                {
                    addlist.Add(item.Value);
                }

                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.add.ToString());
                addparam.Add("staffs", addlist);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.staff.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }

            IsNeedUpdate = true;
            OrginIndex = CurrentIndex;
            s_Add.Clear();
            s_Del.Clear();
            s_Update.Clear();
        }

        private static Dictionary<Staff, Radio> AddStaffRadio = new Dictionary<Staff, Radio>();
        private static Dictionary<Staff, Radio> DelStaffRadio = new Dictionary<Staff, Radio>();

        private static Dictionary<long, List<Radio>> StaffRadio = new Dictionary<long, List<Radio>>();
        private static bool IsNeedListRadio = true;
        public static void AssignRadio(long radio, long staff)
        {
            Staff staffs = null;
            if (StaffMgr.s_Add.ContainsKey(staff))
            {
                staffs = StaffMgr.s_Add[staff];
                staffs.ID = (int)staff;
            }

            if (StaffMgr.SatffList != null)
            {
                List<Staff> tmpstaff = StaffMgr.SatffList.Where(p => p.ID == staff).ToList();
                if (tmpstaff.Count > 0) staffs = tmpstaff[0];
            }

            if (staffs == null) return;


            Radio radios = null;
            if (RadioMgr.s_Add.ContainsKey(radio))
            {
                radios = RadioMgr.s_Add[radio];
                radios.ID = (int)radio;
            }

            if (RadioMgr.RadioList != null)
            {
                List<Radio> tmpradio = RadioMgr.RadioList.Where(p => p.ID == radio).ToList();
                if (tmpradio.Count > 0) radios = tmpradio[0];
            }

            if (radios == null) return;


            if (DelStaffRadio.ContainsKey(staffs))
            {
                if (DelStaffRadio[staffs] == radios) DelStaffRadio.Remove(staffs);
            }

            if (AddStaffRadio.ContainsKey(staffs))
            {
                AddStaffRadio[staffs] = radios;
            }
            else
            {
                AddStaffRadio.Add(staffs, radios);
            }
        }

        public static void DetachRadio(long radio, long staff)
        {
            Staff staffs = null;
            if (StaffMgr.s_Add.ContainsKey(staff))
            {
                staffs = StaffMgr.s_Add[staff];
                staffs.ID = (int)staff;
            }

            if (StaffMgr.SatffList != null)
            {
                List<Staff> tmpstaff = StaffMgr.SatffList.Where(p => p.ID == staff).ToList();
                if (tmpstaff.Count > 0) staffs = tmpstaff[0];
            }

            if (staffs == null) return;


            Radio radios = null;
            if (RadioMgr.s_Add.ContainsKey(radio))
            {
                radios = RadioMgr.s_Add[radio];
                radios.ID = (int)radio;
            }

            if (RadioMgr.RadioList != null)
            {
                List<Radio> tmpradio = RadioMgr.RadioList.Where(p => p.ID == radio).ToList();
                if (tmpradio.Count > 0) radios = tmpradio[0];
            }

            if (radios == null) return;


            if (AddStaffRadio.ContainsKey(staffs))
            {
                if (AddStaffRadio[staffs] == radios) AddStaffRadio.Remove(staffs);
            }

            if (DelStaffRadio.ContainsKey(staffs))
            {
                DelStaffRadio[staffs] = radios;
            }
            else
            {
                DelStaffRadio.Add(staffs, radios);
            }
        }

        public static List<Radio> ListRadio(long staff)
        {

            if (!IsNeedListRadio && StaffRadio.ContainsKey(staff)) return StaffRadio[staff];
            IsNeedListRadio = false;
            Dictionary<string, object> addparam = new Dictionary<string, object>();
            addparam.Add("operation", OperateType.listRadio.ToString());
            addparam.Add("user", staff);

            LogServerRequest addreq = new LogServerRequest()
            {
                call = RequestType.user.ToString(),
                callId = LogServer.CallId,
                param = addparam
            };

            JsonSerializerSettings jsetting = new JsonSerializerSettings();
            jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
            string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

            List<Radio> Res = LogServer.Call(addstr, RadioMgr.ParseList) as List<Radio>;

            if (StaffRadio.ContainsKey(staff))
            {
                StaffRadio[staff] = Res;
            }
            else
            {
                StaffRadio.Add(staff, Res);
            }

            return Res;
        }

        public static void SaveStaffRadio()
        {
            List<Staff> staffs = StaffMgr.List();
            List<Radio> radios = RadioMgr.List();

            foreach (var item in AddStaffRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.assignRadio.ToString());

                List<Staff> tmpstaff = staffs.Where(p => p.Name == item.Key.Name && p.PhoneNumber == item.Key.PhoneNumber).ToList();
                if (tmpstaff.Count < 1) continue;
                addparam.Add("user", tmpstaff[0].ID);


                List<Radio> tmpradio = radios.Where(p => p.RadioID == item.Value.RadioID).ToList();
                if (tmpradio.Count < 1) continue;
                addparam.Add("radio", tmpradio[0].ID);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.user.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }
            foreach (var item in DelStaffRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.detachRadio.ToString());

                List<Staff> tmpstaff = staffs.Where(p => p.Name == item.Key.Name && p.PhoneNumber == item.Key.PhoneNumber).ToList();
                if (tmpstaff.Count < 1) continue;
                addparam.Add("user", tmpstaff[0].ID);


                List<Radio> tmpradio = radios.Where(p => p.RadioID == item.Value.RadioID).ToList();
                if (tmpradio.Count < 1) continue;
                addparam.Add("radio", tmpradio[0].ID);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.user.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }

            IsNeedListRadio = true;
        }
    }
}
