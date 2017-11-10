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
    public class MnisViewModel : ConfigureViewModel<Mnis>
    {
        public MnisViewModel()
        {
            _configurationName = Resource.Mnis;
             ReadOpcode = RequestOpcode.getMnisSetting;
             SaveOpcode = RequestOpcode.setMnisSetting;
        }
        public bool IsEnable { get { return _configuration.IsEnable; } set { _configuration.IsEnable = value; IsChanged = true; NotifyPropertyChanged("IsEnable"); } }
        public int TomeoutSeconds { get { return _configuration.TomeoutSeconds; } set { _configuration.TomeoutSeconds = value; IsChanged = true; } }
        public int ID { get { return _configuration.ID; } set { _configuration.ID = value; IsChanged = true; } }

        public string Host { get { return _configuration.Host; } set { _configuration.Host = value; IsChanged = true; } }

        public int MessagePort { get { return _configuration.MessagePort; } set { _configuration.MessagePort = value; IsChanged = true; } }
        public int ArsPort { get { return _configuration.ArsPort; } set { _configuration.ArsPort = value; IsChanged = true; } }
        public int GpsPort { get { return _configuration.GpsPort; } set { _configuration.GpsPort = value; IsChanged = true; } }
        public int XnlPort { get { return _configuration.XnlPort; } set { _configuration.XnlPort = value; IsChanged = true; } }

        public int CAI { get { return _configuration.CAI; } set { _configuration.CAI = value; IsChanged = true; } }
        public int GroupCAI { get { return _configuration.GroupCAI; } set { _configuration.GroupCAI = value; IsChanged = true; } }

        public Configuration.LocationQueryType_t LocationQueryType { get { return _configuration.LocationQueryType; } set { _configuration.LocationQueryType = value; IsChanged = true; NotifyPropertyChanged("LocationQueryType"); } }
        public bool GeneralQuery{ get {return _configuration.LocationQueryType == Configuration.LocationQueryType_t.General;}}
        public bool CSBKQuery { get { return _configuration.LocationQueryType == Configuration.LocationQueryType_t.CSBK; } }
        public bool EnhCSBKQuery { get { return _configuration.LocationQueryType == Configuration.LocationQueryType_t.EnhCSBK; } }

        protected override void OnConfgurationChanged()
        {
            if (_configuration == null) return;

            NotifyPropertyChanged(new string[]{
                "IsEnable"
                ,"TomeoutSeconds"
                ,"ID"
                ,"Host"
                ,"MessagePort"
                ,"ArsPort"
                ,"GpsPort"
                ,"XnlPort"
                ,"CAI"
                ,"GroupCAI"
                ,"GeneralQuery"
                ,"CSBKQuery"
                ,"EnhCSBKQuery"

            });
        }
    }
}
