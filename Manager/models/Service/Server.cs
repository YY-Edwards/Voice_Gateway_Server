using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Sigmar;
using Sigmar.Extension;
using System;
using System.Collections.Generic;
using System.Text;

namespace Manager.Models
{
    public abstract class Server : TRX
    {
        public class Request
        {
            [JsonIgnore]
            public RequestOpcode Call
            {
                get
                {
                    try
                    {
                        return callStr.ToEnum<RequestOpcode>();
                    }
                    catch
                    {
                        return RequestOpcode.None;
                    }
                }
                set
                {
                    callStr = value.ToString();
                }
            }

            [JsonIgnore]
            public RequestType Type
            {
                get
                {
                    try
                    {
                        return typeStr.ToEnum<RequestType>();
                    }
                    catch
                    {
                        return RequestType.radio;
                    }
                }
                set
                {
                    typeStr = value.ToString();
                }
            }

            [JsonProperty(PropertyName = "call")]
            public string callStr;

            [JsonProperty(PropertyName = "type")]
            public string typeStr;

            [JsonProperty(PropertyName = "callId")]
            public long CallId;

            [JsonProperty(PropertyName = "param")]
            public object Param;

            public string Json
            {
                get
                {
                    return BuildJson();
                }
            }

            private string BuildJson()
            {
                Dictionary<string, object> request = new Dictionary<string, object>();
                request.Add("call", callStr);
                request.Add("type", typeStr);
                request.Add("callId", CallId);

                if (Param != null)
                {
                    request.Add("param", Param);
                }
                try
                {
                    return JsonConvert.SerializeObject(request, Formatting.None);
                }
                catch
                {
                    return "";
                }
            }
        }

        public class Response
        {
            public string status;
            public string statusText;
            public int errCode;
            public long callId;
            public JObject contents;
        }

        public bool IsInitialized { get; private set; }
        public string Host { get; protected set; }
        public int Port { get; protected set; }

        public event Action<object, bool> StatusChanged;

        public event Action<object, Request> ReceivedRequest;

        public event Action<object, Request> Timeout;

        private CTcpClient _Tcp;
        private long _CallId;
        private Dictionary<long, Request> _RequestList = new Dictionary<long, Request>();

        public Server()//:base(0,3000,1)
        {
            IsInitialized = false;

            if (_Tcp == null)
            {
                _Tcp = new CTcpClient();
                _Tcp.StatusChanged += new Action<object, bool>(OnTcpStatusChanged);
                _Tcp.ReceivedBytes += new Action<object, byte[]>(OnTcpReceivedBytes);
            }

            base.WaitResponseTimeout += delegate(long seq)
            {
                lock (_RequestList)
                {
                    if (Timeout != null) Timeout(this, _RequestList.ContainsKey(seq) ? _RequestList[seq] : null);
                }
            };

            base.WaitReplyTimeout += delegate(long seq)
            {
                lock (_RequestList)
                {
                    if (Timeout != null) Timeout(this, _RequestList.ContainsKey(seq) ? _RequestList[seq] : null);
                }
            };

            _CallId = 1;
        }

        private void OnTcpStatusChanged(object sender, bool isconnected)
        {
            if (isconnected)
            {
                IsInitialized = true;
            }
            else
            {
                IsInitialized = false;
            }

            if (StatusChanged != null) StatusChanged(this, IsInitialized);
        }

        private string _untreatedJson = string.Empty;

        private void OnTcpReceivedBytes(object sender, byte[] bytes)
        {
            try
            {
                string str = _untreatedJson + Encoding.UTF8.GetString(bytes);
                string[] Jsons = str.ToJsons();
                if (Jsons.Length <= 0) return;

                _untreatedJson = Jsons[Jsons.Length - 1];

                for (int i = 0; i < Jsons.Length - 1; i++)
                {
                    string jsonstr = Jsons[i];
                    try
                    {
                        JObject json = JsonConvert.DeserializeObject<JObject>(jsonstr);

                        if (json.Property("call") == null || json.Property("call").ToString() == string.Empty)
                        {
                            //response
                            Response response = JsonConvert.DeserializeObject<Response>(jsonstr);
                            if (response != null) OnReceiveResponse(response.callId, response);
                        }
                        else
                        {
                            //request
                            Request request = JsonConvert.DeserializeObject<Request>(jsonstr);

                            if (request != null)
                            {
                                if (request.CallId > _CallId) _CallId = request.CallId;

                                string response = JsonConvert.SerializeObject(new Response()
                                {
                                    status = "success",
                                    callId = request.CallId,
                                });

                                Response(request.CallId, Encoding.UTF8.GetBytes(response));
                                if (ReceivedRequest != null) ReceivedRequest(this, request);
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

        public virtual void InitializeServer()
        {
            if (IsInitialized)
            {
                if (StatusChanged != null) StatusChanged(this, IsInitialized);
            }
            else
            {
                _Tcp.Connect(Host, Port);
            }
        }

        private readonly object _RequestLockHelper = new object();

        public T SendRequest<T>(RequestOpcode call, RequestType type, object param = null)
            where T:class, new()
        {
            lock (_RequestLockHelper)
            {
                _CallId += 1;

                Request request = new Request()
                {
                    Call = call,
                    Type = type,
                    CallId = _CallId,
                    Param = param
                };

                lock (_RequestList)
                {
                    if (_RequestList.ContainsKey(_CallId)) _RequestList.Add(_CallId, request);
                }

                string json = request.Json;

                if (json != string.Empty)
                {
                   return RequestWithoutReply<T>(_CallId, Encoding.UTF8.GetBytes(json));
                }

                return null;
            }
        }

        protected override bool SendBytes(byte[] bytes)
        {
            if (!IsInitialized || _Tcp == null || !_Tcp.IsConnect) return false;
            _Tcp.Write(bytes);
            return true;
        }
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

        queryDevice,
        queryLicense,
        registerLicense,

        setLocationInDoorSetting,
        getLocationInDoorSetting,

        user,
        department,
        radio,
        staff,

        status,
        sendArs,

        call,
        callStatus,

        message,
        messageStatus,

        queryGps,
        queryGpsStatus,
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
    };

    public enum RequestType
    {
        wl,
        radio
    }
}