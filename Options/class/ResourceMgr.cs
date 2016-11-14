using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;


namespace TrboX
{

    public enum OperateType
    {
        add,
        list,
        count,
        del,
        update,

    }
  
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


    public class User    {


        private static string AllAudthStr = "["
        +"{             'Type': 'Call',             'Dept': '呼叫',       'Icon': ''          },"
        +"{             'Type': 'Message',          'Dept': '短信',       'Icon': ''          },"
        +"{             'Type': 'Position',         'Dept': '定位',       'Icon': ''          },"
        +"{             'Type': 'Tracker',          'Dept': '巡更追踪',   'Icon': ''          },"
        +"{             'Type': 'JobTicket',        'Dept': '工单',       'Icon': ''          }"

        +"]";

        private static string AdminAuthStr = "["
        +"{             'Type': 'Call',             'Dept': '呼叫',       'Icon': ''          },"
        +"{             'Type': 'Message',          'Dept': '短信',       'Icon': ''          },"
        +"{             'Type': 'Position',         'Dept': '定位',       'Icon': ''          },"
        +"{             'Type': 'Tracker',          'Dept': '巡更追踪',   'Icon': ''          },"
        +"{             'Type': 'JobTicket',        'Dept': '工单',       'Icon': ''          }"

        +"]";

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

        [DefaultValue((long)0)]
        public long id;
        public string username{set;get;}

        [DefaultValue("")]
        public string password;
        public string type{set;get;}

        public string authority { set; get; }

         [JsonIgnore]
        public List<CAuthority> Auth{ set; get; }

         [JsonIgnore]
         public string Name
         {
             get
             { return username + (type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"); }
         }

         public string parseAuth()
        {
           string res = "";
           foreach(CAuthority item in Auth)
           {
               res += item.Type + ",";
           }
           return res;
        }

        public List<CAuthority> parseauth()
        {
            List<CAuthority> res= new List<CAuthority> ();

            string[] s = authority.Split(new char[] { ',' });

            foreach (string item in s)
            {
                if (item == "") continue;
                foreach(CAuthority auth in AddAuth)
                {
                    if(item == auth.Type)
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

        public User(string user, string psd ,UserType t){
            username =user;
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

    public class Critera
    {
        [DefaultValue(null)]
        public string[][] condition;

        [DefaultValue(null)]
        public string[] sort;

        [DefaultValue(-1)]
        public int offset;

        [DefaultValue(-1)]
        public int count;
    }

    public class TableRequest
    {
        public string operation;
        public object users;
    }

    public class updatestru
    {
        public long id;
        public User user;
    }
    public class UserMgr
    {        
        private static long OrginIndex = 0;
        private static long CurrentIndex = 0;

        private static Dictionary<long,User> s_Add =new  Dictionary<long,User>();
        private static List<long> s_Del = new List<long> ();

        private static List<updatestru> s_Update = new List<updatestru>();


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
                   DataBase.InsertLog("Build Josn Error"  + e.Message);
                   return 0;
               }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch(Exception e)
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

        public static  List<User> List()
        {

           Dictionary<string, object> param = new Dictionary<string, object>();

           param.Add("operation", OperateType.list.ToString());
           param.Add("critera", new Critera() {
               offset = 0,
               count = Count()
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
               DataBase.InsertLog("Build Josn Error"  + e.Message);
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
                List<User> res =  Dic["records"];

                for (int i = 0; i < res.Count; i++ )
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
            user.id = 0;
            s_Add.Add(++CurrentIndex, user);
            return CurrentIndex;
        }

        public static void Delete(long Id)
        {
            try{
            
                if(Id > OrginIndex)
                {
                    s_Add.Remove(Id);
                }
                else 
                {
                    if(!s_Del.Contains(Id))s_Del.Add(Id);
                }
            }
            catch{

            }
        }

        public static void Modify(long Id, User user)
        {
            try
            {
                if (Id > OrginIndex)
                {
                    user.id = 0;
                    s_Add[Id] = user;
                }
                else
                {
                    user.id = 0;
                    user.password = "";
                    s_Update.Add(new updatestru() { id = Id, user = user});
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


    //public class UserStr
    //{
    //    public string id;
    //    public string username{get; set;}
    //    public string password;
    //    public string type;
    //    public string func;

    //    public User GetUser()
    //    {
    //        User user = new User()
    //        {
    //            id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
    //            username = username,
    //            password = password,
    //            type = Trbox.IsNumber(type) ? (UserType)int.Parse(type) : UserType.Guest,
    //        };

    //        try
    //        {
    //            List<string> tmp = JsonConvert.DeserializeObject<List<string>>(func);
    //            List<FuncList> funlist = new List<FuncList>();
    //            foreach (string item in tmp)
    //            {
    //                try
    //                {
    //                    funlist.Add((FuncList)Enum.Parse(typeof(FuncList), item));
    //                }
    //                catch { }
    //            }
    //            user.func = funlist;

    //        }
    //        catch { }

    //        return user;
    //    }
    //}

    public enum StaffType
    {
        Vehicle,
        Staff
    };

    public class Staff
    {
        public int id;
        public string name;
        public StaffType type;
        public string phone;
        public int user;
        public bool valid;

        public StaffStr GetStaffStr()
        {
            return new StaffStr()
            {
                id = id.ToString(),
                name = name,
                type = ((int)type).ToString(),
                phone = phone,
                user = user.ToString(),
                valid = valid ? "1" : "0"
            };

        }


    }
    public class StaffStr
    {
        public string id;
        public string name;
        public string type;
        public string phone;
        public string user;
        public string valid;

        public Staff GetStaff()
        {
            return new Staff()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
                name = name,
                type = Trbox.IsNumber(type) ? (StaffType)int.Parse(type) : StaffType.Staff,
                phone = phone,
                user = Trbox.IsNumber(user) ? int.Parse(user) : 0,
                valid = valid == "0" ? false : true
            };

        }
    }

    public class Department
    {
        public int id;
        public string name;
        public long group_id;

        public DepartmentStr GetDepartmentStr()
        {
            return new DepartmentStr()
            {
                id = id.ToString(),
                name = name,
                group_id = group_id.ToString(),
            };

        }


    }
    public class DepartmentStr
    {
        public string id;
        public string name;
        public string group_id;

        public Department GetDepartment()
        {
            return new Department()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
                name = name,
                group_id = Trbox.IsNumber(group_id) ? long.Parse(group_id) : 0
            };
        }
    }

    public enum RadipType
    {
        Radio,
        Ride
    };

    public class Radio
    {
        public int id;
        public long radio_id;
        public RadipType type;
        public bool screen;
        public bool gps;
        public bool keyboard;
        public string sn;
        public bool valid;


        public RadioStr GetRadioStr()
        {
            return new RadioStr()
            {
                id = id.ToString(),
                radio_id = radio_id.ToString(),
                type = ((int)type).ToString(),
                screen = screen ? "1" : "0",
                gps = gps ? "1" : "0",
                keyboard = keyboard ? "1" : "0",
                sn = sn,
                valid = valid ? "1" : "0",
            };
        }
    }
    public class RadioStr
    {
        public string id;
        public string radio_id;
        public string type;
        public string screen;
        public string gps;
        public string keyboard;
        public string sn;
        public string valid;

        public Radio GetRadio()
        {
            return new Radio()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
                radio_id = Trbox.IsNumber(radio_id) ? long.Parse(radio_id) : 0,
                type = Trbox.IsNumber(type) ? (RadipType)int.Parse(type) : RadipType.Radio,
                screen = screen == "0" ? false : true,
                gps = gps == "0" ? false : true,
                keyboard = keyboard == "0" ? false : true,
                sn = sn,
                valid = valid == "0" ? false : true,
            };
        }
    }

    public class Belong
    {
        public int ID;
        public int Staff;
        public int Department;
        public int Radio;

        //public BelongStr GetBelongStr()
        //{
        //    return new BelongStr()
        //    {
        //        id = id.ToString(),
        //        staff = staff.ToString(),
        //        department = department.ToString(),
        //        radio = radio.ToString(),
        //    };
        //}
    }

    public class BelongStr
    {
        public string id;
        public string staff;
        public string department;
        public string radio;

        //public Belong GetBelong()
        //{
        //    return new Belong()
        //    {
        //        id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
        //        staff = Trbox.IsNumber(staff) ? int.Parse(staff) : 0,
        //        department = Trbox.IsNumber(department) ? int.Parse(department) : 0,
        //        radio = Trbox.IsNumber(radio) ? int.Parse(radio) : 0,
        //    };
        //}
    }

    public class ResList
    {
        public List<User> User;
        public List<Staff> Staff;
        public List<Department> Department;
        public List<Radio> Radio;
        public List<Belong> Belong;
    }

    public enum ResType
    {
        User = 0,
        Staff = 1,
        Department = 2,
        Radio = 3,
        Belong = 4,
    };

    public enum TabOpType
    {
        None = 0,
        Add = 1,
        Delete = 2,
        Modify = 3
    }
    public class ItemIndex
    {
        public ResType type;
        public int index;

        public ItemIndex(ResType t, int id)
        {
            type = t; index = id;
        }
    }
    public class TabOperate
    {
        public object obj;
        public TabOpType operate;

        public TabOperate(TabOpType op, object e)
        {
            operate = op; obj = e;
        }
    }

    public class ModifyObj
    {
        public int id;
        public object obj;

        public ModifyObjStr GetModifyObjStr()
        {
            ModifyObjStr modify = new ModifyObjStr()
            {
                id = id.ToString(),
            };
            //if (null != obj)
            //{
            //    if (obj is User) modify.obj = ((User)obj).GetUserStr();
            //    else if (obj is Staff) ((Staff)obj).GetStaffStr();
            //    else if (obj is Department) ((Department)obj).GetDepartmentStr();
            //    else if (obj is Radio) ((Radio)obj).GetRadioStr();
            //    else if (obj is Belong) ((Belong)obj).GetBelongStr();
            //}

            return modify;
        }
    }

    public class ModifyObjStr
    {
        public string id;
        public object obj;

        //public ModifyObj GetModifyObj()
        //{
        //    ModifyObj modify = new ModifyObj()
        //    {
        //        id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
        //    };
        //    if (null != obj)
        //    {
        //        if (obj is UserStr) modify.obj = ((UserStr)obj).GetUser();
        //        else if (obj is StaffStr) ((StaffStr)obj).GetStaff();
        //        else if (obj is DepartmentStr) ((DepartmentStr)obj).GetDepartment();
        //        else if (obj is RadioStr) ((RadioStr)obj).GetRadio();
        //        else if (obj is BelongStr) ((BelongStr)obj).GetBelong();
        //    }

        //    return modify;
        //}
    }


    public class GetParamStr
    {
        public List<List<string>> condition;
        public string offset;
        public string count;

        public void AddCondition(string relation, string comp, string dest, string src)
        {
            if (null == condition) condition = new List<List<string>>();

            List<string> con = new List<string>();
            con.Add(relation);
            con.Add(comp);
            con.Add(dest);
            con.Add(src);
            condition.Add(con);
        }
    }

    public class GetCountParamStr
    {
        public List<List<string>> condition;

        public void AddCondition(string relation, string comp, string dest, string src)
        {
            if (null == condition) condition = new List<List<string>>();

            List<string> con = new List<string>();
            con.Add(relation);
            con.Add(comp);
            con.Add(dest);
            con.Add(src);
            condition.Add(con);
        }
    }

    public class ResourceMgr
    {
        string user = "["
        + "{'id':'1', 'username':'张三', 'password':'123'},"
        + "{'id':'2', 'username':'李四', 'password':'123'},"
        + "{'id':'3', 'username':'Jim', 'password':'123'},"
        + "{'id':'4', 'username':'John', 'password':'123'},"
        + "{'id':'5', 'username':'二麻子', 'password':'123'},"
        + "{'id':'6', 'username':'崔二胯子', 'password':'123'},"
        + "{'id':'7', 'username':'曾弓北', 'password':'123'},"
        + "{'id':'8', 'username':'萧剑南', 'password':'123'},"
        + "{'id':'9', 'username':'张国忠', 'password':'123'},"
        + "{'id':'10', 'username':'张国义', 'password':'123'},"
        + "{'id':'11', 'username':'张毅成', 'password':'123'},"
        + "{'id':'12', 'username':'柳萌萌', 'password':'123'},"
        + "{'id':'13', 'username':'秦戈', 'password':'123'}"
        + "]";


        string staff = "["
        + "{'id':'1', 'name':'张三', 'type':'0'},"
        + "{'id':'2', 'name':'川A12345', 'type':'1'},"
        + "{'id':'3', 'name':'Jim', 'type':'0'},"
        + "{'id':'4', 'name':'川B12345', 'type':'1'},"
        + "{'id':'5', 'name':'二麻子', 'type':'0'},"
        + "{'id':'6', 'name':'崔二胯子', 'type':'0'},"
        + "{'id':'7', 'name':'川A12345', 'type':'1'},"
        + "{'id':'8', 'name':'萧剑南', 'type':'0'},"
        + "{'id':'9', 'name':'张国忠', 'type':'0'},"
        + "{'id':'10', 'name':'贵C12345', 'type':'1'},"
        + "{'id':'11', 'name':'张毅成', 'type':'0'},"
        + "{'id':'12', 'name':'柳萌萌', 'type':'0'},"
        + "{'id':'13', 'name':'秦戈', 'type':'0'}"
        + "]";


        string department = "["
        + "{'id':'1' , 'group_id':'203', 'name':'调度组'},"
        + "{'id':'2', 'group_id':'314', 'name':'保洁组'},"
        + "{'id':'3', 'group_id':'425', 'name':'地勤组'},"
        + "{'id':'4', 'group_id':'536', 'name':'餐厅'},"
        + "{'id':'5', 'group_id':'647', 'name':'安保组'},"
        + "{'id':'6', 'group_id':'758', 'name':'候机厅'}"
        + "]";

        string radio = "["
        + "{'id':'1', 'radioid':'65536', 'type':'0'},"
        + "{'id':'2', 'radioid':'112'},"
        + "{'id':'3', 'radioid':'113'},"
        + "{'id':'4', 'radioid':'114'},"
        + "{'id':'5', 'radioid':'115'},"
        + "{'id':'6', 'radioid':'116'},"
        + "{'id':'7', 'radioid':'117'},"
        + "{'id':'8', 'radioid':'118'},"
        + "{'id':'9', 'radioid':'119'},"
        + "{'id':'10', 'radioid':'120'},"
        + "{'id':'11', 'radioid':'121'},"
        + "{'id':'12', 'radioid':'122'},"
        + "{'id':'13', 'radioid':'123'},"
        + "{'id':'14', 'radioid':'124'},"
        + "{'id':'15', 'radioid':'125'},"
        + "{'id':'16', 'radioid':'126'},"
        + "{'id':'17', 'radioid':'127'},"
        + "{'id':'18', 'radioid':'128'},"
        + "{'id':'19', 'radioid':'129'},"
        + "{'id':'20', 'radioid':'130'},"
        + "{'id':'21', 'radioid':'131'},"
        + "{'id':'22', 'radioid':'132'},"
        + "{'id':'23', 'radioid':'133'},"
        + "{'id':'24', 'radioid':'134'}"
        + "]";

        string radio_belong = "["
        + "{'department': '1', 'staff': '1', 'radio': '3' },"
        + "{'department': '1', 'staff': '3', 'radio': '4' },"
        + "{'department': '1', 'staff': '5', 'radio': '5' },"
        + "{'department': '1', 'staff': '-1', 'radio': '6' },"
        + "{'department': '1', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '1', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '1', 'staff': '-1', 'radio': '23' },"
        + "{'department': '1', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '2', 'staff': '4', 'radio': '7' },"
        + "{'department': '2', 'staff': '2', 'radio': '8' },"
        + "{'department': '2', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '3', 'staff': '6', 'radio': '9' },"
        + "{'department': '3', 'staff': '7', 'radio': '-1' },"
        + "{'department': '3', 'staff': '-1', 'radio': '10' },"
        + "{'department': '3', 'staff': '-1 ', 'radio': '11' },"
        + "{'department': '3', 'staff': '-1', 'radio': '12' },"
        + "{'department': '4', 'staff': '8', 'radio': '-1' },"
        + "{'department': '4', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '4', 'staff': '-1',  'radio': '14' },"
        + "{'department': '4', 'staff': '-1',  'radio': '-1' },"
        + "{'department': '5', 'staff': '10', 'radio': '-1' },"
        + "{'department': '5', 'staff': '11', 'radio': '15' },"
        + "{'department': '5', 'staff': '12', 'radio': '16' },"
        + "{'department': '5', 'staff': '-1', 'radio': '17' },"
        + "{'department': '5', 'staff': '-1', 'radio': '18' },"
        + "{'department': '5', 'staff': '-1', 'radio': '19' },"
        + "{'department': '5', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '6', 'staff': '9',  'radio': '24' },"
        + "{'department': '6', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '6', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '6', 'staff': '-1', 'radio': '20' },"
        + "{'department': '6', 'staff': '-1',   'radio': '-1' },"
        + "{'department': '6', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '6', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '6', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '-1', 'staff': '13', 'radio': '21' },"
        + "{'department': '-1', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '-1', 'staff': '-1', 'radio': '22' },"
        + "{'department': '-1', 'staff': '-1', 'radio': '-1' },"
        + "{'department': '-1', 'staff': '-1',  'radio': '-1' },"
        + "]";



       // private TcpInterface TServer = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8001));

        private List<Dictionary<ItemIndex, TabOperate>> m_OperateList = new List<Dictionary<ItemIndex, TabOperate>>();
        private int AddTempIndex = -1;

        delegate object ParseResult(TcpResponse res);
        delegate void UpdateRes(List<object> add, List<string> del, List<ModifyObjStr> modify);

        private UpdateRes[] UpdateResFunc;

        //public ResourceMgr()
        //{
        //    UpdateResFunc = new UpdateRes[5] { UpdateUser, UpdateStaff, UpdateDepartment, UpdateRadio, UpdateRadioBelong };
        //}

        //public void Set()
        //{
        //    for (int i = 0; i < 5; i++) SetRes(m_OperateList[i], UpdateUser);
        //}

        public ResList Get()
        {
            ResList res = new ResList();

           // int count = (int)CallRpc(RequestType.getUserCount, new GetCountParamStr(), ParseStatus);
           // GetParamStr param = new GetParamStr() { offset = "0", count = count.ToString() };
           // //param.AddCondition()//add condition
           // res.User = (List<User>)CallRpc(RequestType.getUser, param, ParseStatus);

           // count = (int)CallRpc(RequestType.getStaffCount, new GetCountParamStr(), ParseStatus);
           // param = new GetParamStr() { offset = "0", count = count.ToString() };
           // //param.AddCondition()//add condition
           // res.Staff = (List<Staff>)CallRpc(RequestType.getStaff, param, ParseStatus);

           // count = (int)CallRpc(RequestType.getDepartmentCount, new GetCountParamStr(), ParseStatus);
           // param = new GetParamStr() { offset = "0", count = count.ToString() };
           //// param.AddCondition()//add condition
           // res.Department = (List<Department>)CallRpc(RequestType.getDepartment, param, ParseStatus);

           // count = (int)CallRpc(RequestType.getRadioCount, new GetCountParamStr(), ParseStatus);
           // param = new GetParamStr() { offset = "0", count = count.ToString() };
           //// param.AddCondition()//add condition
           // res.Radio = (List<Radio>)CallRpc(RequestType.getRadio, param, ParseStatus);

           // count = (int)CallRpc(RequestType.getRadioBelongCount, new GetCountParamStr(), ParseStatus);
           // param = new GetParamStr() { offset = "0", count = count.ToString() };
           // //param.AddCondition()//add condition
           // res.RadioBelong = (List<Belong>)CallRpc(RequestType.getRadioBelong, param, ParseStatus);

            FillToOpList(res);
            return res;
        }

        private void FillToOpList(ResList res)
        {
        //    for (int i = 0; i < 5; i++)
        //    {
        //        if (null == m_OperateList[i]) m_OperateList[i] = new Dictionary<ItemIndex, TabOperate>();
        //    }

        //    foreach (User obj in res.User) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
        //    foreach (Staff obj in res.Staff) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
        //    foreach (Department obj in res.Department) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
        //    foreach (Radio obj in res.Radio) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
        //    foreach (Belong obj in res.Belong) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
        //
        }


        public void Add(object obj)
        {
            try
            {
                m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), AddTempIndex), new TabOperate(TabOpType.Add, SetId(obj, AddTempIndex)));
            }
            catch { }

            AddTempIndex--;
        }

        public void Del(object obj)
        {
            try
            {
                if (GetId(obj) > 0) m_OperateList[GetIndex(obj)][new ItemIndex(GetType(obj), GetId(obj))].operate = TabOpType.Delete;
                else if (GetId(obj) < 0) m_OperateList[GetIndex(obj)].Remove(new ItemIndex(GetType(obj), GetId(obj)));
            }
            catch { }
        }

        public void Modify(object obj)
        {
            try
            {
                m_OperateList[GetIndex(obj)][new ItemIndex(GetType(obj), GetId(obj))].operate = GetId(obj) > 0 ? TabOpType.Modify : TabOpType.Add;
                m_OperateList[GetIndex(obj)][new ItemIndex(GetType(obj), GetId(obj))].obj = obj;
            }
            catch { }
        }


        private object ParseStatus(TcpResponse res)
        {
            if (null == res) return false;
            return res.IsSuccess;
        }

        //private object CallRpc(RequestType type, ParseResult Response = null)
        //{
        //    return CallRpc(type, null, Response);
        //}

        //private object CallRpc(RequestType type, object param, ParseResult Response = null)
        //{
        //    TServer.WriteString(JsonParse.Req2Json(type, param));
        //    if (null != Response) return Response(TServer.ReadString(JsonParse.CallID) as TcpResponse);
        //    return null;
        //}

        //private void ExecUpdate(RequestType type, List<object> lst, ParseResult Response)
        //{
        //    int i = 0;
        //    for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
        //    CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        //}

        //private void ExecUpdate(RequestType type, List<string> lst, ParseResult Response)
        //{
        //    int i = 0;
        //    for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
        //    CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        //}
        //private void ExecUpdate(RequestType type, List<ModifyObjStr> lst, ParseResult Response)
        //{
        //    int i = 0;
        //    for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
        //    CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        //}


        //private void UpdateUser(List<object> add, List<string> del, List<ModifyObjStr> modify)
        //{
        //    ExecUpdate(RequestType.addUser, add, ParseStatus);
        //    ExecUpdate(RequestType.deleteUser, del, ParseStatus);
        //    ExecUpdate(RequestType.updateUser, modify, ParseStatus);
        //}

        //private void UpdateStaff(List<object> add, List<string> del, List<ModifyObjStr> modify)
        //{
        //    ExecUpdate(RequestType.addStaff, add, ParseStatus);
        //    ExecUpdate(RequestType.deleteStaff, del, ParseStatus);
        //    ExecUpdate(RequestType.updateStaff, modify, ParseStatus);
        //}
        //private void UpdateDepartment(List<object> add, List<string> del, List<ModifyObjStr> modify)
        //{
        //    ExecUpdate(RequestType.addDepartment, add, ParseStatus);
        //    ExecUpdate(RequestType.deleteDepartment, del, ParseStatus);
        //    ExecUpdate(RequestType.updateDepartment, modify, ParseStatus);
        //}
        //private void UpdateRadio(List<object> add, List<string> del, List<ModifyObjStr> modify)
        //{
        //    ExecUpdate(RequestType.addRadio, add, ParseStatus);
        //    ExecUpdate(RequestType.deleteRadio, del, ParseStatus);
        //    ExecUpdate(RequestType.updateRadio, modify, ParseStatus);
        //}
        //private void UpdateRadioBelong(List<object> add, List<string> del, List<ModifyObjStr> modify)
        //{
        //    ExecUpdate(RequestType.addRadioBelong, add, ParseStatus);
        //    ExecUpdate(RequestType.deleteRadioBelong, del, ParseStatus);
        //    ExecUpdate(RequestType.updateRadioBelong, modify, ParseStatus);
        //}


        private void SetRes(Dictionary<ItemIndex, TabOperate> lst, UpdateRes Update)
        {
            List<object> AddList = new List<object>();
            List<string> DeleteList = new List<string>();
            List<ModifyObjStr> ModifyList = new List<ModifyObjStr>();

            lst = lst.OrderBy(o => o.Value.operate).ToDictionary(p => p.Key, o => o.Value);
            foreach (var item in lst)
            {
                switch (item.Value.operate)
                {
                    case TabOpType.Add:
                        AddList.Add(GetStr(item.Value.obj));
                        break;
                    case TabOpType.Delete:
                        DeleteList.Add(GetId(item.Value.obj).ToString());
                        break;
                    case TabOpType.Modify:
                        ModifyList.Add(new ModifyObjStr() { id = GetId(item.Value.obj).ToString(), obj = GetStr(item.Value.obj) });
                        break;
                }
            }
            if (null != Update) Update(AddList, DeleteList, ModifyList);
        }


        private int GetIndex(object obj)
        {
            if (obj is User) return 0;
            else if (obj is Staff) return 1;
            else if (obj is Department) return 2;
            else if (obj is Radio) return 3;
            else if (obj is Belong) return 4;
            return 0;
        }
        private int GetId(object obj)
        {
            int id = 0;
            //if (obj is User) id = ((User)obj).id;
            //else if (obj is Staff) id = ((Staff)obj).id;
            //else if (obj is Department) id = ((Department)obj).id;
            //else if (obj is Radio) id = ((Radio)obj).id;
            //else if (obj is Belong) id = ((Belong)obj).id;
            return id;
        }

        private object SetId(object obj, int id)
        {
            //if (obj is User) ((User)obj).id = id;
            //else if (obj is Staff) ((Staff)obj).id = id;
            //else if (obj is Department) ((Department)obj).id = id;
            //else if (obj is Radio) ((Radio)obj).id = id;
            //else if (obj is Belong) ((Belong)obj).id = id;
            return obj;
        }

        private ResType GetType(object obj)
        {
            if (obj is User) return ResType.User;
            else if (obj is Staff) return ResType.Staff;
            else if (obj is Department) return ResType.Department;
            else if (obj is Radio) return ResType.Radio;
            else if (obj is Belong) return ResType.Belong;
            return ResType.User;
        }

        private object GetStr(object obj)
        {
            //if (obj is User) return ((User)obj).GetUserStr();
            //else if (obj is Staff) return ((Staff)obj).GetStaffStr();
            //else if (obj is Department) return ((Department)obj).GetDepartmentStr();
            //else if (obj is Radio) return ((Radio)obj).GetRadioStr();
            //else if (obj is Belong) return ((Belong)obj).GetBelongStr();
            return null;
        }

    }
}
