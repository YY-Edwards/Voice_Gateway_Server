using Newtonsoft.Json;

namespace Manager.Models
{
    public class Repeater : Configuration
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

        public Repeater()
        {
            IsEnable = false;
            TomeoutSeconds = 60;
            WorkMode = WireLanType.IPSC;
            Svr = new NetAddress("127.0.0.1", 9002);
            Master = new NetAddress("192.168.2.2", 50000);

            LocalPeerId = 1;
            LocalRadioId = 2;

            DefaultGroupId = 1;
            DefaultChannel = 1;

            Dongle = new DongleSetting();

            AudioPath = App.AudioDirectory;
        }
    }
}