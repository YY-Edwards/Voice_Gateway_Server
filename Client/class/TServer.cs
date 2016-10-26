using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;



namespace TrboX
{
    public delegate object ParseDel(object obj);
    public class TServer
    {        
        private static Dictionary<long, object> RxList = new Dictionary<long, object>();
        private static TcpInterface TCP = null;
        private static long PackageNumber = 0;

        public TServer()
        {
        }

        public static void InitializeTServer()
        {
            TCP = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8000), OnReceive);
        }

        public static object Call(object obj)
        {
            object res = null;
            if(obj is Setting)
            {
                SettingMgr setmgr = new SettingMgr(obj as Setting, PackageNumber);
                Write(setmgr.Json);
                if(null != setmgr.Parse)res = setmgr.Parse(ReadResponse(PackageNumber));
            }
            else if(obj is COperate)
            {
                RadioOperate radioop = new RadioOperate(obj as COperate, PackageNumber);
                Write(radioop.Json);
                if (null != radioop.Parse) res = radioop.Parse(ReadResponse(PackageNumber));
            }

            PackageNumber++;
            return res;
        }

        public static object ReadResponse(long CallId = -1)
        {
            object res = null;
            List<long> del = new List<long> ();
            for (int i = 0; i < 50; i++)
            {
                lock (RxList)
                {
                    if (RxList.Count > 0)
                    {
                        try
                        {
                            if (CallId < 0)
                            {
                                foreach (var value in RxList)
                                {
                                    res = value.Value;
                                    del.Add(value.Key);
                                    break;
                                }
                            }
                            else
                            {
                                foreach (var value in RxList)
                                {                                  
                                    if(CallId == value.Key)res = RxList[CallId];
                                    if(CallId >= value.Key)del.Add(value.Key);
                                }
                            }
                            break;
                        }
                        catch { }
                    }
                }
                Thread.Sleep(100);
            }
            foreach(long key in del)RxList.Remove(key);
            return res;
        }
        public static void Write(Setting setting)
        {

        }
        public static void Write(string str)
        {
            if (null != TCP)TCP.WriteString(str);          
        }
    
        private static void OnReceive(string str)
        {
            try
            {
                JObject json = JsonConvert.DeserializeObject<JObject>(str);

                if (json.Property("type") == null || json.Property("type").ToString() == "")//not type
                {
                    Console.WriteLine("Settingreponse");
                    SettingResponse settingrep = JsonConvert.DeserializeObject<SettingResponse>(str);

                    lock (RxList)
                    {
                        RxList.Add(settingrep.callId, settingrep);
                    }
                }
            }
            catch {
                Console.WriteLine("不是Json");
            }
        }
    }



    public enum RequestType
    {
        None,
        //base
        setBaseSetting,
        getBaseSetting,
        setRadioSetting,
        getRadioSetting,
        setRepeaterSetting,
        getRepeaterSetting,

        getUser,
        getUserCount,
        addUser,
        deleteUser,
        updateUser,

        getStaff,
        getStaffCount,
        addStaff,
        deleteStaff,
        updateStaff,

        getDepartment,
        getDepartmentCount,
        addDepartment,
        deleteDepartment,
        updateDepartment,

        getRadio,
        getRadioCount,
        addRadio,
        deleteRadio,
        updateRadio,

        getRadioBelong,
        getRadioBelongCount,
        addRadioBelong,
        deleteRadioBelong,
        updateRadioBelong,

        call,
        message,

        groupCall,
        allCall,

        sendSms,
        sendGroupSms,

    };

}
