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
   public class CRegister:CConfiguration
   {
       //public DeviceType_t DeviceType;
       //public string RadioMode;
       //public string RadioSerial;
       //public string RepeaterMode;
       //public string RepeaterSerial;

       [JsonProperty(PropertyName = "Time")]
       public string time;
       public int IsEver;
       [JsonProperty(PropertyName = "Expiration")]
       public string expiration;


       [JsonIgnore]
       public bool IsConnectedDevice;

       [JsonIgnore]
       public bool IsRegistered;

       [JsonIgnore]
       public DateTime Time
       {
           get
           {
               return DateTime.ParseExact(time, "yyyyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
           }
       }
       [JsonIgnore]
       public DateTime Expiration
       {
           get
           {
               return DateTime.ParseExact(expiration, "yyyyMMdd", System.Globalization.CultureInfo.CurrentCulture);
           }
       }



        public delegate void OnRegiserHandle(bool success, CRegister res);
        public event OnRegiserHandle OnRegister;
        public event OnRegiserHandle OnQuery;

        public CRegister()
            : base(
            SettingType.Base,
            RequestOpcode.registerLicense,
            RequestOpcode.queryLicense)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                CRegister tserver = JsonConvert.DeserializeObject<CRegister>(json);

                //DeviceType = tserver.DeviceType;
                //RadioMode = tserver.RadioMode;
                //RadioSerial = tserver.RadioSerial;
                //RepeaterMode = tserver.RepeaterMode;
                //RepeaterSerial = tserver.RepeaterSerial;
     
                time = tserver.time;
                IsEver = tserver.IsEver;
                expiration = tserver.expiration;
                return this;
            }
            catch
            {
                InitializeValue();
                return this;
            }

        }

       public override void CustomParse(RequestOpcode opcode,bool success, string reply)
       {         
           if (success) Parse(reply);

           if(opcode == RequestOpcode.registerLicense)
           {
               IsRegistered = success;              
               if (OnRegister != null) OnRegister(success, this);
           }
           else if(opcode == RequestOpcode.queryLicense)
           {
               IsConnectedDevice = success;              
               if (OnQuery != null) OnQuery(success, this);
           }
       }

       private void InitializeValue()
       {
           //DeviceType = DeviceType_t.Radio;
           //RadioMode = string.Empty;
           //RadioSerial = string.Empty;
           //RepeaterMode = string.Empty;
           //RepeaterSerial = string.Empty;

           time = DateTime.Now.ToString("yyyyMMddHHmmss");
           IsEver = 0;
           expiration = time;
       }


       public void SendKey(string ley)
       {
           string[] licArray = ley.Split(new char[2] { ' ', '-' });
           if (licArray.Length <= 2 || licArray[2] == null)
           {
               IsRegistered = false;
               if (OnRegister != null) OnRegister(false, this);
               return;
           }

           Dictionary<string, string> pa = new Dictionary<string, string>();
           pa.Add("license", licArray[2]);

           Request(RequestOpcode.registerLicense, Build(pa));
       }

       public enum DeviceType_t
       {
           Radio = 1,
           Repeater = 2,
           Portable = 3,
           PC = 4,
       } 
    }
}
