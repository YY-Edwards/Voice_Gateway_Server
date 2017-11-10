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
    public class CMnisSetting : CConfiguration
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

       public CMnisSetting()
            : base(
            SettingType.Mnis,
            RequestOpcode.setMnisSetting,
            RequestOpcode.getMnisSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                CMnisSetting tserver = JsonConvert.DeserializeObject<CMnisSetting>(json);
                IsEnable = tserver.IsEnable;
                TomeoutSeconds = tserver.TomeoutSeconds;
                ID =tserver.ID;

                Host = tserver.Host;
                MessagePort =tserver .MessagePort;
                ArsPort =tserver .ArsPort;
                GpsPort =tserver .GpsPort;
                XnlPort =tserver .XnlPort;

                CAI = tserver.CAI;
                GroupCAI = tserver.GroupCAI;
                LocationQueryType = tserver.LocationQueryType;

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
