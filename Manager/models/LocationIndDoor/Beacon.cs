using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;



namespace Manager.Models
{    
    public class Beacon : RElement,ILocationable
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
         public int Vailid { set; get; }

         [JsonProperty(PropertyName = "pointx")]
         public string Pointx { set; get; }

         [JsonProperty(PropertyName = "pointy")]
         public string Pointy { set; get; }

         [JsonIgnore]
         public bool IsValid
         {
             get { return !(Vailid == 0); }
             set
             {
                 Vailid = value ? 1:0;
             }
         }

        [JsonProperty(PropertyName = "area")]
        public long Area { set; get; }

        [JsonIgnore]
        public double X
        {
            get
            {
                try
                {
                    return double.Parse(Pointx);
                }
                catch
                {
                    return 0.5;
                }
            }
            set
            {
                Pointx = value.ToString();
            }
        }

        [JsonIgnore]
        public double Y
        {
            get
            {
                try
                {
                    return double.Parse(Pointy);
                }
                catch
                {
                    return 0.5;
                }
            }
            set
            {
                Pointy = value.ToString();
            }
        }

        [JsonIgnore]
        public string Info { get { return string.Format("{0}({1},{2})", Name, Major, Minor); } set { } }
     
        public Beacon()
        {
            Name = string.Empty;            
            X = 0.5;
            Y = 0.5;
            IsValid = false;
            Area = 0;

            UUID = "50DCB6F6915A4142A6FEFDA7B4418609"; 
            Major = 0;
            Minor = 0;
            TimeStamp = 251;
        }

        public double GetLeft()
        {
            return X;
        }

        public double GetTop()
        {
            return Y;
        }
    }
    
}
