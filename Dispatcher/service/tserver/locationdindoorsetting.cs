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
   public class CLocationInDoorSetting:CConfiguration
   {
       public bool IsEnable { get; set; }

       public int CAI { set; get; }

       public string IP { set; get; }

       public int ID { set; get; }

       public int Port { set; get; }

       public int Interval { set; get; }


       public int iBeaconNumber { set; get; }


       public bool IsTriggered { set; get; }


       public QueryType QueryType { set; get; }

       public bool IsEmergency { set; get; }


        public CLocationInDoorSetting()
            : base(
            SettingType.LocationInDoor,
            RequestOpcode.setLocationInDoorSetting,
            RequestOpcode.getLocationInDoorSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                CLocationInDoorSetting tserver = JsonConvert.DeserializeObject<CLocationInDoorSetting>(json);
                IsEnable = tserver.IsEnable;
                CAI  = tserver.CAI;
                IP = tserver.IP;
                ID = tserver.ID;
                Port = tserver.Port;
                Interval = tserver.Interval;
                IsTriggered = tserver.IsTriggered;
                QueryType = tserver.QueryType;
                iBeaconNumber = tserver.iBeaconNumber;
                IsEmergency = tserver.IsEmergency;
                return this;
            }
            catch
            {
                InitializeValue();
                return this;
            }

        }

        private int m_LocationPort = 4001;
        private int m_CAI = 12;
        private string m_SrcIp = "192.168.10.2";

       private void InitializeValue()
       {
           IsEnable = true;
           CAI = 12;
           IP = "192.168.10.2";
           ID =1;
           Port = 4001;

           Interval = 30;
           IsTriggered = true;
           QueryType = QueryType.bcon_maj_min_time;
           iBeaconNumber = 5;
           IsEmergency = false;
       }
    }

   public enum QueryType
   {
       bcon_maj_min_time = 0,
       bcon_maj_min_txpwr_rssi_time = 1,
       bcon_uuid_maj_min_txpwr_rssi_time = 2,
       last_beacon = 3,
       set_listen_all_uuid = 4
   }


   public enum ReportType
   {
       empty = 0,
       uuid = 1,
       maj_min_time = 2,
       maj_min_txpwr_rssi_time = 3,
       bcon_uuid_maj_min_txpwr_rssi_time = 4,
   }
}
