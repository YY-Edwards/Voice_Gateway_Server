using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Dispatcher.Modules
{

    public class CBeacon : CElement
    {
        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "uuid")]
        public string UUID { set; get; }

        [JsonProperty(PropertyName = "major")]
        public int Major { set; get; }

        [JsonProperty(PropertyName = "minor")]
        public int Minor { set; get; }

        [JsonProperty(PropertyName = "tx_power")]
        public int TxPower { set; get; }

        [JsonProperty(PropertyName = "rssi")]
        public int RSSI { set; get; }

        [JsonProperty(PropertyName = "time_stamp")]
        public int TimeStamp { set; get; }


        [JsonProperty(PropertyName = "valid")]
        public int Vailid { get { return IsValid ? 1 : 0; } set { IsValid = !(value == 0); } }

        [JsonProperty(PropertyName = "pointx")]
        public string Pointx { get { return X.ToString(); } set { X = double.Parse(value); } }

        [JsonProperty(PropertyName = "pointy")]
        public string Pointy { get { return Y.ToString(); } set { Y = double.Parse(value); } }

        [JsonIgnore]
        public bool IsValid;

        [JsonProperty(PropertyName = "area")]
        public int Area { set; get; }


        [JsonIgnore]
        public double X { set; get; }

        [JsonIgnore]
        public double Y { set; get; }

        [JsonIgnore]
        public string NameStr
        {
            get
            {
                return (string.IsNullOrWhiteSpace(Name) ? "" : (Name + ":")) + "(" + Major.ToString() + "," + Minor.ToString() + ")";
            }
        }

        public CBeacon()
        {
            Name = string.Empty;
            UUID = string.Empty;
            X = 0.0;
            Y = 0.0;
            IsValid = false;
            Area = -1;
        }     
    }
}
