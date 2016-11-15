using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;


namespace TrboX
{
   
    public enum StaffType
    {
        Vehicle,
        Staff
    };

    public class Staff
    {
        public int id;
        public string name { get; set; }
        public StaffType type { get; set; }
        public string phone { get; set; }
        public int user;

        [JsonProperty(PropertyName = "valid")]
        public int is_valid;

        [JsonIgnore]
        public bool valid
        {
            set { is_valid = value ? 1 : 0; }
            get { return is_valid == 0 ? false : true; }
        }


        [JsonIgnore]
        public string Name
        {
            get
            { return name; }
        }

        public Staff()
        { }

        public long Add()
        {
            return StaffMgr.Add(this);
        }

        public void Modify()
        {
            StaffMgr.Modify(id, this);
        }

        public void Delete()
        {
           StaffMgr.Delete(id);
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

        private static Dictionary<long, Staff> s_Add = new Dictionary<long, Staff>();
        private static List<long> s_Del = new List<long>();

        private static List<UpdatesStaff> s_Update = new List<UpdatesStaff>();


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
            int count = Count();
            if(count <= 1)return null;
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
                return null;
            }


            try
            {

                List<Staff> s_List = LogServer.Call(str, ParseList) as List<Staff>;

                if (s_List == null) return null;
                OrginIndex = s_List.Select(w => w.id).Max();
                CurrentIndex = OrginIndex;

                s_Add.Clear();
                s_Del.Clear();
                s_Update.Clear();

                return s_List;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
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
            staff.id = 0;
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
                    staff.id = 0;
                    s_Add[Id] = staff;
                }
                else
                {
                    staff.id = 0;
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
                    call = RequestType.user.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }


            OrginIndex = CurrentIndex;
            s_Add.Clear();
            s_Del.Clear();
            s_Update.Clear();
        }

        private static Dictionary<long, long> AddStaffRadio = new Dictionary<long, long>();
        private static Dictionary<long, long> DelStaffRadio = new Dictionary<long, long>();
        public static void AssignStaff(long staff, long radio)
        {
            if (DelStaffRadio.ContainsKey(staff))
            {
                DelStaffRadio.Remove(staff);
            }

            if (AddStaffRadio.ContainsKey(staff))
            {
                AddStaffRadio[staff] = radio;
            }
            else
            {
                AddStaffRadio.Add(staff, radio);
            }
        }

        public static void DetachStaff(long staff, long radio)
        {
            if (AddStaffRadio.ContainsKey(staff))
            {
                AddStaffRadio.Remove(staff);
            }

            if (DelStaffRadio.ContainsKey(staff))
            {
                DelStaffRadio[staff] = radio;
            }
            else
            {
                DelStaffRadio.Add(staff, radio);
            }
        }

        public static List<Radio> ListRadio(long staff)
        {
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

            return LogServer.Call(addstr, StaffMgr.ParseList) as List<Radio>;
        }

        public static void SaveStaffRadio()
        {
            foreach (var item in AddStaffRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.assignRadio.ToString());
                addparam.Add("user", item.Key);
                addparam.Add("radio", item.Value);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.department.ToString(),
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
                addparam.Add("user", item.Key);
                addparam.Add("radio", item.Value);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.department.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }
        }
    }
}
