using Newtonsoft.Json;

namespace Manager.Models
{
    public class Mnis : Configuration
    {
        public bool IsEnable { get; set; }
        public int TomeoutSeconds { get; set; }
        public int ID { get; set; }

        public string Host { get; set; }

        public int MessagePort { get; set; }
        public int ArsPort { get; set; }
        public int GpsPort { get; set; }
        public int XnlPort { get; set; }

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

        public Mnis()
        {
            IsEnable = false;
            TomeoutSeconds = 60;
            ID = 0;

            Host = "192.168.11.2";
            MessagePort = 4007;
            ArsPort = 4005;
            GpsPort = 4001;
            XnlPort = 8002;

            CAI = 12;
            GroupCAI = 225;
            LocationQueryType = LocationQueryType_t.General;
        }
    }
}