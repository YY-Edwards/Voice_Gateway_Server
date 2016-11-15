using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

namespace TrboX
{

    public class Department
    {
        [DefaultValue((long)0)]
        public long id;
        public string name{ set; get; }

        [JsonProperty(PropertyName = "gid")]
        public long group_id { set; get; }

        [JsonIgnore]
        public string Name
        {
            get
            { return name + "(ID:"+group_id.ToString()+")"; }
        }
        private static Department Copy(Department dept)
        {
            return JsonConvert.DeserializeObject<Department>(JsonConvert.SerializeObject(dept));
        }

        public Department()
        { }

        public Department(string n, long id)
        {
            name = n;
            group_id = id;
        }

        public long Add()
        {
            return DepartmentMgr.Add(Copy(this));
        }

        public void Modify()
        {
            DepartmentMgr.Modify(id, Copy(this));
        }

        public void Delete()
        {
            DepartmentMgr.Delete(id);
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
            int count = Count();
            if (count <= 0) return null;
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
                return null;
            }


            try
            {

                List<Department> s_List = LogServer.Call(str, ParseList) as List<Department>;

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
            dept.id = 0;
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
                    dept.id = 0;
                    s_Add[Id] = dept;
                }
                else
                {
                    dept.id = 0;
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


            OrginIndex = CurrentIndex;
            s_Add.Clear();
            s_Del.Clear();
            s_Update.Clear();
        }

        
        private static Dictionary<long, long> AddDeptStaff = new Dictionary<long, long>();
        private static Dictionary<long, long> DelDeptStaff = new Dictionary<long, long>();
        public static void AssignStaff(long staff, long dept)
        {
            if (DelDeptStaff.ContainsKey(staff))
            {
                DelDeptStaff.Remove(staff);
            }          
            
            if (AddDeptStaff.ContainsKey(staff))
            {
                AddDeptStaff[staff] = dept;
            }
            else
            {
                AddDeptStaff.Add(staff, dept);
            }
        }

        public static void DetachStaff(long staff, long dept)
        {
            if (AddDeptStaff.ContainsKey(staff))
            {
                AddDeptStaff.Remove(staff);
            }

            if (DelDeptStaff.ContainsKey(staff))
            {
                DelDeptStaff[staff] = dept;
            }
            else
            {
                DelDeptStaff.Add(staff, dept);
            }
        }

        public static List<Staff> ListStaff(long dept)
        {
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

            return LogServer.Call(addstr) as List<Staff>;
        }

        public static void SaveDeptStaff()
        {
              foreach(var item in AddDeptStaff)
              {
                  Dictionary<string, object> addparam = new Dictionary<string, object>();
                  addparam.Add("operation", OperateType.assignUser.ToString());
                  addparam.Add("user", item.Key);
                  addparam.Add("department", item.Value);

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
                  addparam.Add("user", item.Key);
                  addparam.Add("department", item.Value);

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


        private static Dictionary<long, long> AddDeptRadio = new Dictionary<long, long>();
        private static Dictionary<long, long> DelDeptRadio = new Dictionary<long, long>();
        public static void AssignRadio(long radio, long dept)
        {
            if (DelDeptRadio.ContainsKey(radio))
            {
                DelDeptRadio.Remove(radio);
            }

            if (AddDeptRadio.ContainsKey(radio))
            {
                AddDeptRadio[radio] = dept;
            }
            else
            {
                AddDeptRadio.Add(radio, dept);
            }
        }

        public static void DetachRadio(long radio, long dept)
        {
            if (AddDeptRadio.ContainsKey(radio))
            {
                AddDeptRadio.Remove(radio);
            }

            if (DelDeptRadio.ContainsKey(radio))
            {
                DelDeptRadio[radio] = dept;
            }
            else
            {
                DelDeptRadio.Add(radio, dept);
            }
        }

        public static List<Radio> ListRadio(long dept)
        {
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

            return LogServer.Call(addstr, RadioMgr.ParseList) as List<Radio>;
        }

        public static void SaveDeptRadio()
        {
            foreach (var item in AddDeptRadio)
            {
                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.assignUser.ToString());
                addparam.Add("radio", item.Key);
                addparam.Add("department", item.Value);

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
                addparam.Add("operation", OperateType.detachUser.ToString());
                addparam.Add("radio", item.Key);
                addparam.Add("department", item.Value);

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
