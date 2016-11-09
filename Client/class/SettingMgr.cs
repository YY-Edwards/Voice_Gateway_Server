using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Resources;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.IO;
using System.Xml;
using System.Xml.Serialization;

using System.Xml.Linq;

using System.Net;
using System.Net.Sockets;

using System.Threading;


namespace TrboX
{

    public enum TargetSystemType
    {
        radio,
        Reapeater
    }
    public class NetAddress
    {
        public string Ip;
        public int Port;
    };



    public class BaseSetting
    {
        public NetAddress Svr;
        public NetAddress LogSvr;
        public bool IsSaveCallLog;
        public bool IsSaveMsgLog;
        public bool IsSavePositionLog;
        public bool IsSaveControlLog;
        public bool IsSaveJobLog;
        public bool IsSaveTrackerLog;
    }

    public class RadioSetting
    {
        public bool IsEnable;
        public bool IsOnlyRide;
        public NetAddress Svr;
        public NetAddress Ride;
        public NetAddress Mnis;
        public NetAddress Gps;
        public NetAddress Ars;
        public NetAddress Message;
    }
    public enum WireLanType
    {
        IPSC,
        CPC,
        LCP,
    };


    public class DongleSetting
    {
        public int Com;
    }

    public class WireLanSetting
    {
        public bool IsEnable;
        public WireLanType Type;
        public NetAddress Svr;
        public NetAddress Master;
        public int DefaultGroupId;
        public int DefaultChannel;
        public int MinHungTime;

        public int MaxSiteAliveTime;
        public int MaxPeerAliveTime;

        public int LocalPeerId;
        public int LocalRadioId;

        public DongleSetting Dongle;

    }

    public enum SettingOpType
    {
        Set,
        Get,
    };
    public enum SettingType
    {
        Base,
        Radio,
        WireLan,
    };



    public class Setting
    {
        public SettingOpType Op;
        public SettingType Type;
        public object Configure;


        public void Set()
        {
            Op = SettingOpType.Set;
            new Thread(new ThreadStart(delegate() { TServer.Call(this); })).Start();  
        }
        public object Get()
        {
            Op = SettingOpType.Get;
            return TServer.Call(this);
        }
    }

    public class SettingMgrStr
    {
        public string call;
        public string callId;
        public object param;
    }

    public class SettingMgrSimpleStr
    {
        public string call;
        public string callId;
    }

    public class SettingResponse
    {
        public string status { set; get; }
        public string statusText { set; get; }
        public int errCode { set; get; }
        public Int64 callId { set; get; }
        public object contents { set; get; }

        public bool IsSuccess
        {
            get
            {
                if ("success" == status) return true;
                else return false;
            }
        }
    }


    class SettingMgr
    {
        public RequestType Call;
        public long CallId;
        public object Param;

        public ParseDel Parse;

        public SettingMgr(Setting setting, long PN)
        {
            Call = RequestType.None;
            if (setting.Op == SettingOpType.Get)
            {
                if (setting.Type == SettingType.Base)
                {
                    Call = RequestType.getBaseSetting;
                    Parse = ParseBase;
                }
                else if (setting.Type == SettingType.Radio)
                {
                    Call = RequestType.getRadioSetting;
                    Parse = ParseRadio;
                }
                else if (setting.Type == SettingType.WireLan)
                {
                    Call = RequestType.getRepeaterSetting;
                    Parse = ParseWireLan;
                }
            }
            else if (setting.Op == SettingOpType.Set)
            {
                if (setting.Type == SettingType.Base) Call = RequestType.setBaseSetting;
                else if (setting.Type == SettingType.Radio) Call = RequestType.setRadioSetting;
                else if (setting.Type == SettingType.WireLan) Call = RequestType.setRepeaterSetting;

                Parse = ParseStatus;
            }

            CallId = PN;
            Param = setting.Configure;
        }


        public string Json
        {
            get
            {
                return JsonConvert.SerializeObject(this.ToStr()) + "\r\n";
            }
        }

        public object ToStr()
        {
            if (null == Param) return new SettingMgrSimpleStr()
            {
                call = Call.ToString(),
                callId = CallId.ToString(),
            };
            else return new SettingMgrStr()
            {
                call = Call.ToString(),
                callId = CallId.ToString(),
                param = Param
            };
        }

        private static object ParseStatus(object obj)
        {
            TServerResponse res = null;
            if (obj is TServerResponse) res = obj as TServerResponse;
            else return null;

            if (null == res) return false;
            return res.status == "success";
        }
        private static object ParseBase(object obj)
        {
            TServerResponse res = null;
            if (obj is TServerResponse) res = obj as TServerResponse;
            else return null;

            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<BaseSetting>(JsonConvert.SerializeObject(res.contents));
        }


        private static object ParseRadio(object obj)
        {
            TServerResponse res = null;
            if (obj is TServerResponse) res = obj as TServerResponse;
            else return null;

            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<RadioSetting>(JsonConvert.SerializeObject(res.contents));
        }

        private static object ParseWireLan(object obj)
        {
            TServerResponse res = null;
            if (obj is TServerResponse) res = obj as TServerResponse;
            else return null;
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<WireLanSetting>(JsonConvert.SerializeObject(res.contents));
        }

        public static void Set(List<Setting> setting)
        {
            if (null == setting) return;
            foreach (Setting set in setting)
            {
                set.Set();
            }
        }

        public static List<Setting> Get(SettingType[] lst)
        {
            List<Setting> setting = new List<Setting>();

            foreach (SettingType type in lst)
            {
                setting.Add(new Setting() { Type = type, Configure = new Setting() { Type = type }.Get() });
            }
            return setting;
        }
        public static List<Setting> Get()
        {
            return Get(new SettingType[] {
                SettingType .Base,
                SettingType .Radio,
                SettingType .WireLan,
            });
        }
    }
}
