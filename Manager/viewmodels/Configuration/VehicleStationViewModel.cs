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
    public class VehicleStationViewModel : ConfigureViewModel<VehicleStation>
    {
        public VehicleStationViewModel()
        {
            _configurationName = Resource.VehicleStation;
             ReadOpcode = RequestOpcode.getRadioSetting;
             SaveOpcode = RequestOpcode.setRadioSetting;
        }

        public bool IsEnable { get { return _configuration.IsEnable; } set { _configuration.IsEnable = value; IsChanged = true; NotifyPropertyChanged("IsEnable"); } }
        public int TomeoutSeconds { get { return _configuration.TomeoutSeconds; } set { _configuration.TomeoutSeconds = value; IsChanged = true; } }

        public string SvrIp { get { return  _configuration.Svr.Ip; } set { _configuration.Svr.Ip = value; IsChanged = true; } }
        public int SvrPort { get { return _configuration.Svr.Port; } set { _configuration.Svr.Port = value; IsChanged = true; } }

        public int RideID { get { return _configuration.Ride.ID; } set { _configuration.Ride.ID = value; IsChanged = true; } }
        public string RideIHost { get { return _configuration.Ride.Host; } set { _configuration.Ride.Host = value; IsChanged = true; } }
        public int RideMessagePort { get { return _configuration.Ride.MessagePort; } set { _configuration.Ride.MessagePort = value; IsChanged = true; } }
        public int RideArsPort { get { return _configuration.Ride.ArsPort; } set { _configuration.Ride.ArsPort = value; IsChanged = true; } }
        public int RideGpsPort { get { return _configuration.Ride.GpsPort; } set { _configuration.Ride.GpsPort = value; IsChanged = true; } }
        public int RideXnlPort { get { return _configuration.Ride.XnlPort; } set { _configuration.Ride.XnlPort = value; IsChanged = true; } }

        public VehicleStation.ModeType_t RideWorkMode { get { return _configuration.Ride.WorkMode; } set { _configuration.Ride.WorkMode = value; IsChanged = true; } }
        public bool GeneralMode { get { return _configuration.Ride.WorkMode == VehicleStation.ModeType_t.General; } }
        public bool ClusterMode { get { return _configuration.Ride.WorkMode == VehicleStation.ModeType_t.Cluster; } }

        public int CAI { get { return _configuration.CAI; } set { _configuration.CAI = value; IsChanged = true; } }
        public int GroupCAI { get { return _configuration.GroupCAI; } set { _configuration.GroupCAI = value; IsChanged = true; } }
        public Configuration.LocationQueryType_t LocationQueryType { get { return _configuration.LocationQueryType; } set { _configuration.LocationQueryType = value; IsChanged = true; NotifyPropertyChanged("LocationQueryType"); } }
        public bool GeneralQuery { get { return _configuration.LocationQueryType == Configuration.LocationQueryType_t.General; } }
        public bool CSBKQuery { get { return _configuration.LocationQueryType == Configuration.LocationQueryType_t.CSBK; } }
        public bool EnhCSBKQuery { get { return _configuration.LocationQueryType == Configuration.LocationQueryType_t.EnhCSBK; } }

       protected override void OnConfgurationChanged()
        {
            if (_configuration == null) return;

            NotifyPropertyChanged(new string[]{
                "IsEnable"
                ,"TomeoutSeconds"

                ,"SvrIp"
                ,"SvrPort"
                ,"RideID"
                ,"RideHost"
                ,"RideMessagePort"
                ,"RideArsPort"
                ,"RideGpsPort"
                ,"RideXnlPort"

                ,"GeneralMode"
                ,"ClusterMode"

                ,"CAI"
                ,"GroupCAI"
                ,"GeneralQuery"
                ,"CSBKQuery"
                ,"EnhCSBKQuery"
            });
        }
    }
}
