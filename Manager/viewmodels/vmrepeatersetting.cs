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
    public class CVMRepeaterSetting : CVMManager, INotifyPropertyChanged
    {
        public CVMRepeaterSetting()
        {
            if (m_Repeater == null)
            {
                m_Repeater = new CRepeaterSetting();
                m_Repeater.OnConfigurationChanged += OnConfiguratuinChanged;
            }
          
        }

        #region
        public ICommand EnableCheck { get { return new CDelegateCommand(OnEnableCheck); } }

        #endregion


        //radiosetting
        private CRepeaterSetting m_Repeater;

        public bool IsEnable
        {
            get { 
                return m_Repeater.IsEnable; }
            set { m_Repeater.IsEnable = value; m_Repeater.NeedSave(); }
        }
      
        public bool CPC { get { return m_Repeater.Type == WireLanType.CPC; } set { if (value) { if (m_Repeater.Type != WireLanType.CPC) { m_Repeater.Type = WireLanType.CPC; m_Repeater.NeedSave(); } } } }
        public bool IPSC { get { return m_Repeater.Type == WireLanType.IPSC; } set { if (value) { if (m_Repeater.Type != WireLanType.IPSC) { m_Repeater.Type = WireLanType.IPSC; m_Repeater.NeedSave(); } } } }
        public bool LCP { get { return m_Repeater.Type == WireLanType.LCP; } set { if (value) { if (m_Repeater.Type != WireLanType.LCP) { m_Repeater.Type = WireLanType.LCP; m_Repeater.NeedSave(); } } } }

        public string Svr_Ip { get { return m_Repeater.Svr.Ip; } set { m_Repeater.Svr.Ip = value; m_Repeater.NeedSave(); } }
        public int Svr_port { get { return m_Repeater.Svr.Port; } set { m_Repeater.Svr.Port = value; m_Repeater.NeedSave(); } }
        public string Master_Ip { get { return m_Repeater.Master.Ip; } set { m_Repeater.Master.Ip = value; m_Repeater.NeedSave(); } }
        public int Master_port { get { return m_Repeater.Master.Port; } set { m_Repeater.Master.Port = value; m_Repeater.NeedSave(); } }

        public string Mnis_Host { get { return m_Repeater.Mnis.Host; } set { m_Repeater.Mnis.Host = value; m_Repeater.NeedSave(); } }
        public int Mnis_ID { get { return m_Repeater.Mnis.ID; } set { m_Repeater.Mnis.ID = value; m_Repeater.NeedSave(); } }
        public int Mnis_MessagePort { get { return m_Repeater.Mnis.MessagePort; } set { m_Repeater.Mnis.MessagePort = value; m_Repeater.NeedSave(); } }
        public int Mnis_ArsPort { get { return m_Repeater.Mnis.ArsPort; } set { m_Repeater.Mnis.ArsPort = value; m_Repeater.NeedSave(); } }
        public int Mnis_GpsPort { get { return m_Repeater.Mnis.GpsPort; } set { m_Repeater.Mnis.GpsPort = value; m_Repeater.NeedSave(); } }
        public int Mnis_XnlPort { get { return m_Repeater.Mnis.XnlPort; } set { m_Repeater.Mnis.XnlPort = value; m_Repeater.NeedSave(); } }


        public int DefaultGroupId { get { return m_Repeater.DefaultGroupId; } set { m_Repeater.DefaultGroupId = value; m_Repeater.NeedSave(); } }
        public int DefaultChannel { get { return m_Repeater.DefaultChannel - 1; } set { m_Repeater.DefaultChannel = value + 1; m_Repeater.NeedSave(); } }
        public int MinHungTime { get { return m_Repeater.MinHungTime; } set { m_Repeater.MinHungTime = value; m_Repeater.NeedSave(); } }

        public int MaxSiteAliveTime { get { return m_Repeater.MaxSiteAliveTime; } set { m_Repeater.MaxSiteAliveTime = value; m_Repeater.NeedSave(); } }
        public int MaxPeerAliveTime { get { return m_Repeater.MaxPeerAliveTime; } set { m_Repeater.MaxPeerAliveTime = value; m_Repeater.NeedSave(); } }

        public int LocalPeerId { get { return m_Repeater.LocalPeerId; } set { m_Repeater.LocalPeerId = value; m_Repeater.NeedSave(); } }
        public int LocalRadioId { get { return m_Repeater.LocalRadioId; } set { m_Repeater.LocalRadioId = value; m_Repeater.NeedSave(); } }

        public int Dongle_Com_Index { get { return m_Repeater.Dongle.Com - 1; } set { m_Repeater.Dongle.Com = value + 1; m_Repeater.NeedSave(); } }


        private void OnConfiguratuinChanged(SettingType type, object config)
        {
            if (m_Repeater == null || type != SettingType.Repeater) return;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsEnable"));

                PropertyChanged(this, new PropertyChangedEventArgs("CPC"));
                PropertyChanged(this, new PropertyChangedEventArgs("IPSC"));
                PropertyChanged(this, new PropertyChangedEventArgs("LCP"));

                PropertyChanged(this, new PropertyChangedEventArgs("Svr_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("Svr_port"));

                PropertyChanged(this, new PropertyChangedEventArgs("Master_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("Master_port"));

                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_Host"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_ID"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_MessagePort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_ArsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_GpsPort"));
                PropertyChanged(this, new PropertyChangedEventArgs("Mnis_XnlPort"));


                PropertyChanged(this, new PropertyChangedEventArgs("DefaultGroupId"));
                PropertyChanged(this, new PropertyChangedEventArgs("DefaultChannel"));

                PropertyChanged(this, new PropertyChangedEventArgs("MinHungTime"));
                PropertyChanged(this, new PropertyChangedEventArgs("DefaultChannel")); 
                PropertyChanged(this, new PropertyChangedEventArgs("MaxPeerAliveTime"));

                PropertyChanged(this, new PropertyChangedEventArgs("LocalPeerId"));
                PropertyChanged(this, new PropertyChangedEventArgs("LocalRadioId"));

                PropertyChanged(this, new PropertyChangedEventArgs("Dongle_Com_Index"));
            }
        }

        private void OnEnableCheck(object parameter)
        {
            if (parameter == null || !(parameter is CVMRadioSetting)) return;
            CVMRadioSetting cvmrepeater = parameter as CVMRadioSetting;
            if (m_Repeater.IsEnable)
            {
                cvmrepeater.SetEnable(false);
            }

        }
        public void SetEnable(bool enable)
        {
            m_Repeater.IsEnable = enable;
            if (PropertyChanged != null)PropertyChanged(this, new PropertyChangedEventArgs("IsEnable"));
            m_Repeater.NeedSave();
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;

        public void Set()
        {
            m_Repeater.Set();
        }

        public void Get()
        {
            m_Repeater.Get();
        }
    }
}
