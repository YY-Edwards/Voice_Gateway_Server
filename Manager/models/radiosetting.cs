using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.ComponentModel;

namespace Manager
{
    public class CRadioSetting : CConfiguration
    {
        public bool IsEnable{ get; set; }
        public bool IsOnlyRide{ get; set; }
        public NetAddress Svr{ get; set; }
        public CRideNet Ride { get; set; }
        public CMNISNet Mnis { get; set; }
        public NetAddress GpsC{ get; set; }



        public CRadioSetting()
            : base(
            SettingType.Radio,
            RequestOpcode.setRadioSetting,
            RequestOpcode.getRadioSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                RadioSetting tserver_tmp = JsonConvert.DeserializeObject<RadioSetting>(json);
                CRadioSetting tserver = RadioSetting.Prase(tserver_tmp);

                IsEnable = tserver.IsEnable;
                IsOnlyRide = tserver.IsOnlyRide;
                Svr = tserver.Svr;
                Ride = tserver.Ride;
                Mnis = tserver.Mnis;
                GpsC = tserver.GpsC;
            }
            catch
            {
                InitializeValue();
                return this;
            }

            return base.Parse(json);
        }

        public override object Build(object obj)
        {
            return RadioSetting.Build(obj as CRadioSetting);
        }
        private void InitializeValue()
        {
            IsEnable = true;
            IsOnlyRide = true;

            Svr = new NetAddress()
            {
                Ip = "127.0.0.1",
                Port = 9001
            };

            Ride = new CRideNet()
            {
                Host = "192.168.10.2",
                MessagePort = 4007,
                ArsPort = 4005,
                GpsPort = 4001,
                XnlPort = 8002,
            };

            Mnis = new CMNISNet()
            {
                Host = "192.168.11.2",
                MessagePort = 4007,
                ArsPort = 4005,
                GpsPort = 4001,
                XnlPort = 8002,
            };

            GpsC = new NetAddress()
            {
                Ip = "192.168.12.2",
                Port = 50000
            };
        }
    }

    public class CRideNet
    {
        public string Host { get; set; }
        public int MessagePort { get; set; }
        public int ArsPort { get; set; }
        public int GpsPort { get; set; }
        public int XnlPort { get; set; }
    }

    public class CMNISNet
    {
        public string Host { get; set; }
        public int ID { get; set; }
        public int MessagePort { get; set; }
        public int ArsPort { get; set; }
        public int GpsPort { get; set; }
        public int XnlPort { get; set; }
    }

    //version 1.0
    public class RadioSetting
    {
        public bool IsEnable;
        public bool IsOnlyRide;
        public NetAddress Svr;
        public NetAddress Ride;
        public NetAddress Mnis;
        public NetAddress GpsC;
        public NetAddress Ars;
        public NetAddress Message;
        public NetAddress Gps;
        public NetAddress Xnl;

        public static RadioSetting Build(CRadioSetting setting)
        {
            try
            {
                RadioSetting res = new RadioSetting();
                res.IsEnable = setting.IsEnable;
                res.IsOnlyRide = setting.IsOnlyRide;
                res.Svr = setting.Svr;

                res.Ride = new NetAddress() { Ip = setting.Ride.Host };
                res.Mnis = new NetAddress() { Ip = setting.Mnis.Host };
                res.GpsC = setting.GpsC;

                res.Ars = new NetAddress() { Port = setting.Ride.ArsPort };
                res.Message = new NetAddress() { Port = setting.Ride.MessagePort };
                res.Gps = new NetAddress() { Port = setting.Ride.GpsPort };
                res.Xnl = new NetAddress() { Port = setting.Ride.XnlPort };

                return res;
            }
            catch
            {
                return null;
            }

        }


        public static CRadioSetting Prase(RadioSetting setting)
        {
            try
            {
                CRadioSetting res = new CRadioSetting();
                res.IsEnable = setting.IsEnable;
                res.IsOnlyRide = setting.IsOnlyRide;
                res.Svr = setting.Svr;

                res.Ride = new CRideNet() { Host = setting.Ride.Ip, MessagePort =  setting.Message.Port, ArsPort = setting.Ars.Port, GpsPort = setting.Gps.Port, XnlPort = setting.Xnl.Port};
                res.Mnis = new CMNISNet() { Host = setting.Mnis.Ip, MessagePort = setting.Message.Port, ArsPort = setting.Ars.Port, GpsPort = setting.Gps.Port, XnlPort = setting.Xnl.Port };
                res.GpsC = setting.GpsC;
                return res;
            }
            catch
            {
                return null;
            }

        }
    }
}
