using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using Manager.Models;

namespace Manager.ViewModels
{
    public class DispatchResourcesViewModel : SaveReadableElement
    {
        public GroupsViewModel GroupListViewModel { get; private set; }
        public StaffsViewModel StaffsViewModel { get; private set; }
        public RadiosViewModel RadiosViewModel { get; private set; }

        public DispatchResourcesViewModel()
        {
            GroupListViewModel = new GroupsViewModel();
            GroupListViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            StaffsViewModel = new StaffsViewModel();
            StaffsViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            RadiosViewModel = new RadiosViewModel();

            InitializeElements(this.GetType());
        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
             if(sender == GroupListViewModel)
             {
                 if(e.PropertyName == "GroupList" && GroupListViewModel.GroupList != null)
                 {
                     StaffsViewModel.GroupList = GroupListViewModel.GroupList;
                     RadiosViewModel.GroupList = GroupListViewModel.GroupList;
                 }
             }
             else if (sender == StaffsViewModel)
             {
                 if (e.PropertyName == "StaffList" && StaffsViewModel.StaffList != null)
                 {
                     RadiosViewModel.StaffList = StaffsViewModel.StaffList;
                 }
             }
        }
    }
}
