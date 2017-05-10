﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.Text.RegularExpressions;

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

    public class cmd
    {
        public string json;
        public ParseDel del;
    }
    public class TServer
    {
        private static Dictionary<long, TServerResponse> RxResponse = new Dictionary<long, TServerResponse>();
        public static Queue<TServerRequest> RxRequest = new Queue<TServerRequest>();

        private static TcpInterface TCP = null;
        private static long PackageNumber = 0;

        public static RunMode SystemType = RunMode.Radio;
        public static Dictionary<RequestType, RxRequestDel> RxRequestList = new Dictionary<RequestType, RxRequestDel>();

        public static bool IsInCalled = false;
        
        public static Queue<cmd> CmdList = new Queue<cmd>();
        private static Mutex CallMutex = new Mutex();

        public TServer()
        {
        }

        public static long CallId
        {
            get
            {
                return PackageNumber;
            }
        }
        public static void InitializeServer()
        {
            TCP = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9000), OnReceive);
            TCP.OnConnect = OnConnect;
            TCP.Open();
           

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)OnRx(); });
            Thread th = new Thread(threadStart);
            th.Start();
        }

        private static void OnConnect()
        {
             ThreadStart thread = new ThreadStart(delegate(){ Thread.Sleep(1000);
                MyWindow.PushMessage(new CustomMessage(DestType.OnConnectTServer, ""));
                });
                Thread thx = new Thread(thread);
                thx.Start();
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
            CallMutex.WaitOne();

            object reslut = null;
            if (obj is Setting)
            {
                SettingMgr setmgr = new SettingMgr(obj as Setting, PackageNumber);
                reslut = Call(setmgr.Json, setmgr.Parse);
            }
            else if (obj is COperate)
            {
                Dictionary<string, object> res = new Dictionary<string, object>();

                if (RunMode.Radio == SystemType)
                {
                    RadioOperate radioop = new RadioOperate(obj as COperate, PackageNumber);
                    List<string> json = radioop.Json;
                    foreach (string js in json) res.Add(js, Call(js, radioop.Parse(js)));
                }
                else if (RunMode.Repeater == SystemType)
                {
                    WirelanOperate wirelanopop = new WirelanOperate(obj as COperate, PackageNumber);
                    List<string> json = wirelanopop.Json;
                    foreach (string js in json) res.Add(js, Call(js, wirelanopop.Parse(js)));
                }

                reslut = res;
            }

            CallMutex.ReleaseMutex();
            return reslut;
        }

        public static object Call(string str, ParseDel parse = null)
        {
            object res = null;
            Write(str);
            object obj = ReadResponse(PackageNumber);
            if (null != parse) res = parse(obj);
            PackageNumber++;
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
                                    //if (CallId >= value.Key) del.Add(value.Key);
                                    del.Add(CallId);

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

            if(res == null)
            {
                DataBase.InsertLog("读取Reponse超时，CallID：" + CallId.ToString());
            }
            return res;
        }

        public static void Write(Setting setting)
        {

        }
        public static void Write(string str)
        {
            if (null != TCP)
            {
                lock (TCP)
                {
                    TCP.WriteString(str);
                }
            }
        }

        private static void OnReceive(string str)
        {

             Regex regex=new Regex("}{");//以$cjlovefl$分割
             string[] sArray = regex.Split(str);

             for(int i =0; i < sArray.Length; i++)
             {

                 if (sArray.Length  > 1 )
                 {
                 if(i == 0)
                 {
                     sArray[i] = sArray[i] + "}";
                 }
                 else if (i == sArray.Length - 1)
                 {
                     sArray[i] = "{" + sArray[i];
                 }
                 else
                 {
                     sArray[i] = "{" + sArray[i] + "}";
                 }
                 }

                 try
                 {
                     //Console.WriteLine("接收Json：{0}", sArray[i]);

                     JObject json = JsonConvert.DeserializeObject<JObject>(sArray[i]);

                     if (json.Property("call") == null || json.Property("call").ToString() == "")//not type
                     {
                         //Console.WriteLine("response");
                         TServerResponse rxresponse = JsonConvert.DeserializeObject<TServerResponse>(sArray[i]);

                         lock (RxResponse)
                         {
                             RxResponse.Add(rxresponse.callId, rxresponse);
                         }
                     }
                     else
                     { 
                         TServerRequest rxrequest = JsonConvert.DeserializeObject<TServerRequest>(JsonConvert.SerializeObject(json));

                         if (rxrequest != null)
                         {
                             PackageNumber = rxrequest.callId;

                             Write(JsonConvert.SerializeObject(new TServerResponse()
                             {
                                 status = "success",
                                 callId = rxrequest.callId,
                             }));

                             lock (RxRequest)
                             {
                                 RxRequest.Enqueue(rxrequest);
                             }
                         }
                     }
                 }
                 catch
                 {
                     DataBase.InsertLog("Json解析错误：" + sArray[i]);
                     
                     //Console.WriteLine("不是Json:"+sArray[i]);
                 }

             }
        }

        private static void OnRx()
        {
            lock (RxRequest)
            {
                if (RxRequest.Count > 0)
                {
                    TServerRequest req = RxRequest.Dequeue();

                    RequestType calltemp = RequestType.None;
                    try
                    {
                        calltemp = (RequestType)Enum.Parse(typeof(RequestType), req.call);

                    }
                    catch
                    {
                        //Console.Write("call 解析错误！");
                        DataBase.InsertLog("Request解析错误" + req.call);
                    }

                    if (RxRequestList.ContainsKey(calltemp))
                    {
                        string param = JsonConvert.SerializeObject(req.param);
                        DataBase.InsertLog(param);
                        if (RxRequestList[calltemp] != null) RxRequestList[calltemp](param);
                    }
                    else
                    {
                        DataBase.InsertLog("No Prase");
                    }
                }
                else
                {
                    Thread.Sleep(20);
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

        user,
        department,
        radio,
        staff,
        smslog,
        gpslog,


        status,
        sendArs,

        call,
        callStatus,

        message,
        messageStatus,

        queryGps,
        sendGpsStatus,
        sendGps,

        control,
        controlStatus,

        
        wlInfo,
        wlsendArs,

        wlCall,
        wlCallStatus,

        wlPlay,
        wlPlayStatus,

        wlmessage,
        wlmessageStatus,

        wlqueryGps,
        wlqueryGpsStatus,
        wlsendGps,
    };

}
