using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class CConfiguration
    {
        private bool m_NeedSave = false;
        private bool m_IsUpdated = false;
        private SettingType m_Type;
        private RequestOpcode m_GetOpcode;
        private RequestOpcode m_SetOpcode;
        
        [JsonIgnore]
        public object m_Object;

        public delegate void ConfigurationChangedHandle(SettingType type, object config);
        public event ConfigurationChangedHandle OnConfigurationChanged;

        public event EventHandler Timeout;

        public CConfiguration(SettingType type, RequestOpcode set, RequestOpcode get)
        {
            m_Type = type;
            m_SetOpcode = set;
            m_GetOpcode = get;

            CTServer.Instance().Timeout += new EventHandler(OnTimeout);
        }

        private void OnTimeout(object sender, EventArgs e)
        {
            if (Timeout != null) Timeout(sender, e);
        }

        public void Request(RequestOpcode opcode, object parameter, bool isget = false)
        {
            if (!CTServer.Instance().IsInitialized) return;
            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    string[] reply = CTServer.Instance().Request(opcode, RequestType.radio, parameter);
                    m_NeedSave = false;

                    if (reply != null && reply.Length >=2)
                    {
                        if (isget && reply[0] == "success")
                        {
                            m_Object = Parse(reply[1]);
                            if (OnConfigurationChanged != null) OnConfigurationChanged(m_Type, m_Object);
                            m_IsUpdated = true;
                        }

                        CustomParse(opcode, reply[0] == "success", reply[1]);
                    }
                }
                catch
                {

                }
            })).Start();
        }

        public virtual object Parse(string json)
        {
            return null;
        }


        public virtual void CustomParse(RequestOpcode opcode,bool success, string reply)
        {

        }

        public virtual object Build(object obj)
        {
            return obj;
        }
        public virtual void Get(bool islocal = true)
        {
            if(islocal && m_IsUpdated)
            {
                m_NeedSave = false;
                if (OnConfigurationChanged != null) OnConfigurationChanged(m_Type, m_Object);
            }
            else
            {
                Request(m_GetOpcode, null, true);
            }
           
        }

        public virtual void NeedSave()
        {
            m_NeedSave = true;
        }

        public virtual void Set()
        {
            if (!m_NeedSave) return;
            m_Object = this;
            m_IsUpdated = true;
            Request(m_SetOpcode,Build(this));
        }
    }


    public class NetAddress
    {
        public string Ip { get; set; }
        public int Port { get; set; }
    };
    public enum SettingType
    {
        Base,
        Radio,
        Repeater,
        QueryRegister,
        Register,
        LocationInDoor,
    };

    //public enum Device
    //{
    //    Radio = 1,
    //    Repeater = 2,
    //    Portable = 3,
    //    PC = 4,
    //}

    //public class SettingResponse
    //{
    //    public string status { set; get; }
    //    public string statusText { set; get; }
    //    public int errCode { set; get; }
    //    public Int64 callId { set; get; }
    //    public object contents { set; get; }

    //    public bool IsSuccess
    //    {
    //        get
    //        {
    //            if ("success" == status) return true;
    //            else return false;
    //        }
    //    }
    //}
    //public class LisenceRes
    //{
    //    public Device DeviceType;
    //    public string RadioMode;
    //    public string RadioSerial;
    //    public string RepeaterMode;
    //    public string RepeaterSerial;
    //    [JsonProperty(PropertyName = "Time")]
    //    public string time;
    //    public int IsEver;
    //    [JsonProperty(PropertyName = "Expiration")]
    //    public string expiration;
    //    [JsonIgnore]
    //    public bool IsOK;
    //    [JsonIgnore]
    //    public DateTime Time
    //    {
    //        get
    //        {
    //            return DateTime.ParseExact(time, "yyyyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
    //        }
    //    }
    //    [JsonIgnore]
    //    public DateTime Expiration
    //    {
    //        get
    //        {
    //            return DateTime.ParseExact(expiration, "yyyyMMdd", System.Globalization.CultureInfo.CurrentCulture);
    //        }
    //    }
    //}
}
