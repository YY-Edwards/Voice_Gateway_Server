
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Manager.Models
{
    public class VehicleStation : Configuration
    {
        public enum ModeType_t
        {
            General = 0,
            Cluster = 1,
        }

        public class VehicleStationNet
        {
            public int ID { get; set; }
            public string Host { get; set; }
            public int MessagePort { get; set; }
            public int ArsPort { get; set; }
            public int GpsPort { get; set; }
            public int XnlPort { get; set; }
            public int Mode { get; set; }

            [JsonIgnore]
            public ModeType_t WorkMode
            {
                get
                {
                    try
                    {
                        return (ModeType_t)Mode;
                    }
                    catch
                    {
                        return ModeType_t.General;
                    }
                }
                set
                {
                    Mode = (int)value;
                }
            }

            public VehicleStationNet()
            {
                ID = 0;
                Host = "192.168.10.2";
                MessagePort = 4007;
                ArsPort = 4005;
                GpsPort = 4001;
                XnlPort = 8002;
                WorkMode = ModeType_t.General;
            }
        }

        public bool IsEnable { get; set; }
        public int TomeoutSeconds { get; set; }

        public NetAddress Svr { get; set; }
        public VehicleStationNet Ride { get; set; }

        public int CAI { get; set; }
        public int GroupCAI { get; set; }
        public int LocationType { get; set; }

        [JsonIgnore]
        public LocationQueryType_t LocationQueryType
        {
            get
            {
                try
                {
                    return (LocationQueryType_t)LocationType;
                }
                catch
                {
                    return LocationQueryType_t.General;
                }
            }
            set
            {
                LocationType = (int)value;
            }
        }

        public VehicleStation()
        {
            IsEnable = false;
            TomeoutSeconds = 60;

            Svr = new NetAddress("127.0.0.1", 9001);
            Ride = new VehicleStationNet();

            CAI = 12;
            GroupCAI = 225;
            LocationQueryType = LocationQueryType_t.General;
        }
    }
}