using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;
using System.ComponentModel;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Manager.Models
{
   public class Register
   {
       public event Action<object, bool, DeviceInfor> ReceivedDeviceInfo;
       public event Action<object, bool, License_t> ReceivedRegisterStatus;
       public event Action<object> Timeout;

        private TServer _TServer{get{return TServer.Instance();}}

        public Register()
        {           
            _TServer.Timeout += new Action<object, Server.Request>(OnTServerTimeout);
        }

        private void OnTServerTimeout(object sender, Server.Request request)
        {
            if (Timeout != null) Timeout(this);
        }


        public DeviceInfor QueryDevice()
        {
            if (_TServer == null) return null;

            Server.Response reponse = _TServer.SendRequest<Server.Response>(RequestOpcode.queryDevice, RequestType.radio);
            if (reponse != null)
            {
                try
                {
                    string contents = JsonConvert.SerializeObject(reponse.contents);
                    DeviceInfor deviceInfo = JsonConvert.DeserializeObject<DeviceInfor>(contents);

                    if (ReceivedDeviceInfo != null) ReceivedDeviceInfo(this, reponse.status.ToLower() == "success", deviceInfo);

                    return deviceInfo;
                }
                catch
                {

                }
            }

            return null;
        }

        public License_t QueryLicense()
        {
            if (_TServer == null) return null;

            Server.Response reponse = _TServer.SendRequest<Server.Response>(RequestOpcode.queryLicense, RequestType.radio);
            if(reponse != null  )
            {
                try
                {
                    string contents = JsonConvert.SerializeObject(reponse.contents);
                    License_t license = JsonConvert.DeserializeObject<License_t>(contents);

                    if (ReceivedRegisterStatus != null) ReceivedRegisterStatus(this, reponse.status.ToLower() == "success", license);

                    return license;
                }
                catch
                {

                }
            }
            
            return null;
        }

        public bool SetLicense(string key)
        {
            if (_TServer == null || key == null) return false;

            string[] licArray = key.Split(new char[2] { ' ', '-' });
            if (licArray.Length <= 2 || licArray[2] == null)
            {
                return false;
            }

            Dictionary<string, string> param = new Dictionary<string, string>();
            param.Add("license", licArray[2]);


            Server.Response reponse = _TServer.SendRequest<Server.Response>(RequestOpcode.registerLicense, RequestType.radio, param);

            if(reponse != null)
            {
                try
                {
                    string contents = JsonConvert.SerializeObject(reponse.contents);
                    License_t license = JsonConvert.DeserializeObject<License_t>(contents);

                    if (ReceivedRegisterStatus != null) ReceivedRegisterStatus(this, reponse.status.ToLower() == "success", license);

                    return reponse.status.ToLower() == "success";
                }
                catch
                {

                }
            }

            return false;
        }  



       public enum Device_t
       {
           VehicleStation = 1,
           Repeater = 2,
           Portable = 3,
           PC = 4,
       }

       public class DeviceInfor
       {
           public int DeviceType;
           public string DeviceMode;
           public string DeviceSerial;


           [JsonIgnore]
           public Device_t RegisterDeviceType
           {
               get
               {
                   try
                   {
                       return (Device_t)DeviceType;
                   }
                   catch
                   {
                       return Device_t.VehicleStation;
                   }
               }
               set
               {
                   DeviceType = (int)value;
               }
           }
       }


       public class License_t
       {
           public string Time;
           public int IsEver;
           public string Expiration;


           [JsonIgnore]
           public DateTime RegisterTime
           {
               get
               {
                   try
                   {
                       return DateTime.ParseExact(Time, "yyyyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
                   }
                   catch
                   {
                       return default(DateTime);
                   }                     
               }
               set
               {
                   Time = value.ToString("yyyyMMddHHmmss");
               }
           }
           [JsonIgnore]
           public DateTime ExpirationTime
           {
               get
               {
                   try
                   {
                       return DateTime.ParseExact(Expiration, "yyyyMMdd", System.Globalization.CultureInfo.CurrentCulture);
                   }
                   catch
                   {
                       return default(DateTime);
                   }
               }
               set
               {
                   Expiration = value.ToString("yyyyMMddHHmmss");
               }
           }
       }   
    }
}
