using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.Threading.Tasks;

using Sigmar;
using Sigmar.Extension;
using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class CTServer:TRX
    {
        private CTcpClient s_Tcp;

        //private string m_Host = "127.0.0.1";
        private string m_Host = "192.168.2.108";
        private int m_Port = 9000;
        private int m_ImagePort = 8001;
        private string m_ImageFolder = "images";
        public string ImageUrl { get { return "http://" + m_Host + ":" + m_ImagePort.ToString() + "/" + m_ImageFolder + "/"; } }


        public string Host { get { return m_Host; } }
        public int Port { get { return m_Port; } }

        private long s_CallID = 0;
        private bool s_IsInitialized = false;

        private TServerResponse s_Reponse;

        public bool IsInitialized { get { return s_IsInitialized; } }

        public delegate void ReceiveReponseHandel(string contents);
        public delegate void EventHandel(long req);
        public delegate void ReceiveRequestHandele(RequestOpcode call, RequestType type, object param);


        public delegate void StatusHandel(bool isinit);
        public event StatusHandel OnStatusChanged;
        public event ReceiveRequestHandele OnReceiveRequest;

        public event Action<object, string> SendTimeout;
        public event Action<object, string> SendFailure;

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

        private CTServer():base(0,3000,3)
        {

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
               
                base.WaitResponseTimeout += delegate(long seq) {
                    if(_requestList.ContainsKey(seq))
                    {
                        if (SendTimeout != null) SendTimeout(this, _requestList[seq]);
                        _requestList.Remove(seq);
                    }

                    Log.Error(string.Format("Request:{0} Response Timeout!", seq));
                };
                base.WaitReplyTimeout += delegate(long seq) {
                    if (_requestList.ContainsKey(seq))
                    {
                        if (SendTimeout != null) SendTimeout(this, _requestList[seq]);
                        _requestList.Remove(seq);
                    }
                    Log.Error(string.Format("Request:{0} Reply Timeout!", seq));
                };                           
            }

            s_Tcp.Connect(m_Host, m_Port);    
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
                            TServerResponse response = JsonConvert.DeserializeObject<TServerResponse>(jsonstr);
                            if (response != null) OnReceiveResponse(response.callId, response);
                        }
                        else
                        {
                            //request
                            TServerRequest rxrequest = JsonConvert.DeserializeObject<TServerRequest>(jsonstr);

                            if (rxrequest != null)
                            {
                                if (rxrequest.CallId > s_CallID) s_CallID = rxrequest.CallId;

                                string response = JsonConvert.SerializeObject(new TServerResponse()
                                {
                                    status = "success",
                                    callId = rxrequest.CallId,
                                });

                                Response(rxrequest.CallId, Encoding.UTF8.GetBytes(response));

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
       
        private string BuildJson(RequestOpcode call, RequestType type, object param)
        {
            s_CallID += 1;

            string json = string.Empty;

            if (param != null)
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

            return json;
        }


        private Dictionary<long, string> _requestList = new Dictionary<long, string>();

        public string[] Request(RequestOpcode call, RequestType type, object param)
        {
            Session session = param as Session;

            try
            {
                string json = BuildJson(call, type, param);

                if (json != string.Empty)
                {
                    if (session != null && !_requestList.ContainsKey(s_CallID)) _requestList.Add(s_CallID, session.guid);
                    TServerResponse res = RequestWithoutReply<TServerResponse>(s_CallID, Encoding.UTF8.GetBytes(json));
                    
                    if (res == null) return null;
                    return new string[2] { res.status, JsonConvert.SerializeObject(res.contents, Formatting.None) };
                }

                if (session != null && SendFailure != null) SendFailure(this, session.guid);

                Log.Warning("Request Failure.");
                return null; 
              
            }
            catch (Exception ex)
            {
                if (session != null && SendFailure != null) SendFailure(this, session.guid);
                Log.Warning("Request Failure.", ex);
                return null;
            }
        }


        protected override bool SendBytes(byte[] bytes)
        {
            if (s_Tcp == null || !s_Tcp.IsConnect) return false;
            s_Tcp.Write(bytes);
            return true;
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

        setMnisSetting,
        getMnisSetting,

        setLocationSetting,
        getLocationSetting,

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

        location,
        locationStatus,
        sendBeacons,
        sendGps,

        control,
        controlStatus,
        controlResult,


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
