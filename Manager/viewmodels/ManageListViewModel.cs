using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;

using Manager.Models;
using Manager.Views;


namespace Manager.ViewModels
{
    public class ManageListViewModel : SaveReadableElement, INotifyPropertyChanged
    {
        public BaseViewModel BaseViewModel { get; private set;}
        public VehicleStationViewModel VehicleStationViewModel { get; private set; }
        public RepeaterViewModel RepeaterViewModel { get; private set; }
        public MnisViewModel MnisViewModel { get; private set; }
        public LocationViewModel LocationViewModel { get; private set; }

        public LocationInDoorViewModel LocationInDoorViewModel { get; private set; }


        public UsersViewModel UsersViewModel { get; private set; }
        public DispatchResourcesViewModel DispatchResourcesViewModel { get; private set; }


        public RegisterViewModel RegisterViewModel { get; private set; }



        //public event MessageHandler Notified;

        //private BaseSettingView _BaseSetting;
        //private RadioSettingView _RadioSetting;
        //private RepeaterSettingView _RepeaterSetting;
        //private MnisSettingView _MnisSetting;
        //private LocationSettingView _LocationSetting;
        //private LocationInDoorSettingView _LocationInDoorSetting;

        public ManageListViewModel()
        {
            BaseViewModel = new BaseViewModel();

            VehicleStationViewModel = new VehicleStationViewModel();
            VehicleStationViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            RepeaterViewModel = new RepeaterViewModel();
            RepeaterViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            MnisViewModel = new MnisViewModel();
            MnisViewModel.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

            LocationViewModel = new LocationViewModel();

            LocationInDoorViewModel = new LocationInDoorViewModel();

            UsersViewModel = new UsersViewModel();
            DispatchResourcesViewModel = new DispatchResourcesViewModel();

            RegisterViewModel = new RegisterViewModel();
            

            InitializeElements(this.GetType());
          
        }

       
        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (sender == VehicleStationViewModel)
            {
                if(e.PropertyName == "IsEnable")
                {
                    if (VehicleStationViewModel.IsEnable)
                    {
                        if(RepeaterViewModel.IsEnable)RepeaterViewModel.IsEnable = false;
                        RegisterViewModel.DeviceType = Register.Device_t.VehicleStation;
                    }
                }
                else if(e.PropertyName == "LocationQueryType")
                {
                    if (!MnisViewModel.IsEnable)
                    {
                        if (LocationViewModel.QueryType != VehicleStationViewModel.LocationQueryType) LocationViewModel.QueryType = VehicleStationViewModel.LocationQueryType;
                        if (LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType != VehicleStationViewModel.LocationQueryType) LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType = VehicleStationViewModel.LocationQueryType;

                    }
                }
            }
            else if(sender == RepeaterViewModel)
            {
                if (e.PropertyName == "IsEnable")
                {
                    if (RepeaterViewModel.IsEnable)
                    {
                        if(VehicleStationViewModel.IsEnable)VehicleStationViewModel.IsEnable = false;
                        RegisterViewModel.DeviceType = Register.Device_t.Repeater;
                    }
                }
            }
            else if(sender == MnisViewModel)
            {
                if (e.PropertyName == "IsEnable" || e.PropertyName == "LocationQueryType")
                {
                    if(MnisViewModel.IsEnable)
                    {
                        if (LocationViewModel.QueryType != MnisViewModel.LocationQueryType) LocationViewModel.QueryType = MnisViewModel.LocationQueryType;
                        if (LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType != MnisViewModel.LocationQueryType) LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType = MnisViewModel.LocationQueryType;

                    }
                    else
                    {
                        if (LocationViewModel.QueryType != VehicleStationViewModel.LocationQueryType) LocationViewModel.QueryType = VehicleStationViewModel.LocationQueryType;
                        if (LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType != VehicleStationViewModel.LocationQueryType) LocationInDoorViewModel.LocationInDoorConfigurationViewModel.QueryType = VehicleStationViewModel.LocationQueryType;
                    }
                }
            }
        }

        //public BaseSettingView BaseSetting { get { return _BaseSetting; } private set { _BaseSetting = value; } }
        //public RadioSettingView RadioSetting{get{return _RadioSetting;} private set{_RadioSetting = value;}}
        //public RepeaterSettingView RepeaterSetting { get { return _RepeaterSetting; } private set { _RepeaterSetting = value; } }
        //public MnisSettingView MnisSetting { get { return _MnisSetting; } private set { _MnisSetting = value; } }
        //public LocationSettingView LocationSetting { get { return _LocationSetting; } private set { _LocationSetting = value; } }
        //public LocationInDoorSettingView LocationInDoorSetting { get { return _LocationInDoorSetting; } private set { _LocationInDoorSetting = value; } }

        //private void OnRadioSettingEnableChanged(object sender, bool enable)
        //{
        //    if(enable)
        //    {
        //        RepeaterSetting.Disable();
        //    }
        //}

        //private void OnRadioSettingQueryTypChanged(object sender, LocationQueryType_t type)
        //{
        //    if (!MnisSetting.Enable)
        //    {
        //        _LocationSetting.SetQueryType(type);
        //        _LocationInDoorSetting.SetQueryType(type);
        //    }
        //}

        //private void OnRepeaterSettingEnableChanged(object sender, bool enable)
        //{
        //    if (enable)
        //    {
        //        RadioSetting.Disable();
        //    }
        //}


        //private void OnMnisSettingQueryTypChanged(object sender, LocationQueryType_t type)
        //{
        //    _LocationSetting.SetQueryType(type);
        //    _LocationInDoorSetting.SetQueryType(type);
        //}
      
        public ICommand ReadManagement
        {
            get 
            {
                return new Command(() =>
                {
                    if (RegisterViewModel!=null) RegisterViewModel.Query.Execute(null);
                    Read();  
                });
            }
        }

        public ICommand SaveManagement
        {
            get
            {
                return new Command(() =>
                {
                    Save();
                });
            }
        }

        public override SaveStatus Save()
        {
            Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.OpenNotifyWindow));
            Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.SetNotifyEnable, false));

            SaveStatus status = base.Save();
            if (status == SaveStatus.Skip || status == SaveStatus.Success)
            {
                Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.AddNotifyLine, Resource.SaveComplete));  
            }
            else
            {
            }

            Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.SetNotifyEnable, true));

            return status;
        }


        public ICommand CancelSave
        {
            get
            {
                return new Command(() =>
                {
                    //NotifyComplete();
                });
            }
        }

        //private void OpenNotifyWindow()
        //{
        //    if (Notified != null)
        //    {
        //        Notified(this, new MessageArgs(Messages.OpenNotifyWindow, null));
        //        Notified(this, new MessageArgs(Messages.SetNotifyEnable, false));
        //    }
        //}
        //private void Notify(string message)
        //{
        //    if (Notified != null) Notified(this, new MessageArgs(Messages.AddNotifyLine, message));
        //}

        //private void NotifyComplete()
        //{
        //    if (Notified != null) Notified(this, new MessageArgs(Messages.SetNotifyEnable, true));
        //}

       

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
