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
        public enum WireLanType
        {
            IPSC = 0,
            CPC = 1,
            LCP = 2,
        };

        public class DongleSetting
        {
            public int Com;

            public DongleSetting()
            {
                Com = 1;
            }
        }

        public bool IsEnable { get; set; }
        public int TomeoutSeconds { get; set; }
        public int Type { get; set; }

        public NetAddress Svr { get; set; }
        public NetAddress Master { get; set; }

        public int LocalPeerId { get; set; }
        public int LocalRadioId { get; set; }

        public int DefaultGroupId { get; set; }
        public int DefaultChannel { get; set; }
        public int MinHungTime { get; set; }

        public int MaxSiteAliveTime { get; set; }
        public int MaxPeerAliveTime;

        public DongleSetting Dongle { get; set; }

        public string AudioPath { get; set; }

        [JsonIgnore]
        public WireLanType WorkMode
        {
            get
            {
                try
                {
                    return (WireLanType)Type;
                }
                catch
                {
                    return WireLanType.IPSC;
                }
            }
            set
            {
                Type = (int)value;
            }
        }

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
                CRepeaterSetting tserver = JsonConvert.DeserializeObject<CRepeaterSetting>(json);

                IsEnable = tserver.IsEnable;
                TomeoutSeconds = tserver.TomeoutSeconds;
                Type = tserver.Type;
                Svr = tserver.Svr ?? new NetAddress() { Ip = "127.0.0.1", Port = 9002 };
                Master = tserver.Master ?? new NetAddress() { Ip = "192.168.2.2", Port = 50000 };
                
                DefaultGroupId = tserver.DefaultGroupId;
                DefaultChannel = tserver.DefaultChannel;
                MinHungTime = tserver.MinHungTime;

                MaxSiteAliveTime = tserver.MaxSiteAliveTime;
                MaxPeerAliveTime = tserver.MaxPeerAliveTime;

                LocalPeerId = tserver.LocalPeerId;
                LocalRadioId = tserver.LocalRadioId;

                Dongle = tserver.Dongle ?? new DongleSetting() { Com = 1 };

                AudioPath = tserver.AudioPath;

                return this;
            }
            catch
            {
                InitializeValue();
                return this;
            }

        }

        private void InitializeValue()
        {
            IsEnable = false;
            TomeoutSeconds = 60;
            WorkMode = WireLanType.IPSC;
            Svr = new NetAddress();
            Master = new NetAddress();

            LocalPeerId = 1;
            LocalRadioId = 2;

            DefaultGroupId = 1;
            DefaultChannel = 1;

            Dongle = new DongleSetting();

            AudioPath = "";
        }
    }
}
