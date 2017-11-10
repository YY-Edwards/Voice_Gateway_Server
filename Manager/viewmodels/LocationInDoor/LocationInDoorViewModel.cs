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
    public class LocationInDoorViewModel : SaveReadableElement
    {
        public LocationInDoorConfigurationViewModel LocationInDoorConfigurationViewModel { get; private set; }
        public AreaViewModel AreaViewModel { get; private set; }

        public BeaconViewModel BeaconViewModel { get; private set; }

        public LocationInDoorViewModel()
        {
            LocationInDoorConfigurationViewModel = new LocationInDoorConfigurationViewModel();
            AreaViewModel = new AreaViewModel();
            AreaViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            BeaconViewModel = new BeaconViewModel();

            InitializeElements(this.GetType());
        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (sender == AreaViewModel)
            {
                if (e.PropertyName == "AreaList" && AreaViewModel.AreaList != null)
                {
                    BeaconViewModel.AreaList = AreaViewModel.AreaList;
                }
            }          
        }
    }
}
