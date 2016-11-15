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
        [JsonIgnore]
        public long id;
        public string name{ set; get; }

        [JsonProperty(PropertyName = "id")]
        public long group_id { set; get; }

        [JsonIgnore]
        public string Name
        {
            get
            { return name + "(ID:"+group_id.ToString()+")"; }
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
            return DepartmentMgr.Add(this);
        }

        public void Modify()
        {
            DepartmentMgr.Modify(id, this);
        }

        public void Delete()
        {
            DepartmentMgr.Delete(id);
        }
    }

    public class UpdatesDepartment
    {
        public long id;
        public Department dept;
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

            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = Count()
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
                Dictionary<string, List<User>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<User>>>(JsonConvert.SerializeObject(rep.contents));
                List<User> res = Dic["records"];

                for (int i = 0; i < res.Count; i++)
                {
                    res[i].Auth = res[i].parseauth();
                }

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
                    s_Update.Add(new UpdatesDepartment() { id = Id, dept = dept });
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
    }
}
