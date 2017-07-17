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
    public class CVMBaseSetting : CVMManager, INotifyPropertyChanged
    {

        public CVMBaseSetting()
        {
            if (m_Base == null)
            {
                m_Base = new CBaseSetting();
                m_Base.OnConfigurationChanged += OnConfiguratuinChanged;
            }
          
        }

        #region
        public ICommand BrowseLogPath { get { return new CDelegateCommand(BrowseLogPathMethod); } }

        #endregion


        //basesetting
        private CBaseSetting m_Base;
        public string Svr_Ip { get { return m_Base.Svr.Ip; } set { m_Base.Svr.Ip = value; m_Base.NeedSave(); } }
        public int Svr_Port { get { return m_Base.Svr.Port; } set { m_Base.Svr.Port = value; m_Base.NeedSave(); } }
        public string LogSvr_Ip { get { return m_Base.LogSvr.Ip; } set { m_Base.LogSvr.Ip = value; m_Base.NeedSave(); } }
        public int LogSvr_Port { get { return m_Base.LogSvr.Port; } set { m_Base.LogSvr.Port = value; m_Base.NeedSave(); } }
        public bool IsSaveCallLog { get { return m_Base.IsSaveCallLog; } set { m_Base.IsSaveCallLog = value; m_Base.NeedSave(); } }
        public bool IsSaveMsgLog { get { return m_Base.IsSaveMsgLog; } set { m_Base.IsSaveMsgLog = value; m_Base.NeedSave(); } }
        public bool IsSavePositionLog { get { return m_Base.IsSavePositionLog; } set { m_Base.IsSavePositionLog = value; m_Base.NeedSave(); } }
        public bool IsSaveControlLog { get { return m_Base.IsSaveControlLog; } set { m_Base.IsSaveControlLog = value; m_Base.NeedSave(); } }
        public bool IsSaveJobLog { get { return m_Base.IsSaveJobLog; } set { m_Base.IsSaveJobLog = value; m_Base.NeedSave(); } }
        public bool IsSaveTrackerLog { get { return m_Base.IsSaveTrackerLog; } set { m_Base.IsSaveTrackerLog = value; m_Base.NeedSave(); } }
        public bool IsSaveLocationInDoorLog { get { return m_Base.IsSaveLocationInDoorLog; } set { m_Base.IsSaveLocationInDoorLog = value; m_Base.NeedSave(); } }
        public string LogPath { get { return m_Base.LogPath; } set { m_Base.LogPath = value; m_Base.NeedSave(); } }


        private void OnConfiguratuinChanged(SettingType type, object config)
        {
            if (m_Base == null || type != SettingType.Base) return;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Svr_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("Svr_Port"));
                PropertyChanged(this, new PropertyChangedEventArgs("LogSvr_Ip"));
                PropertyChanged(this, new PropertyChangedEventArgs("LogSvr_Port"));

                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveCallLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveMsgLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSavePositionLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveControlLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveJobLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveTrackerLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsSaveLocationInDoorLog"));
                PropertyChanged(this, new PropertyChangedEventArgs("LogPath"));
            }
        }

        private void BrowseLogPathMethod()
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

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;

        public void Set()
        {
            m_Base.Set();
        }

        public void Get()
        {
            m_Base.Get();
        }
    }
}
