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

    public class TServerRequestStr
    {
        public string call;
        public string type;
        public string callId;
        public object param;
    }

    public class TServerRequestSimpleStr
    {
        public string call;
        public string type;
        public string callId;
    }

    public class TServerRequest
    {
        public string call;
        public string type;
        public long callId;
        public object param;
    }

    public class TServerResponse
    {
        public string status;
        public string statusText;
        public int errCode;
        public Int64 callId;
        public object contents;
    }

    public delegate object ParseDel(object obj);
    public delegate void RxRequestDel(string param);
    public class TServer
    {
        private static Dictionary<long, TServerResponse> RxResponse = new Dictionary<long, TServerResponse>();
        public static Queue<TServerRequest> RxRequest = new Queue<TServerRequest>();

        private static TcpInterface TCP = null;
        private static long PackageNumber = 0;

        public static TargetSystemType SystemType = TargetSystemType.radio;
        public static Dictionary<RequestType, RxRequestDel> RxRequestList = new Dictionary<RequestType, RxRequestDel>();

        public TServer()
        {
        }

        public static void InitializeTServer()
        {
            TCP = new TcpInterface(new IPEndPoint(IPAddress.Parse("192.168.2.110"), 9000), OnReceive);

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)OnRx(); });
            Thread th = new Thread(threadStart);
            th.Start();

        }

        public static void RegRxHanddler(RequestType type, RxRequestDel handler)
        {
            if (RxRequestList.ContainsKey(type))
            {
                RxRequestList[type] = handler;
            }
            else
            {
                RxRequestList.Add(type, handler);
            }
        }

        public static object Call(object obj)
        {
            object res = null;
            if (obj is Setting)
            {
                SettingMgr setmgr = new SettingMgr(obj as Setting, PackageNumber);
                Write(setmgr.Json);
                if (null != setmgr.Parse) res = setmgr.Parse(ReadResponse(PackageNumber));
            }
            else if (obj is COperate)
            {
                if (TargetSystemType.radio == SystemType)
                {
                    RadioOperate radioop = new RadioOperate(obj as COperate, PackageNumber);
                    List<string> json = radioop.Json;
                    foreach (string js in json)
                    {
                        Write(js);
                        if (null != radioop.Parse(js)) res = radioop.Parse(js)(ReadResponse(PackageNumber));
                    }
                }
                else
                {
                    WirelanOperate wirelanopop = new WirelanOperate(obj as COperate, PackageNumber);
                    List<string> json = wirelanopop.Json;
                    foreach (string js in json)
                    {
                        Write(js);
                        if (null != wirelanopop.Parse(js)) res = wirelanopop.Parse(js)(ReadResponse(PackageNumber));
                    }
                }
            }
            return res;
        }

        public static object ReadResponse(long CallId = -1)
        {
            object res = null;
            List<long> del = new List<long>();
            for (int i = 0; i < 50; i++)
            {
                lock (RxResponse)
                {
                    if (RxResponse.Count > 0)
                    {
                        try
                        {
                            if (CallId < 0)
                            {
                                foreach (var value in RxResponse)
                                {
                                    res = value.Value;
                                    del.Add(value.Key);
                                    break;
                                }
                            }
                            else
                            {
                                foreach (var value in RxResponse)
                                {
                                    if (CallId == value.Key) res = RxResponse[CallId];
                                    if (CallId >= value.Key) del.Add(value.Key);
                                }
                            }
                            break;
                        }
                        catch { }
                    }
                }
                Thread.Sleep(100);
            }
            foreach (long key in del) RxResponse.Remove(key);
            return res;
        }

        public static void Write(Setting setting)
        {

        }
        public static void Write(string str)
        {
            if (null != TCP)
            {
                TCP.WriteString(str);
                PackageNumber++;
            }
        }

        private static void OnReceive(string str)
        {
            try
            {
                JObject json = JsonConvert.DeserializeObject<JObject>(str);

                if (json.Property("call") == null || json.Property("call").ToString() == "")//not type
                {
                    Console.WriteLine("Settingreponse");
                    TServerResponse rxresponse = JsonConvert.DeserializeObject<TServerResponse>(str);

                    lock (RxResponse)
                    {
                        RxResponse.Add(rxresponse.callId, rxresponse);
                    }
                }
                else
                { //op request
                    Console.WriteLine("TServerRequestStr");

                    TServerRequest rxrequest = JsonConvert.DeserializeObject<TServerRequest>(JsonConvert.SerializeObject(json));

                    if (rxrequest != null)
                    {
                        PackageNumber = rxrequest.callId;

                        Write(JsonConvert.SerializeObject(new TServerResponse()
                        {
                            status = "success",
                            callId = rxrequest.callId,
                        }));

                        //if (rxrequest.call.ToUpper() == RequestType.message.ToString().ToUpper())
                        //{
                        //    rxrequest.param = JsonConvert.DeserializeObject<RadioSmsParam>(JsonConvert.SerializeObject(rxrequest.param));
                        //}
                        //else if (rxrequest.call.ToUpper() == RequestType.sendArs.ToString().ToUpper())
                        //{
                        //    rxrequest.param = JsonConvert.DeserializeObject<RadioArsParam>(JsonConvert.SerializeObject(rxrequest.param));
                        //}
                        //else if (rxrequest.call.ToUpper() == RequestType.sendGps.ToString().ToUpper())
                        //{
                        //    string strdd = JsonConvert.SerializeObject(rxrequest.param);
                        //    rxrequest.param = JsonConvert.DeserializeObject<GPSParam>(strdd);                       
                        //}

                        lock (RxRequest)
                        {
                            RxRequest.Enqueue(rxrequest);
                        }
                    }
                }
            }
            catch
            {
                Console.WriteLine("不是Json");

                Write(JsonConvert.SerializeObject(new TServerResponse()
                {
                    status = "faliure",
                    callId = PackageNumber++,
                }));
            }
        }

        private static void OnRx()
        {
            lock (TServer.RxRequest)
            {
                if (TServer.RxRequest.Count > 0)
                {
                    TServerRequest req = TServer.RxRequest.Dequeue();

                    RequestType calltemp = RequestType.None;
                    try
                    {
                        calltemp = (RequestType)Enum.Parse(typeof(RequestType), req.call);

                    }
                    catch
                    {
                        Console.Write("call 解析错误！");
                    }

                    if (RxRequestList.ContainsKey(calltemp))
                    {
                        if (RxRequestList[calltemp] != null) RxRequestList[calltemp](JsonConvert.SerializeObject(req.param));
                    }
                    else
                    {
                        Console.Write("Rx " + req.call);
                    }
                }
                else
                {
                    Thread.Sleep(200);
                }
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
        sendArs,
        queryGps,
        sendGps,
        control,

        wlCall,
    };

}
