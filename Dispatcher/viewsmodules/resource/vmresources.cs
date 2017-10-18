using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;

using Sigmar;
using Sigmar.Logger;
using Dispatcher.Service;

using Dispatcher.Modules;
using Sigmar.Extension;

namespace Dispatcher.ViewsModules
{
    public class VMResources : INotifyPropertyChanged
    {
        private VMOrganization _organizationviewmodule;

        public VMOrganization OrganizationViewModule
        {
            get
            {
                return _organizationviewmodule;
            }
            private set
            {
                _organizationviewmodule = value;
                NotifyPropertyChanged("OrganizationViewModule");
            }
        }

        public event OperatedEventHandler OnOperated;
        public event EventHandler OnResourcesInitilized;
        private ResourcesMgr _resource;

        private ICollectionView _groups, _staffs, _vehicles, _devices, _ibeacons;

        public ICollectionView Groups { get{ return _groups; } private set{_groups = value ; NotifyPropertyChanged("Groups");} }
        public ICollectionView Staffs { get { return _staffs; } private set { _staffs = value; NotifyPropertyChanged("Staffs"); } }
        public ICollectionView Vehicles { get { return _vehicles; } private set { _vehicles = value; NotifyPropertyChanged("Vehicles"); } }
        public ICollectionView Devives { get { return _devices; } private set { _devices = value; NotifyPropertyChanged("Devives"); } }
        public ICollectionView Beacons { get { return _ibeacons; } private set { _ibeacons = value; NotifyPropertyChanged("Beacons"); } }
        public VMResources()
        {
            OrganizationViewModule = new VMOrganization();
            OrganizationViewModule.OnOperated += new OperatedEventHandler(OnChildOperated);

            if (_resource == null)
            {
                _resource = ResourcesMgr.Instance();
                _resource.OnResourcesLoaded += new EventHandler(OnResInitilized);

            }

            InitilizeViewResoures();
        }

        public void OnChildOperated(OperatedEventArgs e)
        {
            if (OnOperated != null) OnOperated(e);
        }

        private void OnResInitilized(object sender, EventArgs e)
        {
            if (OnResourcesInitilized != null) OnResourcesInitilized(sender, e);
        }

        public ICommand Loaded { get { return new Command(LoadedExec); } }

        private void LoadedExec(object parameter)
        {
            //_resource.RegisterLoggerEvents(this);
            if (!_resource.IsInitialized) _resource.InitializeResources(this);            
        }

        public ICommand UpdateResource { get { return new Command(UpdateResourceExec); } }
        private void UpdateResourceExec()
        {
            switch(_pagekey)
            {
                case ResourceKey_t.Organization:
                    _organizationviewmodule.UpdateResource.Execute(_resource);
                    break;
                case ResourceKey_t.Group:
                    UpdateGroup();
                    break;
                case ResourceKey_t.Beacon:
                    UpdateBeacons();
                    break;
            }

            UpdateOthers();
        }

        private void UpdateOthers()
        {
            if (_pagekey != ResourceKey_t.Organization) _organizationviewmodule.UpdateResource.Execute(_resource);
            if (_pagekey != ResourceKey_t.Group) UpdateGroup();
            if (_pagekey != ResourceKey_t.Staff || _pagekey != ResourceKey_t.Vehicle || _pagekey != ResourceKey_t.Device) UpdateMembers();
            if (_pagekey != ResourceKey_t.Beacon) UpdateBeacons();
        }

        private void UpdateGroup()
        {
            Groups = new ListCollectionView(_resource.Groups);            
        }


        private void UpdateMembers()
        {
            Staffs = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Staff));
            Vehicles = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Vehicle));
            Devives = new ListCollectionView(_resource.Members.FindAll(p => p.Member.HasDevice)); 
        }

       
        private void UpdateBeacons()
        {
            Beacons = new ListCollectionView(_resource.Beacons);            
        }

        public ICommand SelectionChanged { get { return new Command(SelectionChangedExec); } }

        private void SelectionChangedExec(object parameter)
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.SetCurrentTarget, parameter));
        }
       
        //Resource
        private bool _enableViewResOrganization;
        private bool _enableViewResGroup;
        private bool _enableViewResStaff;
        private bool _enableViewResVehicle;
        private bool _enableViewResDevice;
        private bool _enableViewResBeacon;


        public Visibility OrganizationVisible { get { return _enableViewResOrganization ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility GroupVisible { get { return _enableViewResGroup ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility StaffVisible { get { return _enableViewResStaff ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility VehicleVisible { get { return _enableViewResVehicle ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility DeviceVisible { get { return _enableViewResDevice ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility BeaconVisible { get { return _enableViewResBeacon ? Visibility.Visible : Visibility.Collapsed; } }

        private void InitilizeViewResoures()
        {
            _enableViewResOrganization = FunctionConfigure.EnableViewResOrganization;
            _enableViewResGroup = FunctionConfigure.EnableViewResGroup;
            _enableViewResStaff = FunctionConfigure.EnableViewResStaff;
            _enableViewResVehicle = FunctionConfigure.EnableViewResVehicle;
            _enableViewResDevice = FunctionConfigure.EnableViewResDevice;
            _enableViewResBeacon = FunctionConfigure.EnableViewResBeacon;

            NotifyPropertyChanged("OrganizationVisible");
            NotifyPropertyChanged("GroupVisible");
            NotifyPropertyChanged("StaffVisible");
            NotifyPropertyChanged("VehicleVisible");
            NotifyPropertyChanged("DeviceVisible");
            NotifyPropertyChanged("BeaconVisible");
        }

        public void UpdateView(OperatedEventArgs e)
        {
            try
            {
                switch ((ResourceKey_t)e.parameter)
                {
                    case ResourceKey_t.Organization:
                        _enableViewResOrganization = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("OrganizationVisible");
                        break;
                    case ResourceKey_t.Group:
                        _enableViewResGroup = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("GroupVisible");
                        break;
                    case ResourceKey_t.Staff:
                        _enableViewResStaff = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("StaffVisible");
                        break;
                    case ResourceKey_t.Vehicle:
                        _enableViewResVehicle = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("VehicleVisible");
                        break;
                    case ResourceKey_t.Device:
                        _enableViewResDevice = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("DeviceVisible");
                        break;
                    case ResourceKey_t.Beacon:
                        _enableViewResBeacon = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("BeaconVisible");
                        break;
                    default:
                        break;
                }
            }
            catch (Exception ex)
            {
                //WARNING("There is not type of " + e.parameter.ToString() + " tools");
            }
        }

        private ResourceKey_t _pagekey = ResourceKey_t.Organization;
        public int PageIndex { get { return (int)_pagekey; } private set { _pagekey = (ResourceKey_t)value; NotifyPropertyChanged("PageIndex"); } }

        public ICommand ChangePage { get { return new Command(ChangePageExec); } }

        private void ChangePageExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                _pagekey = ((string)parameter).ToEnum<ResourceKey_t>();
                NotifyPropertyChanged("PageIndex");

                //SerchKey = "";
                SearchExec();
            }
        }

        private string _serchkey = "";
        public string SerchKey { get { return _serchkey; } set { _serchkey = value; NotifyPropertyChanged("SerchKey"); } }
        public ICommand Search { get { return new Command(SearchExec); } }

        private void SearchExec()
        {
            switch(((ResourceKey_t)PageIndex))
            {
                case ResourceKey_t.Organization:
                    OrganizationViewModule.Search.Execute(_serchkey);
                    break;
                case ResourceKey_t.Group:
                    if (_serchkey == null || _serchkey == "") Groups = new ListCollectionView(_resource.Groups); 
                    else  Groups = new ListCollectionView(_resource.Groups.FindAll(p=> p.FullName.IndexOf(_serchkey) >= 0));
                    break;
                case ResourceKey_t.Staff:
                    if (_serchkey == null || _serchkey == "")  Staffs = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Staff));
                    else   Staffs = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Staff && p.FullName.IndexOf(_serchkey) >= 0));
                    break;
                case ResourceKey_t.Vehicle:
                     if (_serchkey == null || _serchkey == "")  Vehicles = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Vehicle));
                    else   Vehicles = new ListCollectionView(_resource.Members.FindAll(p => p.Member.MemberType == CMember.MemberType_t.Vehicle && p.FullName.IndexOf(_serchkey) >= 0));
                    break;
                case ResourceKey_t.Device:
                     if (_serchkey == null || _serchkey == "")  Devives = new ListCollectionView(_resource.Members.FindAll(p => p.Member.HasDevice));
                    else   Devives = new ListCollectionView(_resource.Members.FindAll(p => p.Member.HasDevice && p.FullName.IndexOf(_serchkey) >= 0));
                    break;
                case ResourceKey_t.Beacon:
                    // if (_serchkey == null || _serchkey == "") Groups = new ListCollectionView(_resource.Groups); 
                    //else  Groups = new ListCollectionView(_resource.Groups.FindAll(p=> p.FullName.IndexOf(_serchkey) >= 0));
                    //break;
                    break;
            }
        }

         
        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion
    }
}
