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
    public class RepeaterViewModel: ConfigureViewModel<Repeater>
    {
        public RepeaterViewModel()
        {
            _configurationName = Resource.Repeater;
             ReadOpcode = RequestOpcode.getRepeaterSetting;
             SaveOpcode = RequestOpcode.setRepeaterSetting;
        }

        public bool IsEnable { get { return _configuration.IsEnable; } set { _configuration.IsEnable = value; IsChanged = true; NotifyPropertyChanged("IsEnable"); } }
        public int TomeoutSeconds { get { return _configuration.TomeoutSeconds; } set { _configuration.TomeoutSeconds = value; IsChanged = true; } }
        public Repeater.WireLanType WorkMode { get { return _configuration.WorkMode; } set { _configuration.WorkMode = value; IsChanged = true; } }
        public bool CPC { get { return _configuration.WorkMode == Repeater.WireLanType.CPC; } }
        public bool IPSC { get { return _configuration.WorkMode == Repeater.WireLanType.IPSC; } }
        public bool LCP { get { return _configuration.WorkMode == Repeater.WireLanType.LCP; } }

        public string SvrIp { get { return _configuration.Svr.Ip; } set { _configuration.Svr.Ip = value; IsChanged = true; } }
        public int SvrPort { get { return _configuration.Svr.Port; } set { _configuration.Svr.Port = value; IsChanged = true; } }
        public string MasterIp { get { return _configuration.Master.Ip; } set { _configuration.Master.Ip = value; IsChanged = true; } }
        public int MasterPort { get { return _configuration.Master.Port; } set { _configuration.Master.Port = value; IsChanged = true; } }

        public int LocalPeerId { get { return _configuration.LocalPeerId; } set { _configuration.LocalPeerId = value; IsChanged = true; } }
        public int LocalRadioId { get { return _configuration.LocalRadioId; } set { _configuration.LocalRadioId = value; IsChanged = true; } }

        public int DefaultGroupId { get { return _configuration.DefaultGroupId; } set { _configuration.DefaultGroupId = value; IsChanged = true; } }
        public int DefaultChannel { get { return _configuration.DefaultChannel - 1; } set { _configuration.DefaultChannel = value + 1; IsChanged = true; } }
        public int MinHungTime { get { return _configuration.MinHungTime; } set { _configuration.MinHungTime = value; IsChanged = true; } }

        public int MaxSiteAliveTime { get { return _configuration.MaxSiteAliveTime; } set { _configuration.MaxSiteAliveTime = value; IsChanged = true; } }
        public int MaxPeerAliveTime { get { return _configuration.MaxPeerAliveTime; } set { _configuration.MaxPeerAliveTime = value; IsChanged = true; } }

        public int DongleComIndex { get { return _configuration.Dongle.Com - 1; } set { _configuration.Dongle.Com = value + 1; IsChanged = true; } }

        public string AudioPath { get { return _configuration.AudioPath; } set { _configuration.AudioPath = value; IsChanged = true; } }


        protected override void OnConfgurationChanged()
        {
            if (_configuration == null) return;

            NotifyPropertyChanged(new string[]{
                "IsEnable"
                ,"TomeoutSeconds"       
                ,"CPC"
                ,"IPSC"
                ,"LCP"
                ,"SvrIp"
                ,"SvrPort"
                ,"MasterIp"
                ,"MasterPort"
                ,"LocalPeerId"
                ,"LocalRadioId"
                ,"DefaultGroupId"
                ,"DefaultChannel"
                ,"MinHungTime"
                ,"MaxSiteAliveTime"
                ,"MaxPeerAliveTime"
                ,"DongleComIndex"
                ,"AudioPath"
            });                              
        }
    }
}
