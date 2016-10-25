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
        public NetAddressStr GetNetAddressStr()
        {
            return new NetAddressStr()
            {
                Ip = Ip,
                Port = Port.ToString()
            };
        }
    };

    public class NetAddressStr
    {
        public string Ip;
        public string Port;

        public NetAddress GetNetAddress() {
            return new NetAddress()
            {
                Ip = Ip,
                Port = ((Port != "") && System.Text.RegularExpressions.Regex.IsMatch(Port, @"^\d+$")) ? int.Parse(Port):0
            };
            
        }
    }


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
        public BaseSettingStr GetBaseSettingStr() {
                return new BaseSettingStr()
                {
                    Svr = null == Svr ? new NetAddressStr() : Svr.GetNetAddressStr(),
                    LogSvr = null == LogSvr ? new NetAddressStr() : LogSvr.GetNetAddressStr(),
                    IsSaveCallLog = IsSaveCallLog.ToString(),
                    IsSaveMsgLog = IsSaveMsgLog.ToString(),
                    IsSavePositionLog = IsSavePositionLog.ToString(),
                    IsSaveControlLog = IsSaveControlLog.ToString(),
                    IsSaveJobLog = IsSaveJobLog.ToString(),
                    IsSaveTrackerLog = IsSaveTrackerLog.ToString(),
                };           
        }
    }

    public class BaseSettingStr
    {
        public NetAddressStr Svr;
        public NetAddressStr LogSvr;
        public string IsSaveCallLog;
        public string IsSaveMsgLog;
        public string IsSavePositionLog;
        public string IsSaveControlLog;
        public string IsSaveJobLog;
        public string IsSaveTrackerLog;

        public BaseSetting GetBaseSetting()
        {   
                return new BaseSetting()
                {
                    Svr = null == Svr ? new NetAddress() :Svr.GetNetAddress(),
                    LogSvr = null == LogSvr ? new NetAddress() : LogSvr.GetNetAddress(),
                    IsSaveCallLog = (IsSaveCallLog.ToUpper() == "TRUE") ? true : false,
                    IsSaveMsgLog = (IsSaveMsgLog.ToUpper() == "TRUE") ? true : false,
                    IsSavePositionLog = (IsSavePositionLog.ToUpper() == "TRUE") ? true : false,
                    IsSaveControlLog = (IsSaveControlLog.ToUpper() == "TRUE") ? true : false,
                    IsSaveJobLog = (IsSaveJobLog.ToUpper() == "TRUE") ? true : false,
                    IsSaveTrackerLog = (IsSaveTrackerLog.ToUpper() == "TRUE") ? true : false,
                };
           
        }

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

        public RadioSettingStr GetRadioSettingStr()
        {
            return new RadioSettingStr()
            {
                IsEnable = IsEnable.ToString(),
                IsOnlyRide = IsOnlyRide.ToString(),
                Svr = null == Svr ? new NetAddressStr() : Svr.GetNetAddressStr(),
                Ride = null == Ride ? new NetAddressStr() : Ride.GetNetAddressStr(),
                Mnis = null == Mnis ? new NetAddressStr() : Mnis.GetNetAddressStr(),
                Gps = null == Gps ? new NetAddressStr() : Gps.GetNetAddressStr(),
                Ars = null == Ars ? new NetAddressStr() : Ars.GetNetAddressStr(),
                Message = null == Message ? new NetAddressStr() : Message.GetNetAddressStr()           
            };
        }
    }

    public class RadioSettingStr
    {
        public string IsEnable;
        public string IsOnlyRide;
        public NetAddressStr Svr;
        public NetAddressStr Ride;
        public NetAddressStr Mnis;
        public NetAddressStr Gps;
        public NetAddressStr Ars;
        public NetAddressStr Message;

        public RadioSetting GetRadioSetting()
        { 
            return new RadioSetting(){
                IsEnable = (IsEnable.ToUpper() == "TRUE") ? true : false,
                IsOnlyRide = (IsEnable.ToUpper() == "TRUE") ? true : false,
                Svr = null == Svr ? new NetAddress(): Svr.GetNetAddress(),
                Ride = null == Ride ? new NetAddress() : Ride.GetNetAddress(),
                Mnis = null == Mnis ? new NetAddress() : Mnis.GetNetAddress(),
                Gps = null == Gps ? new NetAddress() : Gps.GetNetAddress(),
                Ars = null == Ars ? new NetAddress() : Ars.GetNetAddress(),
                Message = null == Message ? new NetAddress() : Message.GetNetAddress()  
            };
        }
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

        public DongleSettingStr GetDongleSettingStr()
        {
            return new DongleSettingStr()
            {
                Com = "COM"+Com.ToString()
            };
        }
    }

    public class DongleSettingStr
    {
        public string Com;
        public DongleSetting GetDongleSetting()
        {
            return new DongleSetting() {
                Com = ((Com != "") && System.Text.RegularExpressions.Regex.IsMatch(Com.Substring(3, Com.Length - 3), @"^\d+$")) ? int.Parse(Com.Substring(3, Com.Length - 3)) : 1
            };
        }

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

        public WireLanSettingStr GetWireLanSettingStr()
        {
            return new WireLanSettingStr()
            {
                IsEnable = IsEnable.ToString(),
                Type = Type.ToString(),
                Svr = null == Svr ? new NetAddressStr() :  Svr.GetNetAddressStr(),
                Master = null == Svr ? new NetAddressStr() :  Master.GetNetAddressStr(),                
                DefaultGroupId = DefaultGroupId.ToString(),
                DefaultChannel = DefaultChannel.ToString(),
                MinHungTime = MinHungTime.ToString(),
                MaxSiteAliveTime = MaxSiteAliveTime.ToString(),
                MaxPeerAliveTime = MaxPeerAliveTime.ToString(),
                LocalPeerId = LocalPeerId.ToString(),
                LocalRadioId = LocalRadioId.ToString(),
                Dongle = null == Dongle ? new DongleSettingStr() { Com = "COM1" } : Dongle.GetDongleSettingStr()
            };
        }
    }

    public class WireLanSettingStr
    {
        public string IsEnable;
        public string Type;
        public NetAddressStr Svr;
        public NetAddressStr Master;
        public string DefaultGroupId;
        public string DefaultChannel;
        public string MinHungTime;

        public string MaxSiteAliveTime;
        public string MaxPeerAliveTime;

        public string LocalPeerId;
        public string LocalRadioId;

        public DongleSettingStr Dongle;

        public WireLanSetting GetWireLanSetting()
        {
            return new WireLanSetting()
            {
                IsEnable = (IsEnable.ToUpper() == "TRUE") ? true : false,
                Type = (Type.ToUpper() == "LCP") ? WireLanType.LCP : ((Type.ToUpper() == "CPC") ? WireLanType.CPC : WireLanType.IPSC),
                Svr = null== Svr ? new NetAddress() : Svr.GetNetAddress(),
                Master = null == Master ? new NetAddress() : Master.GetNetAddress(),
                DefaultGroupId = ((DefaultGroupId != "") && System.Text.RegularExpressions.Regex.IsMatch(DefaultGroupId, @"^\d+$")) ? int.Parse(DefaultGroupId) : 0,
                DefaultChannel = ((DefaultChannel != "") && System.Text.RegularExpressions.Regex.IsMatch(DefaultChannel, @"^\d+$")) ? int.Parse(DefaultChannel) : 0,
                MinHungTime = ((MinHungTime != "") && System.Text.RegularExpressions.Regex.IsMatch(MinHungTime, @"^\d+$")) ? int.Parse(MinHungTime) : 0,
                MaxSiteAliveTime = ((MaxSiteAliveTime != "") && System.Text.RegularExpressions.Regex.IsMatch(MaxSiteAliveTime, @"^\d+$")) ? int.Parse(MaxSiteAliveTime) : 0,
                MaxPeerAliveTime = ((MaxPeerAliveTime != "") && System.Text.RegularExpressions.Regex.IsMatch(MaxPeerAliveTime, @"^\d+$")) ? int.Parse(MaxPeerAliveTime) : 0,
                LocalPeerId = ((LocalPeerId != "") && System.Text.RegularExpressions.Regex.IsMatch(LocalPeerId, @"^\d+$")) ? int.Parse(LocalPeerId) : 0,
                LocalRadioId = ((LocalRadioId != "") && System.Text.RegularExpressions.Regex.IsMatch(LocalRadioId, @"^\d+$")) ? int.Parse(LocalRadioId) : 0,
                Dongle = null == Dongle ? new DongleSetting() { Com = 1 } : Dongle.GetDongleSetting()
            };
        }

    }

    public class Setting
    {
        public BaseSetting Base { set; get; }
        public RadioSetting Radio{set; get;}
        public WireLanSetting WireLan{set; get;}
    }
    public  class SettingStr
    {
        public BaseSettingStr Base;
        public RadioSetting Radio { set; get; }
        public WireLanSetting WireLan { set; get; }

        public Setting GetSetting()
        {
            return new Setting()
            {
                Base = Base.GetBaseSetting(),
                Radio = Radio,
                WireLan = WireLan
            };
        }
    }


    class SettingMgr
    {
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
            //CallRpc(RequestType.setBase,setting.Base, new ParseResult(ParseStatus));

            CallRpc(RequestType.setBaseSetting, setting.Base.GetBaseSettingStr(), new ParseResult(ParseStatus));
            CallRpc(RequestType.setRadioSetting, setting.Radio.GetRadioSettingStr(), new ParseResult(ParseStatus));
            CallRpc(RequestType.setRepeaterSetting, setting.WireLan.GetWireLanSettingStr(), new ParseResult(ParseStatus));
        }

        public Setting Get()
        {
            Setting setting = new Setting();
            setting.Base = (BaseSetting)CallRpc(RequestType.getBaseSetting, new ParseResult(ParseBase));
            setting.Radio = (RadioSetting)CallRpc(RequestType.getRadioSetting, new ParseResult(ParseRadio));
            setting.WireLan = (WireLanSetting)CallRpc(RequestType.getRepeaterSetting, new ParseResult(ParseWireLan));

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
            return JsonConvert.DeserializeObject<BaseSettingStr>(JsonConvert.SerializeObject(res.contents)).GetBaseSetting();
        }
        

        private object ParseRadio(TcpResponse res)
        {
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<RadioSettingStr>(JsonConvert.SerializeObject(res.contents)).GetRadioSetting();
        }

        private object ParseWireLan(TcpResponse res)
        {
            if ((null == res) || (null == res.contents)) return null;
            return JsonConvert.DeserializeObject<WireLanSettingStr>(JsonConvert.SerializeObject(res.contents)).GetWireLanSetting();
        }


    }
}
