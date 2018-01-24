using System;
using System.Text;
using System.Text.RegularExpressions;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;

using Sigmar;
using Sigmar.Extension;
using Dispatcher;

namespace Dispatcher.Service
{
    public class CLogServer:Transceiver
    {
        private CTcpClient s_Tcp;

        private string m_Host = "127.0.0.1";
        //private string m_Host = "192.168.2.128";
        private int m_Port = 9003;

        private long s_CallID = 0;
        private bool s_IsInitialized = false;

        private int TimoutTimes = 0;

        public string Host { get { return m_Host; } }
        public int Port { get { return m_Port; } }

        public bool IsInitialized { get { return s_IsInitialized; } }

        public delegate void ReceiveReponseHandel(string contents);
        public delegate void EventHandel(long req);
        public delegate void ReceiveRequestHandele(RequestOpcode call, object param);


        public delegate void StatusHandel(bool isinit);
        public event StatusHandel OnStatusChanged;
        public event ReceiveRequestHandele OnReceiveRequest;


        public event EventHandler Timeout;

        private readonly object m_RequestLockHelper = new object();


        private volatile static CLogServer _instance = null;
        private static readonly object lockHelper = new object();

        public static CLogServer Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new CLogServer();
                }
            }
            return _instance;
        }

        private CLogServer() :base(0,3000,3)
        {
            this.OnReceiveRequest += new ReceiveRequestHandele(OnLogServerReceiveRequest); 
        }

        private void OnLogServerReceiveRequest(RequestOpcode call, object param)
        {
            switch(call)
            {
                case RequestOpcode.status:
                case RequestOpcode.wlInfo:
                    if (param != null) OnStatusUpdate(JsonConvert.SerializeObject(param));
                    break;
            }        
        }

        public void GetGlobalStatus()
        {
            RequestOpcode opcode = RequestOpcode.status;
            var param = new StatusParameter() { getType = (long)StatusType_t.GlobalStatus };
            Request(opcode, param);
        }
        private void OnStatusUpdate(string parameter)
        {
            if (parameter == null || parameter == "") return;
            StatusParameter _status = null;

            try
            {
                _status = JsonConvert.DeserializeObject<StatusParameter>(parameter);
            }
            catch
            {
                //WARNING("Parse  RadioStatusParam Error");
                return;
            }

            if (_status == null) return;

            try
            {
                if (_status.getType == (long)StatusType_t.GlobalStatus) OnGlobalStatus(JsonConvert.DeserializeObject<GlobalStatus>(JsonConvert.SerializeObject(_status.info)));
            }
            catch
            {
                //WARNING(ex);
            }
        }

        private void OnGlobalStatus(GlobalStatus status)
        {
            ServerStatus.Instance().SetDatabaseStatus(status);
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
                    if (Timeout != null) Timeout(this, new EventArgs());
                    Log.Error(string.Format("Request:{0} Response Timeout!", seq));

                   if(++TimoutTimes > 100)
                   {
                       TimoutTimes = 0;
                       s_Tcp = new CTcpClient();

                       s_Tcp.OnConnected += delegate { if (OnStatusChanged != null)OnStatusChanged(true); };
                       s_Tcp.OnDisconnected += delegate { if (OnStatusChanged != null)OnStatusChanged(false); };
                       s_Tcp.OnRecvData += OnReceiveBytes;
                   }

                
                };
                base.WaitReplyTimeout += delegate(long seq) {
                    if (Timeout != null) Timeout(this, new EventArgs());
                    Log.Error(string.Format("Request:{0} Reply Timeout!", seq));

                    if (++TimoutTimes > 100)
                    {
                        TimoutTimes = 0;
                        s_Tcp = new CTcpClient();

                        s_Tcp.OnConnected += delegate { if (OnStatusChanged != null)OnStatusChanged(true); };
                        s_Tcp.OnDisconnected += delegate { if (OnStatusChanged != null)OnStatusChanged(false); };
                        s_Tcp.OnRecvData += OnReceiveBytes;
                    }
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
                            LogServerResponse response = JsonConvert.DeserializeObject<LogServerResponse>(jsonstr);

                            if (response != null) OnReceiveResponse(response.callId, response);

                            Log.Info(string.Format("Log Receive Response:{0}.", response.callId));
                            
                        }
                        else
                        {
                            //request
                            LogServerRequest rxrequest = JsonConvert.DeserializeObject<LogServerRequest>(jsonstr);
                            Log.Info(string.Format("Log Receive Request:{0}.", rxrequest.Call.ToString()));

                            if (rxrequest != null)
                            {
                                if (rxrequest.CallId > s_CallID) s_CallID = rxrequest.CallId;
                                string response = JsonConvert.SerializeObject(new TServerResponse()
                                {
                                    status = "success",
                                    callId = rxrequest.CallId,
                                });

                                Response(rxrequest.CallId, Encoding.UTF8.GetBytes(response));

                                if (OnReceiveRequest != null) OnReceiveRequest(rxrequest.Call, rxrequest.Param);
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



        private string BuildJson(RequestOpcode call, object param)
        {
            s_CallID += 1;

            LogServerRequest requset = new LogServerRequest()
            {
                Call = call,
                CallId = s_CallID,
                Param = param
            };

            JsonSerializerSettings jsetting = new JsonSerializerSettings();
            jsetting.NullValueHandling = NullValueHandling.Ignore;
            string json = JsonConvert.SerializeObject(requset, Formatting.None, jsetting);

            return json;
        }



        public string[] Request(RequestOpcode call, object param)
        {
            lock (m_RequestLockHelper)
            {
                try
                {
                    string json = BuildJson(call, param);

                    if (json != string.Empty)
                    {
                        LogServerResponse res = RequestWithoutReply<LogServerResponse>(s_CallID, Encoding.UTF8.GetBytes(json));

                        Log.Info(string.Format("Log Request:{0}({1}).", call.ToString(), s_CallID));

                        if (res == null) return null;
                        return new string[2] { res.status, JsonConvert.SerializeObject(res.contents, Formatting.None) };
                    }

                    Log.Warning("Request Failure.");
                    return null; 
                }
                catch
                {
                    return null;
                }
            }
        }

        protected override bool SendBytes(byte[] bytes)
        {
            if (s_Tcp == null || !s_Tcp.IsConnect) return false;
            s_Tcp.Write(bytes);
            return true;
        }
    }

    public class LogServerRequest
    {
        [JsonIgnore]
        public RequestOpcode Call;

        [JsonProperty(PropertyName = "call")]
        public string callStr { get { return Call.ToString(); } set { Call = (RequestOpcode)Enum.Parse(typeof(RequestOpcode), value); } }

        [JsonProperty(PropertyName = "callId")]
        public long CallId;
        [JsonProperty(PropertyName = "param")]
        public object Param;

    }

    public class LogServerResponse
    {
        public string status;
        public string statusText;
        public int errCode;
        public long callId;
        public object contents;
    }
}
