using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;

namespace Manager
{
    public class CConfiguration
    {
        private static bool m_IsTServerConnected = false;

        private bool m_NeedSave = false;
        private bool m_IsUpdated = false;
        private SettingType m_Type;
        private RequestOpcode m_GetOpcode;
        private RequestOpcode m_SetOpcode;
        
        [JsonIgnore]
        public object m_Object;

        public delegate void ConfigurationChangedHandle(SettingType type, object config);
        public event ConfigurationChangedHandle OnConfigurationChanged;

        public delegate void ServerStatusChangedHandle(bool isinit);
        public static event ServerStatusChangedHandle ServerStatusChanged;

        [JsonIgnore]
        public static bool IsBindingServerStatusChanged { get { return ServerStatusChanged != null; } }

        public CConfiguration(SettingType type, RequestOpcode set, RequestOpcode get)
        {
            m_Type = type;
            m_SetOpcode = set;
            m_GetOpcode = get;
        }

        public static void InitializeTServer()
        {
            if (!CTServer.Instance().IsInitialized)
            {
                CTServer.Instance().OnReceiveRequest += delegate { };
                CTServer.Instance().OnStatusChanged += delegate(bool isinit) { m_IsTServerConnected = isinit;  if (ServerStatusChanged != null)ServerStatusChanged(isinit); };
            }

            CTServer.Instance().Initialize();      
        }

        private void Request(RequestOpcode opcode, object parameter, bool isget = false)
        {
            if (!m_IsTServerConnected) InitializeTServer();
            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    string reply = CTServer.Instance().Request(opcode, RequestType.radio, parameter);
                    m_NeedSave = false;

                    if (isget)
                    {
                        m_Object = Parse(reply);                      
                        if (OnConfigurationChanged != null) OnConfigurationChanged(m_Type, m_Object);
                        m_IsUpdated = true;
                    }
                }
                catch
                {

                }
            })).Start();
        }

        public virtual object Parse(string json)
        {
            return null;
        }


        public virtual object Build(object obj)
        {
            return obj;
        }
        public virtual void Get(bool islocal = true)
        {
            if(islocal && m_IsUpdated)
            {
                m_NeedSave = false;
                if (OnConfigurationChanged != null) OnConfigurationChanged(m_Type, m_Object);
            }
            else
            {
                Request(m_GetOpcode, null, true);
            }
           
        }

        public virtual void NeedSave()
        {
            m_NeedSave = true;
        }

        public virtual void Set()
        {
            if (!m_NeedSave) return;
            m_Object = this;
            m_IsUpdated = true;
            Request(m_SetOpcode,Build(this));
        }
    }


    public class NetAddress
    {
        public string Ip { get; set; }
        public int Port { get; set; }
    };

    //public enum TargetSystemType
    //{
    //    radio,
    //    Reapeater
    //}
    //public class NetAddress
    //{
    //    public string Ip { get; set; }
    //    public int Port;
    //};

    //public class BaseSetting
    //{
    //    public NetAddress Svr{get; set;}
    //    public NetAddress LogSvr;
    //    public bool IsSaveCallLog;
    //    public bool IsSaveMsgLog;
    //    public bool IsSavePositionLog;
    //    public bool IsSaveControlLog;
    //    public bool IsSaveJobLog;
    //    public bool IsSaveTrackerLog;
    //}

    //public class RadioSetting
    //{
    //    public bool IsEnable;
    //    public bool IsOnlyRide;
    //    public NetAddress Svr;
    //    public NetAddress Ride;
    //    public NetAddress Mnis;
    //    public NetAddress GpsC;
    //    public NetAddress Ars;
    //    public NetAddress Message;
    //    public NetAddress Gps;
    //    public NetAddress Xnl;
    //}
    //public enum WireLanType
    //{
    //    IPSC = 0,
    //    CPC = 1,
    //    LCP = 2,
    //};


    //public class DongleSetting
    //{
    //    public int Com;
    //}

    //public class WireLanSetting
    //{
    //    public bool IsEnable;
    //    public WireLanType Type;
    //    public NetAddress Svr;
    //    public NetAddress Master;
    //    public NetAddress Mnis;
    //    public int MnisId;
    //    public int DefaultGroupId;
    //    public int DefaultChannel;
    //    public int MinHungTime;

    //    public int MaxSiteAliveTime;
    //    public int MaxPeerAliveTime;

    //    public int LocalPeerId;
    //    public int LocalRadioId;

    //    public DongleSetting Dongle;

    //}

    public enum SettingType
    {
        Base,
        Radio,
        Repeater,
        QueryRegister,
        Register
    };

    //public enum Device
    //{
    //    Radio = 1,
    //    Repeater = 2,
    //    Portable = 3,
    //    PC = 4,
    //}

    //public class SettingResponse
    //{
    //    public string status { set; get; }
    //    public string statusText { set; get; }
    //    public int errCode { set; get; }
    //    public Int64 callId { set; get; }
    //    public object contents { set; get; }

    //    public bool IsSuccess
    //    {
    //        get
    //        {
    //            if ("success" == status) return true;
    //            else return false;
    //        }
    //    }
    //}
    //public class LisenceRes
    //{
    //    public Device DeviceType;
    //    public string RadioMode;
    //    public string RadioSerial;
    //    public string RepeaterMode;
    //    public string RepeaterSerial;
    //    [JsonProperty(PropertyName = "Time")]
    //    public string time;
    //    public int IsEver;
    //    [JsonProperty(PropertyName = "Expiration")]
    //    public string expiration;
    //    [JsonIgnore]
    //    public bool IsOK;
    //    [JsonIgnore]
    //    public DateTime Time
    //    {
    //        get
    //        {
    //            return DateTime.ParseExact(time, "yyyyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
    //        }
    //    }
    //    [JsonIgnore]
    //    public DateTime Expiration
    //    {
    //        get
    //        {
    //            return DateTime.ParseExact(expiration, "yyyyMMdd", System.Globalization.CultureInfo.CurrentCulture);
    //        }
    //    }
    //}
}
