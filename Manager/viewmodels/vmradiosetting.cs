using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;

namespace Manager
{
    public class CVMRadioSetting : CVMManager, INotifyPropertyChanged
    {
        public CVMRadioSetting()
        {
            if (m_Radio == null)
            {
                m_Radio = new CRadioSetting();
                m_Radio.OnConfigurationChanged += OnConfiguratuinChanged;
            }
          
        }

        #region
        public ICommand EnableCheck { get { return new CDelegateCommand(OnEnableCheck); } }

        #endregion


        //radiosetting
        private CRadioSetting m_Radio;

        public bool IsEnable
        {
            get { return m_Radio.IsEnable; }
            set{
                m_Radio.IsEnable = value;m_Radio.NeedSave(); }
        }
        public bool IsRideMnis
        {
            get { return !m_Radio.IsOnlyRide; }
            set
            {
                if (m_Radio.IsOnlyRide != value)
                {
                    m_Radio.IsOnlyRide = value;
                    m_Radio.NeedSave();
                }
            }
        }
        public bool IsOnlyRide
        {
            get { return m_Radio.IsOnlyRide; }
            set
            {              
                if (m_Radio.IsOnlyRide != value)
                {
                    m_Radio.IsOnlyRide = value;
                    m_Radio.NeedSave();
                }
            }
        }
        public string Svr_Ip { get { return m_Radio.Svr.Ip; } set { m_Radio.Svr.Ip = value; m_Radio.NeedSave(); } }
        public int Svr_port { get { return m_Radio.Svr.Port; } set { m_Radio.Svr.Port = value; m_Radio.NeedSave(); } }
        public string Ride_Host { get { return m_Radio.Ride.Host; } set { m_Radio.Ride.Host = value; m_Radio.NeedSave(); } }
        public int Ride_MessagePort { get { return m_Radio.Ride.MessagePort; } set { m_Radio.Ride.MessagePort = value; m_Radio.NeedSave(); } }
        public int Ride_ArsPort { get { return m_Radio.Ride.ArsPort; } set { m_Radio.Ride.ArsPort = value; m_Radio.NeedSave(); } }
        public int Ride_GpsPort { get { return m_Radio.Ride.GpsPort; } set { m_Radio.Ride.GpsPort = value; m_Radio.NeedSave(); } }
        public int Ride_XnlPort { get { return m_Radio.Ride.XnlPort; } set { m_Radio.Ride.XnlPort = value; m_Radio.NeedSave(); } }

        public string Mnis_Host { get { return m_Radio.Mnis.Host; } set { m_Radio.Mnis.Host = value; m_Radio.NeedSave(); } }
        public int Mnis_MessagePort { get { return m_Radio.Mnis.MessagePort; } set { m_Radio.Mnis.MessagePort = value; m_Radio.NeedSave(); } }
        public int Mnis_ArsPort { get { return m_Radio.Mnis.ArsPort; } set { m_Radio.Mnis.ArsPort = value; m_Radio.NeedSave(); } }
        public int Mnis_GpsPort { get { return m_Radio.Mnis.GpsPort; } set { m_Radio.Mnis.GpsPort = value; m_Radio.NeedSave(); } }
        public int Mnis_XnlPort { get { return m_Radio.Mnis.XnlPort; } set { m_Radio.Mnis.XnlPort = value; m_Radio.NeedSave(); } }

        public string GpsC_Ip { get { return m_Radio.GpsC.Ip; } set { m_Radio.GpsC.Ip = value; m_Radio.NeedSave(); } }
        public int GpsC_port { get { return m_Radio.GpsC.Port; } set { m_Radio.GpsC.Port = value; m_Radio.NeedSave(); } }


        private void OnConfiguratuinChanged(SettingType type, object config)
        {
            if (m_Radio == null || type != SettingType.Radio) return;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsEnable"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsRideMnis"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsOnlyRide"));

                PropertyChanged(this, new PropertyChangedEventArgs("Svr_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("Svr_port"));


                PropertyChanged(this, new PropertyChangedEventArgs("Ride_Host"));
                PropertyChanged(this, new PropertyChangedEventArgs("Ride_MessagePort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Ride_ArsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Ride_GpsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Ride_XnlPort"));


                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_Host"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_MessagePort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_ArsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_GpsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_XnlPort"));

                PropertyChanged(this, new PropertyChangedEventArgs("GpsC_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("GpsC_port"));
            }
        }

        private void OnEnableCheck(object parameter)
        {
            if(parameter == null || !(parameter is CVMRepeaterSetting))return;
            CVMRepeaterSetting cvmrepeater = parameter as CVMRepeaterSetting;
            if (m_Radio.IsEnable)
            {
                cvmrepeater.SetEnable(false);
            }

        }
        public void SetEnable(bool enable)
        {
            m_Radio.IsEnable = enable;
            PropertyChanged(this, new PropertyChangedEventArgs("IsEnable"));
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;

        public void Set()
        {
            m_Radio.Set();
        }

        public void Get()
        {
            m_Radio.Get();
        }
    }
}
