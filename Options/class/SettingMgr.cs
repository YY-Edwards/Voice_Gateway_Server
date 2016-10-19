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


namespace TrboX
{
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
        public int Com{set; get;}
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
    public class Setting
    {
        public BaseSetting Base { set; get; }
        public RadioSetting Radio{set; get;}
        public WireLanSetting WireLan{set; get;}
    }


    public enum RequestType
    {
        //base
        setBase,
        getBase,

        setRadio,
        getRadio,

        setRepeater,
        getRepeater,

    };

    class SettingMgr
    {
        private Setting m_Setting = new Setting();
        private TcpInterface TServer = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8000));
       
        public Setting GetDefalut()
        {
            string path = App.SettingTempPath;
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            Uri uri = new Uri("/DefaultConfiguration.txt", UriKind.Relative);
            StreamResourceInfo info = Application.GetResourceStream(uri);
            byte[] rx = new byte[10240];
            info.Stream.Read(rx, 0, (int)info.Stream.Length);
            string str = Encoding.ASCII.GetString(rx);
            str = str.Substring(str.IndexOf("{"), str.LastIndexOf("}") - str.IndexOf("{") + 1);
            return JsonConvert.DeserializeObject<Setting>(str);
        }
        public void Set(Setting setting)
        {
            CallRpc(RequestType.setBase,setting.Base, new ParseResult(ParseStatus));
            CallRpc(RequestType.setRadio,setting.Radio, new ParseResult(ParseStatus));
            CallRpc(RequestType.setRepeater,setting.WireLan, new ParseResult(ParseStatus));
        }

        public Setting Get()
        {
            Setting setting = new Setting();
            setting.Base = (BaseSetting)CallRpc(RequestType.getBase, new ParseResult(ParseBase));
            setting.Radio = (RadioSetting)CallRpc(RequestType.getRadio, new ParseResult(ParseRadio));
            setting.WireLan = (WireLanSetting)CallRpc(RequestType.getRepeater, new ParseResult(ParseWireLan));

            return setting;
        }

        delegate object ParseResult(TcpResponse res);

        private object CallRpc(RequestType type, ParseResult Response = null)
        {
            return CallRpc(type, null, Response);
        }

        private object CallRpc(RequestType type, object param, ParseResult Response=null)
        {

            TServer.WriteString(JsonParse.Req2Json(type, param));
            if(null != Response)return Response(TServer.ReadString(JsonParse.CallID) as TcpResponse);
            return null;
        }
        private object ParseStatus(TcpResponse res)
        {
            if (null == res) return false;
            return res.IsSuccess;
        }
        private object ParseBase(TcpResponse res)
        {
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<BaseSetting>(JsonConvert.SerializeObject(res.contents));
        }

        private object ParseRadio(TcpResponse res)
        {
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<RadioSetting>(JsonConvert.SerializeObject(res.contents));
        }

        private object ParseWireLan(TcpResponse res)
        {
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<WireLanSetting>(JsonConvert.SerializeObject(res.contents));
        }


    }
}
