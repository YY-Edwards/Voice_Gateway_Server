﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

namespace TrboX
{    
  
    public enum UserType
    {
        Admin,
        Guest
    }

    public enum AuthorityType
    {
        Call,
        Message,
        Position,
        Control,

    }

    public class CAuthority
    {
        public string Type;
        public string Dept { set; get; }
        public string Icon;
    }


    public class User
    {
        private static string AllAudthStr = "["
        + "{             'Type': 'Call',             'Dept': '呼叫',       'Icon': ''          },"
        + "{             'Type': 'Message',          'Dept': '短信',       'Icon': ''          },"
        + "{             'Type': 'Position',         'Dept': '定位',       'Icon': ''          },"
        + "{             'Type': 'Tracker',          'Dept': '巡更追踪',   'Icon': ''          },"
        + "{             'Type': 'JobTicket',        'Dept': '工单',       'Icon': ''          }"

        + "]";

        private static string AdminAuthStr = "["
        + "{             'Type': 'Call',             'Dept': '呼叫',       'Icon': ''          },"
        + "{             'Type': 'Message',          'Dept': '短信',       'Icon': ''          },"
        + "{             'Type': 'Position',         'Dept': '定位',       'Icon': ''          },"
        + "{             'Type': 'Tracker',          'Dept': '巡更追踪',   'Icon': ''          },"
        + "{             'Type': 'JobTicket',        'Dept': '工单',       'Icon': ''          }"

        + "]";

        private static string GuestAuthStr = "["
         + " {            'Type': 'Call',            'Dept': '呼叫',            'Icon': ''        },"
         + " {            'Type': 'Message',         'Dept': '短信',            'Icon': ''        },"
         + " {            'Type': 'Position',        'Dept': '定位',            'Icon': ''        },"
         + " {            'Type': 'Tracker',         'Dept': '巡更追踪',        'Icon': ''        },"
         + " {            'Type': 'JobTicket',       'Dept': '工单',            'Icon': ''        }"
         + "]";


        public static List<CAuthority> AddAuth = JsonConvert.DeserializeObject<List<CAuthority>>(AllAudthStr);
        private static List<CAuthority> GuestAuth = JsonConvert.DeserializeObject<List<CAuthority>>(AdminAuthStr);
        private static List<CAuthority> AdminAuth = JsonConvert.DeserializeObject<List<CAuthority>>(GuestAuthStr);

        [JsonIgnore]
        public long id;
        public string username { set; get; }

        [DefaultValue("")]
        public string password;
        public string type { set; get; }

        public string authority { set; get; }

        [JsonIgnore]
        public List<CAuthority> Auth { set; get; }

        [JsonIgnore]
        public string Name
        {
            get
            { return username + (type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"); }
        }

        public string parseAuth()
        {
            string res = "";
            foreach (CAuthority item in Auth)
            {
                res += item.Type + ",";
            }
            return res;
        }

        public List<CAuthority> parseauth()
        {
            List<CAuthority> res = new List<CAuthority>();

            string[] s = authority.Split(new char[] { ',' });

            foreach (string item in s)
            {
                if (item == "") continue;
                foreach (CAuthority auth in AddAuth)
                {
                    if (item == auth.Type)
                    {
                        res.Add(auth);
                        break;
                    }
                }
            }

            return res;
        }

        public User()
        {
        }

        public User(string user, string psd, UserType t)
        {
            username = user;
            password = psd;
            type = t.ToString();
            Auth = t == UserType.Admin ? AdminAuth : GuestAuth;
            authority = parseAuth();
        }
        public long Add()
        {
            return UserMgr.Add(this);
        }

        public void Modify()
        {
            UserMgr.Modify(id, this);
        }

        public void Delete()
        {
            UserMgr.Delete(id);
        }
    }

    public class UpdatesUser
    {
        public long id;
        public User user;
    }

    public class UserMgr
    {
        private static long OrginIndex = 0;
        private static long CurrentIndex = 0;

        private static Dictionary<long, User> s_Add = new Dictionary<long, User>();
        private static List<long> s_Del = new List<long>();

        private static List<UpdatesUser> s_Update = new List<UpdatesUser>();


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
                call = RequestType.user.ToString(),
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

        public static List<User> List()
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
                call = RequestType.user.ToString(),
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

                List<User> s_List = LogServer.Call(str, ParseList) as List<User>;

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

        public static long Add(User user)
        {
            s_Add.Add(++CurrentIndex, user);
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

        public static void Modify(long Id, User user)
        {
            try
            {
                if (Id > OrginIndex)
                {
                    s_Add[Id] = user;
                }
                else
                {
                    user.password = "";
                    s_Update.Add(new UpdatesUser() { id = Id, user = user });
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
                delparam.Add("users", s_Del);

                LogServerRequest delreq = new LogServerRequest()
                {
                    call = RequestType.user.ToString(),
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
                updateparam.Add("users", s_Update);

                LogServerRequest updatereq = new LogServerRequest()
                {
                    call = RequestType.user.ToString(),
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
                List<User> addlist = new List<User>();
                foreach (var item in s_Add)
                {
                    addlist.Add(item.Value);
                }

                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.add.ToString());
                addparam.Add("users", addlist);

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
    }

}
