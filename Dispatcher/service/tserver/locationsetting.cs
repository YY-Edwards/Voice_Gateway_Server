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
   public class CLocationSetting:CConfiguration
   {
        public bool IsEnable{get; set;}
        public double Interval{get; set;}
        public bool IsEnableGpsC{get; set;}
        public NetAddress GpsC{get; set;}

        public CLocationSetting()
            : base(
            SettingType.Location,
            RequestOpcode.setLocationSetting,
            RequestOpcode.getLocationSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                CLocationSetting tserver = JsonConvert.DeserializeObject<CLocationSetting>(json);
                IsEnable = tserver.IsEnable;
                Interval = tserver.Interval;
                IsEnableGpsC = tserver.IsEnableGpsC;
                GpsC =tserver.GpsC ??  new NetAddress();

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
            Interval = 60;
            IsEnableGpsC = false;
            GpsC = new NetAddress();
       }
    }
}
