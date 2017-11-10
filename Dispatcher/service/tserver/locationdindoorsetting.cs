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
       public double Interval { set; get; }
       public int iBeaconNumber { set; get; }
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
                Interval = tserver.Interval;
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

       private void InitializeValue()
       {
           IsEnable = false;
           Interval = 30;
           iBeaconNumber = 5;
           IsEmergency = false;
       }
    }
}
