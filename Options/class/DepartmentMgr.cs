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
    [Serializable]
    public class Department
    {
        [DefaultValue((long)0), JsonProperty(PropertyName = "id")]
        public long ID{ set; get; }

        [JsonProperty(PropertyName = "name")]
        public string Name{ set; get; }

        [JsonProperty(PropertyName = "gid")]
        public long GroupID { set; get; }

         [JsonIgnore]
        public bool IsTx { set; get; }

        [JsonIgnore]
        public bool IsRx { set; get; }

        [JsonIgnore]
        public string NameInfo
        {
            get
            { return Name + "(ID:" + GroupID.ToString() + ")"; }
        }
        private static Department Copy(Department dept)
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, dept);
            stream.Seek(0, SeekOrigin.Begin);
            return (Department)new BinaryFormatter().Deserialize(stream);
        }

        public Department()
        { }

        public Department(string n, long id)
        {
            Name = n;
            GroupID = id;
        }

        public long Add()
        {
            return DepartmentMgr.Add(Copy(this));
        }

        public void Modify()
        {
            DepartmentMgr.Modify(ID, Copy(this));
        }

        public void Delete()
        {
            DepartmentMgr.Delete(ID);
        }
    }

    public class UpdatesDepartment
    {
        public long id;
        public Department department;
    }
    class DepartmentMgr
    {
        private static long OrginIndex = 0;
        private static long CurrentIndex = 0;

        private static Dictionary<long, Department> s_Add = new Dictionary<long, Department>();
        private static List<long> s_Del = new List<long>();
        private static List<UpdatesDepartment> s_Update = new List<UpdatesDepartment>();

        private static List<Department> DeptList = new List<Department>();
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
                call = RequestType.department.ToString(),
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

        public static List<Department> List()
        {

            if (!IsNeedUpdate) return DeptList;
            DeptList = null;
            IsNeedUpdate = false;
            int count = Count();
            if (count <= 0) return DeptList;
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = count
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.department.ToString(),
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
                return DeptList;
            }


            try
            {

                DeptList = LogServer.Call(str, ParseList) as List<Department>;

                if (DeptList == null) return null;
                OrginIndex = DeptList.Select(w => w.ID).Max();
                CurrentIndex = OrginIndex;

                s_Add.Clear();
                s_Del.Clear();
                s_Update.Clear();

                return DeptList;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return DeptList;
            }
        }
        public static object ParseList(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<Department>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<Department>>>(JsonConvert.SerializeObject(rep.contents));
                List<Department> res = Dic["records"];

                return res;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        public static long Add(Department dept)
        {
            dept.ID = 0;
            s_Add.Add(++CurrentIndex, dept);
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

        public static void Modify(long Id, Department dept)
        {
            try
            {
                if (Id > OrginIndex)
                {
                    dept.ID = 0;
                    s_Add[Id] = dept;
                }
                else
                {
                    dept.ID = 0;
                    s_Update.Add(new UpdatesDepartment() { id = Id, department = dept });
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
                delparam.Add("departments", s_Del);

                LogServerRequest delreq = new LogServerRequest()
                {
                    call = RequestType.department.ToString(),
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
                updateparam.Add("departments", s_Update);

                LogServerRequest updatereq = new LogServerRequest()
                {
                    call = RequestType.department.ToString(),
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
                List<Department> addlist = new List<Department>();
                foreach (var item in s_Add)
                {
                    addlist.Add(item.Value);
                }

                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.add.ToString());
                addparam.Add("departments", addlist);

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

            IsNeedUpdate = true;
            OrginIndex = CurrentIndex;
            s_Add.Clear();
            s_Del.Clear();
            s_Update.Clear();
        }


        private static Dictionary<Staff, Department> AddDeptStaff = new Dictionary<Staff, Department>();
        private static Dictionary<Staff, Department> DelDeptStaff = new Dictionary<Staff, Department>();

        private static Dictionary<long, List<Staff>> StaffDept = new Dictionary<long, List<Staff>>();
        private static bool IsNeedListStaff = true;
        public static void AssignStaff(long staff, long dept)
        {
            Staff staffs = null;
            if (StaffMgr.s_Add.ContainsKey(staff))
            {
                staffs = StaffMgr.s_Add[staff];
                staffs.ID = (int)staff;
            }

            List<Staff> tmpstaff = StaffMgr.SatffList.Where(p => p.ID == staff).ToList();
            if (tmpstaff.Count > 0) staffs = tmpstaff[0];

            if (staffs == null) return;

            Department depts = null;
            if (StaffMgr.s_Add.ContainsKey(dept))
            {
                depts = s_Add[dept];
                depts.ID = (int)dept;
            }

            List<Department> tmpdept = DeptList.Where(p => p.ID == dept).ToList();
            if (tmpdept.Count > 0) depts = tmpdept[0];

            if (depts == null) return;



            if (DelDeptStaff.ContainsKey(staffs))
            {
                if (DelDeptStaff[staffs] == depts) DelDeptStaff.Remove(staffs);
            }

            if (AddDeptStaff.ContainsKey(staffs))
            {
                AddDeptStaff[staffs] = depts;
            }
            else
            {
                AddDeptStaff.Add(staffs, depts);
            }
        }

        public static void DetachStaff(long staff, long dept)
        {
            Staff staffs = null;
            if (StaffMgr.s_Add.ContainsKey(staff))
            {
                staffs = StaffMgr.s_Add[staff];
                staffs.ID = (int)staff;
            }

            List<Staff> tmp = StaffMgr.SatffList.Where(p => p.ID == staff).ToList();
            if (tmp.Count > 0) staffs = tmp[0];

            if (staffs == null) return;

            Department depts = null;
            if (StaffMgr.s_Add.ContainsKey(dept))
            {
                depts = s_Add[dept];
                depts.ID = (int)dept;
            }

            List<Department> tmpdept = DeptList.Where(p => p.ID == dept).ToList();
            if (tmpdept.Count > 0) depts = tmpdept[0];

            if (depts == null) return;

            if (AddDeptStaff.ContainsKey(staffs))
            {
                if (AddDeptStaff[staffs] == depts) AddDeptStaff.Remove(staffs);               
            }

            if (DelDeptStaff.ContainsKey(staffs))
            {
                DelDeptStaff[staffs] = depts;
            }
            else
            {
                DelDeptStaff.Add(staffs, depts);
            }
        }

        public static List<Staff> ListStaff(long dept)
        {
            if (!IsNeedListStaff && StaffDept.ContainsKey(dept)) return StaffDept[dept];
            IsNeedListStaff = false;

            Dictionary<string, object> addparam = new Dictionary<string, object>();
            addparam.Add("operation", OperateType.listUser.ToString());
            addparam.Add("department", dept);

            LogServerRequest addreq = new LogServerRequest()
            {
                call = RequestType.department.ToString(),
                callId = LogServer.CallId,
                param = addparam
            };

            JsonSerializerSettings jsetting = new JsonSerializerSettings();
            jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
            string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);


            List<Staff> Res = LogServer.Call(addstr, StaffMgr.ParseList) as List<Staff>;

            if (StaffDept.ContainsKey(dept))
            {
                StaffDept[dept] = Res;
            }
            else
            {
                StaffDept.Add(dept, Res);
            }

            return Res;
        }

        public static void SaveDeptStaff()
        {
              List<Staff> staffs = StaffMgr.List();
              List<Department> depts = List();

              foreach(var item in AddDeptStaff)
              {
                  Dictionary<string, object> addparam = new Dictionary<string, object>();
                  addparam.Add("operation", OperateType.assignUser.ToString());

                  List<Staff> tmp = staffs.Where(p => p.Name == item.Key.Name && p.PhoneNumber == item.Key.PhoneNumber).ToList();
                  if (tmp.Count < 1) continue;
                  addparam.Add("user", tmp[0].ID);


                  List<Department> tmpdept = depts.Where(p => p.GroupID == item.Value.GroupID).ToList();
                  if (tmpdept.Count < 1) continue;
                  addparam.Add("department", tmpdept[0].ID);

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
              foreach(var item in DelDeptStaff)
              {
                  Dictionary<string, object> addparam = new Dictionary<string, object>();
                  addparam.Add("operation", OperateType.detachUser.ToString());

                  List<Staff> tmp = staffs.Where(p => p.Name == item.Key.Name && p.PhoneNumber == item.Key.PhoneNumber).ToList();
                  if (tmp.Count < 1) continue;
                  addparam.Add("user", tmp[0].ID);

                  List<Department> tmpdept = depts.Where(p => p.GroupID == item.Value.GroupID).ToList();
                  if (tmpdept.Count < 1) continue;
                  addparam.Add("department", tmpdept[0].ID);

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

              IsNeedListStaff = true;
              AddDeptStaff.Clear();
              DelDeptStaff.Clear();
        }


        private static Dictionary<Radio, Department> AddDeptRadio = new Dictionary<Radio, Department>();
        private static Dictionary<Radio, Department> DelDeptRadio = new Dictionary<Radio, Department>();

        private static Dictionary<long, List<Radio>> RadioDept = new Dictionary<long, List<Radio>>();
        private static bool IsNeedListRadio = true;
        public static void AssignRadio(long radio, long dept)
        {
            Radio radios = null;
            if (RadioMgr.s_Add.ContainsKey(radio))
            {
                radios = RadioMgr.s_Add[radio];
                radios.ID = (int)radio;
            }

            List<Radio> tmp =RadioMgr.RadioList.Where(p => p.ID == radio).ToList();
            if (tmp.Count > 0) radios = tmp[0];

            if (radios == null) return;

            Department depts = null;
            if (StaffMgr.s_Add.ContainsKey(dept))
            {
                depts = s_Add[dept];
                depts.ID = (int)dept;
            }

            List<Department> tmpdept = DeptList.Where(p => p.ID == dept).ToList();
            if (tmpdept.Count > 0) depts = tmpdept[0];

            if (depts == null) return;

            if (DelDeptRadio.ContainsKey(radios))
            {
                if (DelDeptRadio[radios] == depts) DelDeptRadio.Remove(radios);
            }

            if (AddDeptRadio.ContainsKey(radios))
            {
                AddDeptRadio[radios] = depts;
            }
            else
            {
                AddDeptRadio.Add(radios, depts);
            }
        }

        public static void DetachRadio(long radio, long dept)
        {
            Radio radios = null;
            if (RadioMgr.s_Add.ContainsKey(radio))
            {
                radios = RadioMgr.s_Add[radio];
                radios.ID = (int)radio;
            }

            List<Radio> tmp = RadioMgr.RadioList.Where(p => p.ID == radio).ToList();
            if (tmp.Count > 0) radios = tmp[0];

            if (radios == null) return;

            Department depts = null;
            if (StaffMgr.s_Add.ContainsKey(dept))
            {
                depts = s_Add[dept];
                depts.ID = (int)dept;
            }

            List<Department> tmpdept = DeptList.Where(p => p.ID == dept).ToList();
            if (tmpdept.Count > 0) depts = tmpdept[0];

            if (depts == null) return;


            if (AddDeptRadio.ContainsKey(radios))
            {
                if (AddDeptRadio[radios] == depts) AddDeptRadio.Remove(radios);
            }

            if (DelDeptRadio.ContainsKey(radios))
            {
                DelDeptRadio[radios] = depts;
            }
            else
            {
                DelDeptRadio.Add(radios, depts);
            }
        }

        public static List<Radio> ListRadio(long dept)
        {
            if (!IsNeedListRadio && RadioDept.ContainsKey(dept)) return RadioDept[dept];
            IsNeedListRadio = false;

            Dictionary<string, object> addparam = new Dictionary<string, object>();
            addparam.Add("operation", OperateType.listRadio.ToString());
            addparam.Add("department", dept);

            LogServerRequest addreq = new LogServerRequest()
            {
                call = RequestType.department.ToString(),
                callId = LogServer.CallId,
                param = addparam
            };

            JsonSerializerSettings jsetting = new JsonSerializerSettings();
            jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
            string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

            List<Radio>  Res = LogServer.Call(addstr, RadioMgr.ParseList) as List<Radio>;

            if (RadioDept.ContainsKey(dept))
            {
                 RadioDept[dept] = Res;
            }
            else{
                RadioDept.Add(dept, Res);
            }

            return Res;
        }

        public static void SaveDeptRadio()
        {
            List<Radio> radios = RadioMgr.List();
            List<Department> depts = List();

            foreach (var item in AddDeptRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.assignRadio.ToString());

                List<Radio> tmp = radios.Where(p => p.RadioID == item.Key.RadioID).ToList();
                if (tmp.Count < 1) continue;
                addparam.Add("radio", tmp[0].ID);

                List<Department> tmpdept = depts.Where(p => p.GroupID == item.Value.GroupID).ToList();
                if (tmpdept.Count < 1) continue;
                addparam.Add("department", tmpdept[0].ID);

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
            foreach (var item in DelDeptRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.detachRadio.ToString());

                List<Radio> tmp = radios.Where(p => p.RadioID == item.Key.RadioID).ToList();
                if (tmp.Count < 1) continue;
                addparam.Add("radio", tmp[0].ID);

                List<Department> tmpdept = depts.Where(p => p.GroupID == item.Value.GroupID).ToList();
                if (tmpdept.Count < 1) continue;
                addparam.Add("department", tmpdept[0].ID);

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

            IsNeedListRadio = true;
            AddDeptRadio.Clear();
            DelDeptRadio.Clear();
        }
    }
}
