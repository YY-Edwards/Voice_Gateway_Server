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
using System.Windows.Data;

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{
    public class LocationInDoorConfigurationViewModel : ConfigureViewModel<LocationInDoor>
    {      
        private List<double> _IntervalList;
        private Configuration.LocationQueryType_t _queryType;

        public LocationInDoorConfigurationViewModel()
        {
            _configurationName = Resource.LocationInDoor;
            ReadOpcode = RequestOpcode.getLocationInDoorSetting;
            SaveOpcode = RequestOpcode.setLocationInDoorSetting;

            QueryType = Configuration.LocationQueryType_t.General;
            _IntervalList = Utility.GeneralQueryIntervals;        
        }


        public Configuration.LocationQueryType_t QueryType { get { return _queryType; } set { _queryType = value; SetQueryType(value); } }
        public ICollectionView Intervals { get { return new ListCollectionView(_IntervalList); } }

        public bool IsEnable { get { return _configuration.IsEnable; } set { _configuration.IsEnable = value; IsChanged = true; } }


        public int IntervalIndex { get { return _IntervalList.FindIndex(p => p == _configuration.Interval); } set { _configuration.Interval = _IntervalList[value]; IsChanged = true; } }

        public int iBeaconNumber { get { return _configuration.iBeaconNumber; } set { _configuration.iBeaconNumber = value; IsChanged = true; } }

        public bool IsEmergency { get { return _configuration.IsEmergency; } set { _configuration.IsEmergency = value; IsChanged = true; } }
        
        protected override void OnConfgurationChanged()
        {
            if (_configuration == null) return;

            NotifyPropertyChanged(new string[]{
                "IsEnable"
                ,"IntervalIndex"
                ,"iBeaconNumber"
                ,"IsEmergency"
            });
        }

        private void SetQueryType(Configuration.LocationQueryType_t type)
        {
            switch (type)
            {
                case Configuration.LocationQueryType_t.CSBK:
                    _IntervalList = Utility.CSBKQueryIntervals;
                    break;
                case Configuration.LocationQueryType_t.EnhCSBK:
                    _IntervalList = Utility.EnhCSBKQueryIntervals;
                    break;
                default:
                    _IntervalList = Utility.GeneralQueryIntervals;
                    break;
            }

            NotifyPropertyChanged(new string[] { "Intervals", "IntervalIndex" });
        }
    }
}
