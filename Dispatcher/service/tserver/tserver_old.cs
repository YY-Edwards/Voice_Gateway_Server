using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.Threading.Tasks;

using Sigmar.Extension;
using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class CTServer
    {
        private CTcpClient s_Tcp;
        private CSender s_Sender;

        private string m_Host = "127.0.0.1";
        //private string m_Host = "192.168.2.133";
        private int m_Port = 9000;
        private int m_ImagePort = 8001;
        private string m_ImageFolder = "images";
        public string ImageUrl { get { return "http://" + m_Host + ":" + m_ImagePort.ToString() + "/" + m_ImageFolder + "/"; } }


        private long s_CallID = 0;
        private bool s_IsInitialized = false;

        private Semaphore m_WaitReponse;
        private TServerResponse s_Reponse;

        public bool IsInitialized { get { return s_IsInitialized; } }

        public delegate void ReceiveReponseHandel(string contents);
        public delegate void EventHandel(long req);
        public delegate void ReceiveRequestHandele(RequestOpcode call, RequestType type, object param);


        public delegate void StatusHandel(bool isinit);
        public event StatusHandel OnStatusChanged;
        public event ReceiveRequestHandele OnReceiveRequest;

        public event EventHandler Timeout;

        private readonly object m_RequestLockHelper = new object();


        private volatile static CTServer _instance = null;
        private static readonly object lockHelper = new object();

        public static CTServer Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new CTServer();
                }
            }
            return _instance;
        }

        public void Initialize()
        {
            if (!s_IsInitialized)
            {
                s_IsInitialized = true;
                if (s_Tcp == null || !s_Tcp.IsConnect)
                {
                    s_Tcp = new CTcpClient();

                    s_Tcp.OnConnected += delegate { if (OnStatusChanged != null)OnStatusChanged(true); };
                    s_Tcp.OnDisconnected += delegate { if (OnStatusChanged != null)OnStatusChanged(false); };
                    s_Tcp.OnRecvData += OnReceiveBytes;
                }
                if (s_Sender == null)
                {
                    s_Sender = new CSender();
                    s_Sender.OnTimeout += delegate { if (m_WaitReponse != null)try { m_WaitReponse.Release(); } catch { } if (Timeout != null)Timeout(this, new EventArgs()); };
                }
                s_Tcp.Connect(m_Host, m_Port);               
            }

        }

        private void SendJson(string json)
        {
            if (s_Tcp == null || !s_Tcp.IsConnect) return;
            s_Tcp.Write(Encoding.UTF8.GetBytes(json));

        }

        private string _untreatedjson = string.Empty;
        private void OnReceiveBytes(object sender, byte[] bytes)
        {
            try
            {
                string str = _untreatedjson + Encoding.UTF8.GetString(bytes);
                string[] Jsons = str.ToJsons();
                if (Jsons.Length <= 0) return;

                _untreatedjson = Jsons[Jsons.Length - 1];

                for (int i = 0; i < Jsons.Length - 1;i++ )
                {
                    string jsonstr = Jsons[i];
                    try
                    {
                       
                        JObject json = JsonConvert.DeserializeObject<JObject>(jsonstr);

                        if (json.Property("call") == null || json.Property("call").ToString() == string.Empty)
                        {
                            //response
                            s_Reponse = JsonConvert.DeserializeObject<TServerResponse>(jsonstr);

                            if (s_Reponse != null) s_Sender.End(s_Reponse.callId);

                            if (m_WaitReponse != null)
                            {
                                try
                                {
                                    m_WaitReponse.Release();
                                }
                                catch
                                {

                                }
                            }
                        }
                        else
                        {
                            //request
                            TServerRequest rxrequest = JsonConvert.DeserializeObject<TServerRequest>(jsonstr);

                            if (rxrequest != null)
                            {
                                if (rxrequest.CallId > s_CallID) s_CallID = rxrequest.CallId;
                                SendJson(JsonConvert.SerializeObject(new TServerResponse()
                                {
                                    status = "success",
                                    callId = rxrequest.CallId,
                                }));

                                if (OnReceiveRequest != null) OnReceiveRequest(rxrequest.Call, rxrequest.Type, rxrequest.Param);
                            }
                        }
                    }
                    catch
                    {
                        continue;
                    }
                }
            }
            catch
            {
            }
        }

        public string[] Request(RequestOpcode call, RequestType type, object param)
        {

            lock (m_RequestLockHelper)
            {
                try
                {

                    m_WaitReponse = new Semaphore(0, 1);

                    s_CallID += 1;

                    s_Reponse = null;

                    string json = string.Empty;

                    if(param != null)
                    {
                        TServerRequest requset = new TServerRequest()
                        {
                            Call = call,
                            Type = type,
                            CallId = s_CallID,
                            Param = param
                        };

                        json = JsonConvert.SerializeObject(requset, Formatting.None);
                    }
                    else
                    {
                        TServerRequestNulParam requset = new TServerRequestNulParam()
                        {
                            Call = call,
                            Type = type,
                            CallId = s_CallID,
                        };

                        json = JsonConvert.SerializeObject(requset, Formatting.None);                       
                    }
                   

                    if (json != string.Empty)
                    {
                        s_Sender.Begin(s_CallID, 3000, 3, delegate { SendJson(json ); });
                        try
                        {
                            if (m_WaitReponse != null) m_WaitReponse.WaitOne();
                        }
                        catch{

                        }
                    }

                    return new string[2] { 
                        s_Reponse.status,
                        JsonConvert.SerializeObject(s_Reponse.contents, Formatting.None)
                    };
                }
                catch(Exception ex)
                {
                    Log.Warning("Request Failure.", ex);
                    return null;
                }
            }
        }
    }

    public class TServerRequest
    {
        [JsonIgnore]
        public RequestOpcode Call;

        [JsonIgnore]
        public RequestType Type;

        [JsonProperty(PropertyName = "call")]
        public string callStr { get { return Call.ToString(); } set {
            try
            {
                Call = (RequestOpcode)Enum.Parse(typeof(RequestOpcode), value);
            }
            catch (Exception ex)
            {
                Log.Error(string.Format("Prase Call Type Error({0})", value), ex);
            }
        }
        }

        [JsonProperty(PropertyName = "type")]
        public string typestr
        {
            get { return Type.ToString(); }
            set
            {
                try
                {
                    Type = (RequestType)Enum.Parse(typeof(RequestType), value);
                }
                catch (Exception ex)
                {
                    Log.Error(string.Format("Prase Request Type Error({0})", value),ex);
                }
            }
        }

        [JsonProperty(PropertyName = "callId")]
        public long CallId;

        [JsonProperty(PropertyName = "param")]
        public object Param;
    }

    public class TServerRequestNulParam
    {
        [JsonIgnore]
        public RequestOpcode Call;

        [JsonIgnore]
        public RequestType Type;

        [JsonProperty(PropertyName = "call")]
        public string callStr
        {
            get { return Call.ToString(); }
            set
            {
                try
                {
                    Call = (RequestOpcode)Enum.Parse(typeof(RequestOpcode), value);
                }
                catch (Exception ex)
                {
                    Log.Error(string.Format("Prase Call Type Error({0})", value), ex);
                }
            }
        }

        [JsonProperty(PropertyName = "type")]
        public string typestr
        {
            get { return Type.ToString(); }
            set
            {
                try
                {
                    Type = (RequestType)Enum.Parse(typeof(RequestType), value);
                }
                catch (Exception ex)
                {
                    Log.Error(string.Format("Prase Request Type Error({0})", value), ex);
                }
            }
        }

        [JsonProperty(PropertyName = "callId")]
        public long CallId;
    }

    public class TServerResponse
    {
        public string status;
        public string statusText;
        public int errCode;
        public long callId;
        public object contents;
    }

    public enum RequestType
    {
        wl,
        radio
    }

    public enum RequestOpcode
    {
        None,
        //base
        setBaseSetting,
        getBaseSetting,
        setRadioSetting,
        getRadioSetting,
        setRepeaterSetting,
        getRepeaterSetting,

        queryLicense,
        registerLicense,

        setLocationInDoorSetting,
        getLocationInDoorSetting,

        user,
        department,
        radio,
        staff,

        smslog,
        gpslog,
        voicelog,

        status,
        sendArs,

        call,
        callStatus,

        message,
        messageStatus,

        queryGps,
        queryGpsStatus,
        sendGpsStatus,
        sendGps,

        control,
        controlStatus,


        wlInfo,

        wlCall,
        wlCallStatus,

        wlPlay,
        wlPlayStatus,


        area,
        ibeacon,
        locationIndoor,
        locatioindoorlog,
        locationLog,

    };
}
