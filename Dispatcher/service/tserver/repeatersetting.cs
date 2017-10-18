using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.ComponentModel;

namespace Dispatcher.Service
{
    public class CRepeaterSetting : CConfiguration
    {
        public bool IsEnable;
        public WireLanType Type;
        public NetAddress Svr;
        public NetAddress Master;
        public CMNISNet Mnis;
        public int DefaultGroupId;
        public int DefaultChannel;
        public int MinHungTime;

        public int MaxSiteAliveTime;
        public int MaxPeerAliveTime;

        public int LocalPeerId;
        public int LocalRadioId;

        public DongleSetting Dongle;

        public CRepeaterSetting()
            : base(
            SettingType.Repeater,
            RequestOpcode.setRepeaterSetting,
            RequestOpcode.getRepeaterSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                WireLanSetting tserver_tmp = JsonConvert.DeserializeObject<WireLanSetting>(json);
                CRepeaterSetting tserver = WireLanSetting.Prase(tserver_tmp);

                IsEnable = tserver.IsEnable;
                Type = tserver.Type;
                Svr = tserver.Svr ?? new NetAddress() { Ip = "127.0.0.1", Port = 9002 };
                Master = tserver.Master ?? new NetAddress() { Ip = "192.168.2.2", Port = 50000 };
                Mnis = tserver.Mnis ?? new CMNISNet()
                {
                    Host = "192.168.11.2",
                    ID = 50001,
                    MessagePort = 4007,
                    ArsPort = 4005,
                    GpsPort = 4001,
                    XnlPort = 8002,
                };
                DefaultGroupId = tserver.DefaultGroupId;
                DefaultChannel = tserver.DefaultChannel;
                MinHungTime = tserver.MinHungTime;

                MaxSiteAliveTime = tserver.MaxSiteAliveTime;
                MaxPeerAliveTime = tserver.MaxPeerAliveTime;

                LocalPeerId = tserver.LocalPeerId;
                LocalRadioId = tserver.LocalRadioId;

                Dongle = tserver.Dongle ?? new DongleSetting() { Com = 1 };

                return this;
            }
            catch
            {
                InitializeValue();
                return this;
            }

        }

        public override object Build(object obj)
        {
            return WireLanSetting.Build(obj as CRepeaterSetting);
        }
        private void InitializeValue()
        {
            IsEnable = false;
            Type = WireLanType.IPSC;
            Svr = new NetAddress() { Ip= "127.0.0.1",Port = 9002};
            Master = new NetAddress() { Ip = "192.168.2.2", Port = 50000 };
            Mnis = new CMNISNet()
            {
                Host = "192.168.11.2",
                ID = 50001,
                MessagePort = 4007,
                ArsPort = 4005,
                GpsPort = 4001,
                XnlPort = 8002,
            };

            DefaultGroupId = 1;
            DefaultChannel = 1;
            MinHungTime = 0;

            MaxSiteAliveTime = 0;
            MaxPeerAliveTime = 0;

            LocalPeerId = 1;
            LocalRadioId = 2;

            Dongle = new DongleSetting() { Com = 1};
        }
    }

    //version 1.0

    public enum WireLanType
    {
        IPSC = 0,
        CPC = 1,
        LCP = 2,
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
        public NetAddress Mnis;
        public int MnisId;
        public int DefaultGroupId;
        public int DefaultChannel;
        public int MinHungTime;

        public int MaxSiteAliveTime;
        public int MaxPeerAliveTime;

        public int LocalPeerId;
        public int LocalRadioId;

        public DongleSetting Dongle;

        public static WireLanSetting Build(CRepeaterSetting setting)
        {
            try
            {
                WireLanSetting res = new WireLanSetting();


                res.IsEnable = setting.IsEnable;
                res.Type = setting.Type;
                res.Svr = setting.Svr;
                res.Master = setting.Master;

                res.Mnis = new NetAddress() { Ip = setting.Mnis.Host };
                res.MnisId = setting.Mnis.ID;

                res.DefaultGroupId = setting.DefaultGroupId;
                res.DefaultChannel = setting.DefaultChannel;
                res.MinHungTime = setting.MinHungTime;

                res.MaxSiteAliveTime = setting.MaxSiteAliveTime;
                res.MaxPeerAliveTime = setting.MaxPeerAliveTime;

                res.LocalPeerId = setting.LocalPeerId;
                res.LocalRadioId = setting.LocalRadioId;

                res.Dongle = setting.Dongle;
                return res;
            }
            catch
            {
                return null;
            }

        }


        public static CRepeaterSetting Prase(WireLanSetting setting)
        {
            try
            {
                CRepeaterSetting res = new CRepeaterSetting();

                res.IsEnable = setting.IsEnable;
                res.Type = setting.Type;
                res.Svr = setting.Svr;
                res.Master = setting.Master;
                res.Mnis = new CMNISNet()
                {
                    Host = setting.Mnis.Ip,
                    ID = setting.MnisId,
                };
                res.DefaultGroupId = setting.DefaultGroupId;
                res.DefaultChannel = setting.DefaultChannel;
                res.MinHungTime = setting.MinHungTime;

                res.MaxSiteAliveTime = setting.MaxSiteAliveTime;
                res.MaxPeerAliveTime = setting.MaxPeerAliveTime;

                res.LocalPeerId = setting.LocalPeerId;
                res.LocalRadioId = setting.LocalRadioId;

                res.Dongle = setting.Dongle;
                return res;
            }
            catch
            {
                return null;
            }

        }

    }

}
