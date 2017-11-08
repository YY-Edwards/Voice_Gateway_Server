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
       public event Action<object, bool, License_t> ReceivedLicenseMessage;
        public event Action<object,bool, License_t> ReceivedRegisterReply;
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

                    if (ReceivedLicenseMessage != null) ReceivedLicenseMessage(this, reponse.status.ToLower() == "success",  license);

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

                    if (ReceivedRegisterReply != null) ReceivedRegisterReply(this, reponse.status.ToLower() == "success", license);

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
       public class License_t
       {
           public int DeviceType;
           public string RadioMode;
           public string RadioSerial;
           public string RepeaterMode;
           public string RepeaterSerial;
           public string Time;
           public int IsEver;
           public string Expiration;

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

       //public event Action<RequestOpcode, bool> RegisterStatusChanged;
       //public CRegister()
       //    : base(
       //    SettingType.Register)
       //{

       //}

       //public override void CustomParse(RequestOpcode opcode,bool success, string reply)
       //{          
       //    QueryResponse response =  null;
       //    try
       //    {
       //        response = JsonConvert.DeserializeObject<QueryResponse>(reply);
       //    }
       //    catch
       //    {
       //    }

       //    OnResponse(opcode,success, response);        
       //}


       //private void OnResponse(RequestOpcode opcode,bool success, QueryResponse response)
       //{
       //    if (RegisterStatusChanged != null) RegisterStatusChanged(opcode, success);           
       //}
      

       //public void Query()
       //{
       //    Request(RequestOpcode.queryLicense, null);
       //}

       //public bool Register(string key)
       //{
       //    string[] licArray = key.Split(new char[2] { ' ', '-' });
       //    if (licArray.Length <= 2 || licArray[2] == null)
       //    {             
       //        return false;
       //    }

       //    Dictionary<string, string> pa = new Dictionary<string, string>();
       //    pa.Add("license", licArray[2]);

       //    Request(RequestOpcode.registerLicense, Build(pa));
       //    return true;
       //}
    }
}
