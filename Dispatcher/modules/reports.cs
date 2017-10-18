using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;

namespace Dispatcher.Modules
{
    public class ReportCall
    {

        public string Time { set; get; }
        public int SourceID { set; get; }
        public int TargetID { set; get; }
        public int RecordType { set; get; }
        public string RecordTypeStr
        {
            get
            {
                switch (RecordType)
                {
                    case 0: return "IPSC";
                    case 1: return "CPC";
                    case 2:
                        return "LCP";
                    default: return "未知";
                }
            }
        }
        public int CallType { set; get; }
        public string CallTypeStr
        {
            get
            {
                switch (CallType)
                {
                    case 79: return "组呼";
                    case 80: return "个呼";
                    case 83:
                        return "全呼";
                    default: return "未知";
                }
            }
        }
        public int DataSize { set; get; }
        public string CallLength { get { return (((double)DataSize) / 350).ToString("0.0") + "s"; } }
    }

    public class ReportSms
    {
        [JsonProperty(PropertyName = "createdf_at")]
        public string Time { set; get; }

        [JsonProperty(PropertyName = "source")]
        public int SourceID { set; get; }

        [JsonProperty(PropertyName = "destination")]
        public int TargetID { set; get; }

        [JsonProperty(PropertyName = "message")]
        public string Content { set; get; }
    }

    public class ReportGps
    {
         [JsonProperty(PropertyName = "created_at")]
        public string Time { set; get; }
         [JsonProperty(PropertyName = "radio")]
        public int TargetID { set; get; }
         [JsonProperty(PropertyName = "latitude")]
        public string Lat { set; get; }
         [JsonProperty(PropertyName = "logitude")]
        public string Log { set; get; }
         [JsonProperty(PropertyName = "velocity")]
        public string Vel { set; get; }
         [JsonProperty(PropertyName = "altitude")]
        public string Alt { set; get; }
    }

    public class LocationInDoorLog
    {
        [JsonProperty(PropertyName = "created_at")]
        public string Time { set; get; }
        [JsonProperty(PropertyName = "source")]
        public string TargetID { set; get; }

        [JsonProperty(PropertyName = "major")]
        public string Major { set; get; }
        [JsonProperty(PropertyName = "minor")]
        public string Minor { set; get; }

        [JsonProperty(PropertyName = "timestamp")]
        public string TimeStamp { set; get; }
        [JsonProperty(PropertyName = "uuid")]
        public string UUID { set; get; }

        [JsonProperty(PropertyName = "txper")]
        public string TxPower { set; get; }
        [JsonProperty(PropertyName = "rssi")]
        public string RSSI { set; get; }


        [JsonProperty(PropertyName = "areaname")]
        public string AreaName { set; get; }
        [JsonProperty(PropertyName = "pointx")]

        public string PointX { set; get; }
        [JsonProperty(PropertyName = "pointy")]
        public string PointY { set; get; }
    }
}
