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

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{
    public class BaseViewModel : ConfigureViewModel<Base>
    {      
        public BaseViewModel()
        {
            _configurationName = Resource.Base;
            ReadOpcode = RequestOpcode.getBaseSetting;
            SaveOpcode = RequestOpcode.setBaseSetting;
        }

        public string Svr_Ip { get { return _configuration.TSvr.Ip; } set { _configuration.TSvr.Ip = value; IsChanged = true; } }
        public int Svr_Port { get { return _configuration.TSvr.Port; } set { _configuration.TSvr.Port = value; IsChanged = true; } }
        public string LogSvr_Ip { get { return _configuration.LogSvr.Ip; } set { _configuration.LogSvr.Ip = value; IsChanged = true; } }
        public int LogSvr_Port { get { return _configuration.LogSvr.Port; } set { _configuration.LogSvr.Port = value; IsChanged = true; } }
        public bool IsSaveCallLog { get { return _configuration.IsSaveCallLog; } set { _configuration.IsSaveCallLog = value; IsChanged = true; } }
        public bool IsSaveMsgLog { get { return _configuration.IsSaveMsgLog; } set { _configuration.IsSaveMsgLog = value; IsChanged = true; } }
        public bool IsSavePositionLog { get { return _configuration.IsSavePositionLog; } set { _configuration.IsSavePositionLog = value; IsChanged = true; } }
        public bool IsSaveControlLog { get { return _configuration.IsSaveControlLog; } set { _configuration.IsSaveControlLog = value; IsChanged = true; } }
        public bool IsSaveJobLog { get { return _configuration.IsSaveJobLog; } set { _configuration.IsSaveJobLog = value; IsChanged = true; } }
        public bool IsSaveTrackerLog { get { return _configuration.IsSaveTrackerLog; } set { _configuration.IsSaveTrackerLog = value; IsChanged = true; } }
        public bool IsSaveLocationInDoorLog { get { return _configuration.IsSaveLocationInDoorLog; } set { _configuration.IsSaveLocationInDoorLog = value; IsChanged = true; } }
        public string LogPath { get { return _configuration.LogPath; } set { _configuration.LogPath = value; IsChanged = true; } }

        protected override void OnConfgurationChanged()
        {
            if (_configuration == null) return;

            NotifyPropertyChanged(new string[]{
                "Svr_Ip",
                "Svr_Port",
                "LogSvr_Ip",
                "LogSvr_Port",

                "IsSaveCallLog",
                "IsSaveMsgLog",
                "IsSavePositionLog",
                "IsSaveControlLog",
                "IsSaveJobLog",
                "IsSaveTrackerLog",
                "IsSaveLocationInDoorLog",
                "LogPath",               
            });           
        }
    }
}
