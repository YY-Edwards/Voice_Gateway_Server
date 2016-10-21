using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

using Newtonsoft.Json;

using System.Net;
using System.Net.Sockets;

namespace TrboX
{
    public enum UserType
    {
        Admin,
        Guest
    }

    public enum FuncList
    {
        
    }

    public class TabUser 
    {
        public int id;
        public string username;
        public string password;
        public UserType type;
        public List<FuncList> func;

        public UserStr GetUserStr()
        {
            UserStr str = new UserStr()
            {
                id = id.ToString(),
                username = username,
                password = password,
                type = ((int)type).ToString(),
            };
            List<string>  tmp = new List<string> ();
            foreach(FuncList item in func)
            {
                tmp.Add(item.ToString());
            }

            str.func = JsonConvert.SerializeObject(tmp);
            return str;
        }
    }
    public class UserStr {
        public string id;
        public string username;
        public string password;
        public string type;
        public string func;

        public TabUser GetUser()
        {
            TabUser user = new TabUser()
            {
            id = Trbox.IsNumber(id) ? int.Parse(id) : 0 ,
            username = username,
            password = password,
            type = Trbox.IsNumber(type) ? (UserType)int.Parse(type) : UserType.Guest,
            };

            try{
            List<string>  tmp = JsonConvert.DeserializeObject<List<string>>(func);
            List<FuncList> funlist = new List<FuncList>();
            foreach(string item in tmp)
            {
                try{
                    funlist.Add((FuncList)Enum.Parse(typeof(FuncList),item));
                }
                catch{}
            }
            user.func = funlist;

            }
            catch{}

            return user;
        }
    }

    public enum StaffType
    {
        Vehicle,
        Staff
    };

    public class TabStaff
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
                valid = valid ? "1":"0"
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

        public TabStaff GetStaff()
        {
            return new TabStaff()
            {
            id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
            name = name,
            type = Trbox.IsNumber(type) ? (StaffType)int.Parse(type) : StaffType.Staff,
            phone = phone,
            user = Trbox.IsNumber(user) ? int.Parse(user) : 0,
            valid = valid == "0" ? false:true
            };

        }
    }

    public class TabDepartment
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

        public TabDepartment GetDepartment()
        {
            return new TabDepartment()
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

    public class TabRadio
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

        public TabRadio GetRadio()
        {
            return new TabRadio()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
                radio_id = Trbox.IsNumber(radio_id) ? long.Parse(radio_id) : 0,
                type =Trbox.IsNumber(type) ? (RadipType)int.Parse(type) : RadipType.Radio,
                screen = screen == "0" ? false:true,
                gps = gps == "0" ? false:true,
                keyboard = keyboard == "0" ? false:true,
                sn = sn,
                valid = valid == "0" ? false:true,
            };
        }
    }

    public class TabRadioBelong
    {
        public int id;
        public int staff;
        public int department;
        public int radio;

        public RadioBelongStr GetRadioBelongStr()
        {
            return new RadioBelongStr()
            {
                id =id.ToString(),
                staff = staff.ToString(),
                department = department.ToString(),
                radio = radio.ToString(),
            };
        }
    }

    public class RadioBelongStr
    {
        public string id;
        public string staff;
        public string department;
        public string radio;

        public TabRadioBelong GetRadioBelong()
        {
            return new TabRadioBelong()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
                staff = Trbox.IsNumber(staff) ? int.Parse(staff) : 0,
                department = Trbox.IsNumber(department) ? int.Parse(department) : 0,
                radio = Trbox.IsNumber(radio) ? int.Parse(radio) : 0,
            };
        }
    }

    public class ResTables
    {
        public List<TabUser> User;
        public List<TabStaff> Staff;
        public List<TabDepartment> Department;
        public List<TabRadio> Radio;
        public List<TabRadioBelong> RadioBelong;
    }

    public enum ResType
    {
        User = 0,
        Staff = 1,
        Department = 2,
        Radio = 3,
        RadioBelong = 4,
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

       public TabOperate(TabOpType op,object e)
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
            if (null != obj)
            {
                if (obj is TabUser) modify.obj = ((TabUser)obj).GetUserStr();
                else if (obj is TabStaff) ((TabStaff)obj).GetStaffStr();
                else if (obj is TabDepartment) ((TabDepartment)obj).GetDepartmentStr();
                else if (obj is TabRadio) ((TabRadio)obj).GetRadioStr();
                else if (obj is TabRadioBelong) ((TabRadioBelong)obj).GetRadioBelongStr();
            }

            return modify;
        }
    }

    public class ModifyObjStr
    {
        public string id;
        public object obj;

        public ModifyObj GetModifyObj()
        {
            ModifyObj modify = new ModifyObj()
            {
                id = Trbox.IsNumber(id) ? int.Parse(id) : 0,
            };
            if (null != obj)
            {
                if (obj is UserStr) modify.obj = ((UserStr)obj).GetUser();
                else if (obj is StaffStr) ((StaffStr)obj).GetStaff();
                else if (obj is DepartmentStr) ((DepartmentStr)obj).GetDepartment();
                else if (obj is RadioStr) ((RadioStr)obj).GetRadio();
                else if (obj is RadioBelongStr) ((RadioBelongStr)obj).GetRadioBelong();
            }

            return modify;
        }
    }


    public class GetTabParamStr
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

    class ResourceMgr
    {
        private TcpInterface TServer = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8001));

        private List<Dictionary<ItemIndex, TabOperate>> m_OperateList = new List<Dictionary<ItemIndex, TabOperate>>();
        private int AddTempIndex = -1;

        delegate object ParseResult(TcpResponse res);
        delegate void UpdateRes(List<object> add, List<string> del, List<ModifyObjStr> modify);

        private UpdateRes[] UpdateResFunc;

        public ResourceMgr()
        {
           UpdateResFunc  = new UpdateRes[5] {UpdateUser, UpdateStaff ,UpdateDepartment, UpdateRadio,UpdateRadioBelong};
        }

        public void Set()
        {
            for(int i = 0 ; i< 5; i++)SetRes(m_OperateList[i], UpdateUser);
        }

        public ResTables Get()
        {
            ResTables res = new ResTables();

            int count = (int)CallRpc(RequestType.getUserCount, new GetCountParamStr(), ParseStatus);
            GetTabParamStr param = new GetTabParamStr() { offset = "0", count = count.ToString() };
            //param.AddCondition()//add condition
            res.User = (List<TabUser>)CallRpc(RequestType.getUser, param, ParseStatus);

            count = (int)CallRpc(RequestType.getStaffCount, new GetCountParamStr(), ParseStatus);
            param = new GetTabParamStr() { offset = "0", count = count.ToString() };
            //param.AddCondition()//add condition
            res.Staff = (List<TabStaff>)CallRpc(RequestType.getStaff, param, ParseStatus);

            count = (int)CallRpc(RequestType.getDepartmentCount, new GetCountParamStr(), ParseStatus);
            param = new GetTabParamStr() { offset = "0", count = count.ToString() };
            //param.AddCondition()//add condition
            res.Department = (List<TabDepartment>)CallRpc(RequestType.getDepartment, param, ParseStatus);

            count = (int)CallRpc(RequestType.getRadioCount, new GetCountParamStr(), ParseStatus);
            param = new GetTabParamStr() { offset = "0", count = count.ToString() };
            //param.AddCondition()//add condition
            res.Radio = (List<TabRadio>)CallRpc(RequestType.getRadio, param, ParseStatus);

            count = (int)CallRpc(RequestType.getRadioBelongCount, new GetCountParamStr(), ParseStatus);
            param = new GetTabParamStr() { offset = "0", count = count.ToString() };
            //param.AddCondition()//add condition
            res.RadioBelong = (List<TabRadioBelong>)CallRpc(RequestType.getRadioBelong, param, ParseStatus);

            FillToOpList(res);
            return res;
        }

        private void FillToOpList(ResTables res)
        {
            for(int i = 0 ; i< 5; i++)
            {
                if (null == m_OperateList[i]) m_OperateList[i] = new Dictionary<ItemIndex, TabOperate>();
            }

            foreach (TabUser obj in res.User) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
            foreach (TabStaff obj in res.Staff) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
            foreach (TabDepartment obj in res.Department) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
            foreach (TabRadio obj in res.Radio) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj));
            foreach (TabRadioBelong obj in res.RadioBelong) m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), obj.id), new TabOperate(TabOpType.None, obj)); 
        }


        public void Add(object obj)
        {
            try
            {
                m_OperateList[GetIndex(obj)].Add(new ItemIndex(GetType(obj), AddTempIndex), new TabOperate(TabOpType.Add, SetId(obj,AddTempIndex)));
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

        private object CallRpc(RequestType type, ParseResult Response = null)
        {
            return CallRpc(type, null, Response);
        }

        private object CallRpc(RequestType type, object param, ParseResult Response = null)
        {
            TServer.WriteString(JsonParse.Req2Json(type, param));
            if (null != Response) return Response(TServer.ReadString(JsonParse.CallID) as TcpResponse);
            return null;
        }

        private void ExecUpdate(RequestType type, List<object> lst, ParseResult Response)
        {
            int i = 0;
            for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
            CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        }

        private void ExecUpdate(RequestType type, List<string> lst, ParseResult Response)
        {
            int i = 0;
            for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
            CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        }
        private void ExecUpdate(RequestType type, List<ModifyObjStr> lst, ParseResult Response)
        {
            int i = 0;
            for (i = 0; i < lst.Count / 20; i++) CallRpc(type, lst.Skip(i * 20).Take(20) as List<Object>, Response);
            CallRpc(type, lst.Skip(i * 20).Take(lst.Count % 20) as List<Object>, Response);
        }


        private void UpdateUser(List<object> add, List<string> del, List<ModifyObjStr> modify)
        {
            ExecUpdate(RequestType.addUser, add, ParseStatus);
            ExecUpdate(RequestType.deleteUser, del, ParseStatus);
            ExecUpdate(RequestType.updateUser, modify, ParseStatus);
        }

        private void UpdateStaff(List<object> add, List<string> del, List<ModifyObjStr> modify)
        {
            ExecUpdate(RequestType.addStaff, add, ParseStatus);
            ExecUpdate(RequestType.deleteStaff, del, ParseStatus);
            ExecUpdate(RequestType.updateStaff, modify, ParseStatus);
        }
        private void UpdateDepartment(List<object> add, List<string> del, List<ModifyObjStr> modify)
        {
            ExecUpdate(RequestType.addDepartment, add, ParseStatus);
            ExecUpdate(RequestType.deleteDepartment, del, ParseStatus);
            ExecUpdate(RequestType.updateDepartment, modify, ParseStatus);
        }
        private void UpdateRadio(List<object> add, List<string> del, List<ModifyObjStr> modify)
        {
            ExecUpdate(RequestType.addRadio, add, ParseStatus);
            ExecUpdate(RequestType.deleteRadio, del, ParseStatus);
            ExecUpdate(RequestType.updateRadio, modify, ParseStatus);
        }
        private void UpdateRadioBelong(List<object> add, List<string> del, List<ModifyObjStr> modify)
        {
            ExecUpdate(RequestType.addRadioBelong, add, ParseStatus);
            ExecUpdate(RequestType.deleteRadioBelong, del, ParseStatus);
            ExecUpdate(RequestType.updateRadioBelong, modify, ParseStatus);
        }


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
            if (obj is TabUser) return 0;
            else if (obj is TabStaff) return 1;
            else if (obj is TabDepartment)  return 2;
            else if (obj is TabRadio)  return 3;
            else if (obj is TabRadioBelong)  return 4;
            return 0;
        }
        private int GetId(object obj)
        {
            int id = 0;
            if (obj is TabUser) id = ((TabUser)obj).id;
            else if (obj is TabStaff) id = ((TabStaff)obj).id;
            else if (obj is TabDepartment) id = ((TabDepartment)obj).id;
            else if (obj is TabRadio) id = ((TabRadio)obj).id;
            else if (obj is TabRadioBelong) id = ((TabRadioBelong)obj).id;
            return id;
        }

        private object SetId(object obj, int id)
        {
            if (obj is TabUser) ((TabUser)obj).id = id;
            else if (obj is TabStaff) ((TabStaff)obj).id = id;
            else if (obj is TabDepartment) ((TabDepartment)obj).id = id;
            else if (obj is TabRadio) ((TabRadio)obj).id = id;
            else if (obj is TabRadioBelong) ((TabRadioBelong)obj).id = id;
            return obj;
        }

        private ResType GetType(object obj)
        {
            if (obj is TabUser) return ResType.User;
            else if (obj is TabStaff) return ResType.Staff;
            else if (obj is TabDepartment) return ResType.Department;
            else if (obj is TabRadio) return ResType.Radio;
            else if (obj is TabRadioBelong) return ResType.RadioBelong;
            return ResType.User;
        }

        private object GetStr(object obj)
        {
            if (obj is TabUser) return ((TabUser)obj).GetUserStr();
            else if (obj is TabStaff) return ((TabStaff)obj).GetStaffStr();
            else if (obj is TabDepartment) return ((TabDepartment)obj).GetDepartmentStr();
            else if (obj is TabRadio) return ((TabRadio)obj).GetRadioStr();
            else if (obj is TabRadioBelong) return ((TabRadioBelong)obj).GetRadioBelongStr();
            return null;
        }

    }
}
