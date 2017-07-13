using System;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;

namespace Manager
{
    public class CVMConfiguration : INotifyPropertyChanged
    {
        //Command
        #region
        public ICommand Load { get { return new CDelegateCommand(LoadWin); } }

        public ICommand base_BrowseLogPath { get { return new CDelegateCommand(base_BrowseLogPathMethod); } }

        #endregion

        //Property
        #region
            private bool m_IsConnect = false;
            public bool IsConnect { get { return m_IsConnect; } }

           //basesetting
            private CBaseSetting m_Base;
            public string base_Svr_Ip { get { return m_Base.Svr.Ip; } set { m_Base.Svr.Ip = value; m_Base.NeedSave(); } }
            public int base_Svr_Port { get { return m_Base.Svr.Port; } set { m_Base.Svr.Port = value; m_Base.NeedSave(); } }
            public string base_LogSvr_Ip { get { return m_Base.LogSvr.Ip; } set { m_Base.LogSvr.Ip = value; m_Base.NeedSave(); } }
            public int base_LogSvr_Port { get { return m_Base.LogSvr.Port; } set { m_Base.LogSvr.Port = value; m_Base.NeedSave(); } }
            public bool base_IsSaveCallLog { get { return m_Base.IsSaveCallLog; } set { m_Base.IsSaveCallLog = value; m_Base.NeedSave(); } }
            public bool base_IsSaveMsgLog { get { return m_Base.IsSaveMsgLog; } set { m_Base.IsSaveMsgLog = value; m_Base.NeedSave(); } }
            public bool base_IsSavePositionLog { get { return m_Base.IsSavePositionLog; } set { m_Base.IsSavePositionLog = value; m_Base.NeedSave(); } }
            public bool base_IsSaveControlLog { get { return m_Base.IsSaveControlLog; } set { m_Base.IsSaveControlLog = value; m_Base.NeedSave(); } }
            public bool base_IsSaveJobLog { get { return m_Base.IsSaveJobLog; } set { m_Base.IsSaveJobLog = value; m_Base.NeedSave(); } }
            public bool base_IsSaveTrackerLog { get { return m_Base.IsSaveTrackerLog; } set { m_Base.IsSaveTrackerLog = value; m_Base.NeedSave(); } }
            public bool base_IsSaveLocationInDoorLog { get { return m_Base.IsSaveLocationInDoorLog; } set { m_Base.IsSaveLocationInDoorLog = value; m_Base.NeedSave(); } }
            public string base_LogPath { get { return m_Base.LogPath; } set { m_Base.LogPath = value; m_Base.NeedSave(); } }


            //radiosetting
            private CRadioSetting m_Radio;

            public bool radio_IsEnable { get { return m_Radio.IsEnable; } set {
                m_Radio.IsEnable = value; 
                m_Radio.NeedSave(); 
                if(value)
                {
                    m_Repeater.IsEnable = false;
                    m_Repeater.NeedSave();
                    PropertyChanged(this, new PropertyChangedEventArgs("repeater_IsEnable"));
                }
            } }
            public bool radio_IsRideMnis { get { return !m_Radio.IsOnlyRide; } set {               
                m_Radio.IsOnlyRide = !value; 
                m_Radio.NeedSave();
                PropertyChanged(this, new PropertyChangedEventArgs("radio_IsOnlyRide"));
            }}
            public bool radio_IsOnlyRide { get { return m_Radio.IsOnlyRide; } set {
                m_Radio.IsOnlyRide = value; 
                m_Radio.NeedSave();
                PropertyChanged(this, new PropertyChangedEventArgs("radio_IsRideMnis"));
            }}
            public string radio_Svr_Ip { get { return m_Radio.Svr.Ip; } set { m_Radio.Svr.Ip = value; m_Radio.NeedSave(); } }
            public int radio_Svr_port { get { return m_Radio.Svr.Port; } set { m_Radio.Svr.Port = value; m_Radio.NeedSave(); } }
            public string radio_Ride_Host { get { return m_Radio.Ride.Host; } set { m_Radio.Ride.Host = value; m_Radio.NeedSave(); } }
            public int radio_Ride_MessagePort { get { return m_Radio.Ride.MessagePort; } set { m_Radio.Ride.MessagePort = value; m_Radio.NeedSave(); } }
            public int radio_Ride_ArsPort { get { return m_Radio.Ride.ArsPort; } set { m_Radio.Ride.ArsPort = value; m_Radio.NeedSave(); } }
            public int radio_Ride_GpsPort { get { return m_Radio.Ride.GpsPort; } set { m_Radio.Ride.GpsPort = value; m_Radio.NeedSave(); } }
            public int radio_Ride_XnlPort { get { return m_Radio.Ride.XnlPort; } set { m_Radio.Ride.XnlPort = value; m_Radio.NeedSave(); } }

            public string radio_Mnis_Host { get { return m_Radio.Mnis.Host; } set { m_Radio.Mnis.Host = value; m_Radio.NeedSave(); } }
            public int radio_Mnis_MessagePort { get { return m_Radio.Mnis.MessagePort; } set { m_Radio.Mnis.MessagePort = value; m_Radio.NeedSave(); } }
            public int radio_Mnis_ArsPort { get { return m_Radio.Mnis.ArsPort; } set { m_Radio.Mnis.ArsPort = value; m_Radio.NeedSave(); } }
            public int radio_Mnis_GpsPort { get { return m_Radio.Mnis.GpsPort; } set { m_Radio.Mnis.GpsPort = value; m_Radio.NeedSave(); } }
            public int radio_Mnis_XnlPort { get { return m_Radio.Mnis.XnlPort; } set { m_Radio.Mnis.XnlPort = value; m_Radio.NeedSave(); } }

            public string radio_GpsC_Ip { get { return m_Radio.GpsC.Ip; } set { m_Radio.GpsC.Ip = value; m_Radio.NeedSave(); } }
            public int radio_GpsC_port { get { return m_Radio.GpsC.Port; } set { m_Radio.GpsC.Port = value; m_Radio.NeedSave(); } }


            //repeatersetting
            private CRepeaterSetting m_Repeater;

            public bool repeater_IsEnable { get { return m_Repeater.IsEnable; } set {m_Repeater.IsEnable = value; 
                m_Repeater.NeedSave();
                if (value)
                {
                    m_Radio.IsEnable = false;
                    m_Radio.NeedSave();
                    PropertyChanged(this, new PropertyChangedEventArgs("radio_IsEnable"));
                }
            } }
            public bool repeater_CPC { get { return m_Repeater.Type == WireLanType.CPC; } set { if (value) { m_Repeater.Type = WireLanType.CPC; m_Repeater.NeedSave(); } } }
            public bool repeater_IPSC { get { return m_Repeater.Type == WireLanType.IPSC; } set { if (value) { m_Repeater.Type = WireLanType.IPSC; m_Repeater.NeedSave(); } } }
            public bool repeater_LCP { get { return m_Repeater.Type == WireLanType.LCP; } set { if (value) { m_Repeater.Type = WireLanType.LCP; m_Repeater.NeedSave(); } } }

            public string repeater_Svr_Ip { get { return m_Repeater.Svr.Ip; } set { m_Repeater.Svr.Ip = value; m_Repeater.NeedSave(); } }
            public int repeater_Svr_port { get { return m_Repeater.Svr.Port; } set { m_Repeater.Svr.Port = value; m_Repeater.NeedSave(); } }
            public string repeater_Master_Ip { get { return m_Repeater.Master.Ip; } set { m_Repeater.Master.Ip = value; m_Repeater.NeedSave(); } }
            public int repeater_Master_port { get { return m_Repeater.Master.Port; } set { m_Repeater.Master.Port = value; m_Repeater.NeedSave(); } }

            public string repeater_Mnis_Host { get { return m_Repeater.Mnis.Host; } set { m_Repeater.Mnis.Host = value; m_Repeater.NeedSave(); } }
            public int repeater_Mnis_ID { get { return m_Repeater.Mnis.ID; } set { m_Repeater.Mnis.ID = value; m_Repeater.NeedSave(); } }
            public int repeater_Mnis_MessagePort { get { return m_Repeater.Mnis.MessagePort; } set { m_Repeater.Mnis.MessagePort = value; m_Repeater.NeedSave(); } }
            public int repeater_Mnis_ArsPort { get { return m_Repeater.Mnis.ArsPort; } set { m_Repeater.Mnis.ArsPort = value; m_Repeater.NeedSave(); } }
            public int repeater_Mnis_GpsPort { get { return m_Repeater.Mnis.GpsPort; } set { m_Repeater.Mnis.GpsPort = value; m_Repeater.NeedSave(); } }
            public int repeater_Mnis_XnlPort { get { return m_Repeater.Mnis.XnlPort; } set { m_Repeater.Mnis.XnlPort = value; m_Repeater.NeedSave(); } }


            public int repeater_DefaultGroupId { get { return m_Repeater.DefaultGroupId; } set { m_Repeater.DefaultGroupId = value; m_Repeater.NeedSave(); } }
            public int repeater_DefaultChannel { get { return m_Repeater.DefaultChannel - 1; } set { m_Repeater.DefaultChannel = value + 1; m_Repeater.NeedSave(); } }
            public int repeater_MinHungTime { get { return m_Repeater.MinHungTime; } set { m_Repeater.MinHungTime = value; m_Repeater.NeedSave(); } }

            public int repeater_MaxSiteAliveTime { get { return m_Repeater.MaxSiteAliveTime; } set { m_Repeater.MaxSiteAliveTime = value; m_Repeater.NeedSave(); } }
            public int repeater_MaxPeerAliveTime { get { return m_Repeater.MaxPeerAliveTime; } set { m_Repeater.MaxPeerAliveTime = value; m_Repeater.NeedSave(); } }

            public int repeater_LocalPeerId { get { return m_Repeater.LocalPeerId; } set { m_Repeater.LocalPeerId = value; m_Repeater.NeedSave(); } }
            public int repeater_LocalRadioId { get { return m_Repeater.LocalRadioId; } set { m_Repeater.LocalRadioId = value; m_Repeater.NeedSave(); } }

            public int repeater_Dongle_Com_Index { get { return m_Repeater.Dongle.Com - 1; } set { m_Repeater.Dongle.Com = value + 1; m_Repeater.NeedSave(); } }

        #endregion
        public event PropertyChangedEventHandler PropertyChanged;

        private CConfiguration m_Configure;

        public CVMConfiguration()
        {
            if (m_Base == null)
            {
                m_Base = new CBaseSetting();
                m_Base.OnConfigurationChanged += OnConfiguratuinChanged;
            }

            if (m_Radio == null)
            {
                m_Radio = new CRadioSetting();
                m_Radio.OnConfigurationChanged += OnConfiguratuinChanged;
            }

            if(m_Repeater == null)
            {
                m_Repeater = new CRepeaterSetting();
                m_Repeater.OnConfigurationChanged += OnConfiguratuinChanged;
            }
        }

        private void OnConfiguratuinChanged(SettingType type, object config)
        {
            switch(type)
            {
                case SettingType.Base:
                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("base_Svr_Ip"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_Svr_Port"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_LogSvr_Ip"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_LogSvr_Port"));

                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveCallLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveMsgLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSavePositionLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveControlLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveJobLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveTrackerLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_IsSaveLocationInDoorLog"));
                        PropertyChanged(this, new PropertyChangedEventArgs("base_LogPath"));
                    }
                    break;
                case SettingType.Radio:
                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_IsEnable"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_IsRideMnis"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_IsOnlyRide"));

                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Svr_Ip"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Svr_port"));


                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Ride_Host"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Ride_MessagePort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Ride_ArsPort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Ride_GpsPort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Ride_XnlPort"));

        
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Mnis_Host"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Mnis_MessagePort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Mnis_ArsPort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Mnis_GpsPort"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_Mnis_XnlPort"));

                       PropertyChanged(this, new PropertyChangedEventArgs("radio_GpsC_Ip"));
                        PropertyChanged(this, new PropertyChangedEventArgs("radio_GpsC_port"));
                    }
                    break;
            }
        }

        //private 

        private void LoadWin()
        {

            ConnectConfiguration();

            //CBaseConfiguration baseseting = new CBaseConfiguration();

            //baseseting.Get();

            if (m_Configure == null)
            {
                //m_Configure = new CConfiguration();
                //m_Configure.ServerStatusChanged += delegate(bool isinit){OnServerStatusChanged(isinit);};
                //m_Configure.OnConfigurationChanged += OnConfiguratuinChanged;
                //m_Configure.Initialize();
            }
        }

        private void base_BrowseLogPathMethod()
        {
            System.Windows.Forms.FolderBrowserDialog sfd = new System.Windows.Forms.FolderBrowserDialog();
            sfd.SelectedPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                m_Base.LogPath = sfd.SelectedPath;
                PropertyChanged(this, new PropertyChangedEventArgs("base_LogPath"));

            }
            else
            {

            }  
        }

        // Method
        public void ConnectConfiguration()
        {
            if (!CConfiguration.IsBindingServerStatusChanged) CConfiguration.ServerStatusChanged += OnServerStatusChanged;
            if (!m_IsConnect) CConfiguration.InitializeTServer();
        }

        private void OnServerStatusChanged(bool isinit)
        {
            m_IsConnect = isinit;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("IsConnect"));
            
            if (isinit)
            {
                m_Base.Get(false);
                m_Radio.Get(false);
                m_Repeater.Get(false);
            }
        }

        public void Save()
        {
            m_Base.Set();
            m_Radio.Set();
            m_Repeater.Set();
        }
    }
}
