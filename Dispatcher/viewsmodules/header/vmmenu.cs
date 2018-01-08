using Dispatcher.Service;
using Dispatcher.Modules;
using Sigmar;
using Sigmar.Extension;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;

using Dispatcher;



namespace Dispatcher.ViewsModules
{
    public class VMMenu :  INotifyPropertyChanged
    {
        public event OperatedEventHandler OnOperated;

         private SaveWork _savework;

        public SaveWork Save {get{return _savework;}}

        public VMMenu()
        {
             if (_savework == null)
            {
                _savework = SaveWork.Instance();
            }
            InitilizeViewTools();
            InitilizeViewRes();
            InitilizeViewNavigation();
            InitilizeViewNotify();
        }
        //File
        public ICommand NewOperate { get { return new Command(NewOperateExec); } }

        private void NewOperateExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                TaskType_t newtype = ((string)parameter).ToEnum<TaskType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenNewOperateWindow, newtype));
            }
        }

        public ICommand SaveWorkspace { get { return new Command(SaveWorkspaceExec); } }

        private void SaveWorkspaceExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.SaveWorkspace));
        }

        public ICommand ExportWorkspace { get { return new Command(ExportWorkspaceExec); } }

        private void ExportWorkspaceExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.ExportWorkspace));
        }

        public ICommand Print { get { return new Command(PrintExec); } }

        private void PrintExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.Print));
        }

        public ICommand Close { get { return new Command(CloseExec); } }

        private void CloseExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowClose));
        }

        //Views
        //Tools
        private bool _enableViewBaseTools;
        private bool _enableViewFastTools;
        private bool _enableViewControlerTools;
        private bool _enableViewLocationInDoorTools;
        private bool _enableViewHelpTools;

        public bool EnableViewBaseTools { get { return _enableViewBaseTools; } private set { _enableViewBaseTools = value; NotifyPropertyChanged("EnableViewBaseTools"); } }
        public bool EnableViewFastTools { get { return _enableViewFastTools; } private set { _enableViewFastTools = value; NotifyPropertyChanged("EnableViewFastTools"); } }
        public bool EnableViewControlerTools { get { return _enableViewControlerTools; } private set { _enableViewControlerTools = value; NotifyPropertyChanged("EnableViewControlerTools"); } }
        public bool EnableViewLocationInDoorTools { get { return _enableViewLocationInDoorTools; } private set { _enableViewLocationInDoorTools = value; NotifyPropertyChanged("EnableViewLocationInDoorTools"); } }
        public bool EnableViewHelpTools { get { return _enableViewHelpTools; } private set { _enableViewHelpTools = value; NotifyPropertyChanged("EnableViewHelpTools"); } }

        private void InitilizeViewTools()
        {
            EnableViewBaseTools = FunctionConfigure.EnableViewBaseTools;
            EnableViewFastTools = FunctionConfigure.EnableViewFastTools;
            EnableViewControlerTools = FunctionConfigure.EnableViewControlerTools;
            EnableViewLocationInDoorTools = FunctionConfigure.EnableViewLocationInDoorTools;
            EnableViewHelpTools = FunctionConfigure.EnableViewHelpTools;
        }

        public ICommand ViewTools { get { return new Command(ViewToolsExec); } }

        private void ViewToolsExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                ToolsKey_t key = ((string)parameter).ToEnum<ToolsKey_t>();
                bool isopen = true;

                switch(key)
                {
                    case ToolsKey_t.Base:
                        EnableViewBaseTools = !EnableViewBaseTools;
                        isopen = EnableViewBaseTools;
                        break;
                    case ToolsKey_t.Fast:
                        EnableViewFastTools = !EnableViewFastTools;
                        isopen = EnableViewFastTools;
                        break;
                    case ToolsKey_t.Controler:
                        EnableViewControlerTools = !EnableViewControlerTools;
                        isopen = EnableViewControlerTools;
                        break;
                    case ToolsKey_t.LocationInDoor:
                        EnableViewLocationInDoorTools = !EnableViewLocationInDoorTools;
                        isopen = EnableViewLocationInDoorTools;
                        break;
                    case ToolsKey_t.Help:
                        EnableViewHelpTools = !EnableViewHelpTools;
                        isopen = EnableViewHelpTools;
                        break;
                    default: return;
                }

                if (OnOperated != null) OnOperated(new OperatedEventArgs(isopen ? OperateType_t.OpenTools:OperateType_t.CloseTools, key));
            }
        }


        //Resource
        private bool _enableViewResOrganization;
        private bool _enableViewResGroup;
        private bool _enableViewResStaff;
        private bool _enableViewResVehicle;
        private bool _enableViewResDevice;
        private bool _enableViewResBeacon;
        

        public bool EnableViewResOrganization { get { return _enableViewResOrganization; } private set { _enableViewResOrganization = value; NotifyPropertyChanged("EnableViewResOrganization"); } }
        public bool EnableViewResGroup { get { return _enableViewResGroup; } private set { _enableViewResGroup = value; NotifyPropertyChanged("EnableViewResGroup"); } }
        public bool EnableViewResStaff { get { return _enableViewResStaff; } private set { _enableViewResStaff = value; NotifyPropertyChanged("EnableViewResStaff"); } }
        public bool EnableViewResVehicle { get { return _enableViewResVehicle; } private set { _enableViewResVehicle = value; NotifyPropertyChanged("EnableViewResVehicle"); } }
        public bool EnableViewResDevice { get { return _enableViewResDevice; } private set { _enableViewResDevice = value; NotifyPropertyChanged("EnableViewResDevice"); } }
        public bool EnableViewResBeacon { get { return _enableViewResBeacon; } private set { _enableViewResBeacon = value; NotifyPropertyChanged("EnableViewResBeacon"); } }
        
        private void InitilizeViewRes()
        {
            EnableViewResOrganization = FunctionConfigure.EnableViewResOrganization;
            EnableViewResGroup = FunctionConfigure.EnableViewResGroup;
            EnableViewResStaff = FunctionConfigure.EnableViewResStaff;
            EnableViewResVehicle = FunctionConfigure.EnableViewResVehicle;
            EnableViewResDevice = FunctionConfigure.EnableViewResDevice;
            EnableViewResBeacon = FunctionConfigure.EnableViewResBeacon;
        }

        public ICommand ViewRes { get { return new Command(ViewResExec); } }

        private void ViewResExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                ResourceKey_t key = ((string)parameter).ToEnum<ResourceKey_t>();
                bool isopen = true;

                switch (key)
                {
                    case ResourceKey_t.Organization:
                        EnableViewResOrganization = !EnableViewResOrganization;
                        isopen = EnableViewResOrganization;
                        break;
                    case ResourceKey_t.Group:
                        EnableViewResGroup = !EnableViewResGroup;
                        isopen = EnableViewResGroup;
                        break;
                    case ResourceKey_t.Staff:
                        EnableViewResStaff = !EnableViewResStaff;
                        isopen = EnableViewResStaff;
                        break;
                    case ResourceKey_t.Vehicle:
                        EnableViewResVehicle = !EnableViewResVehicle;
                        isopen = EnableViewResVehicle;
                        break;
                    case ResourceKey_t.Device:
                        EnableViewResDevice = !EnableViewResDevice;
                        isopen = EnableViewResDevice;
                        break;
                    case ResourceKey_t.Beacon:
                        EnableViewResBeacon = !EnableViewResBeacon;
                        isopen = EnableViewResBeacon;
                        break;
                    default: return;
                }

                if (OnOperated != null) OnOperated(new OperatedEventArgs(isopen ? OperateType_t.OpenResource : OperateType_t.CloseResource, key));
            }
        }

        //Navigation
        private  bool _enableViewNavigationSchedule;
        private bool _enableViewNavigationLocation;
        private bool _enableViewNavigationLocationInDoor;
        private bool _enableViewNavigationRecord;
        private bool _enableViewNavigationJobTicket;
        private bool _enableViewNavigationPatrol;
        private bool _enableViewNavigationReport;


        public bool EnableViewNavigationSchedule { get { return _enableViewNavigationSchedule; } private set { _enableViewNavigationSchedule = value; NotifyPropertyChanged("EnableViewNavigationSchedule"); } }
        public bool EnableViewNavigationLocation { get { return _enableViewNavigationLocation; } private set { _enableViewNavigationLocation = value; NotifyPropertyChanged("EnableViewNavigationLocation"); } }
        public bool EnableViewNavigationLocationInDoor { get { return _enableViewNavigationLocationInDoor; } private set { _enableViewNavigationLocationInDoor = value; NotifyPropertyChanged("EnableViewNavigationLocationInDoor"); } }
        public bool EnableViewNavigationRecord { get { return _enableViewNavigationRecord; } private set { _enableViewNavigationRecord = value; NotifyPropertyChanged("EnableViewNavigationRecord"); } }
        public bool EnableViewNavigationJobTicket { get { return _enableViewNavigationJobTicket; } private set { _enableViewNavigationJobTicket = value; NotifyPropertyChanged("EnableViewNavigationJobTicket"); } }
        public bool EnableViewNavigationPatrol { get { return _enableViewNavigationPatrol; } private set { _enableViewNavigationPatrol = value; NotifyPropertyChanged("EnableViewNavigationPatrol"); } }
        public bool EnableViewNavigationReport { get { return _enableViewNavigationReport; } private set { _enableViewNavigationReport = value; NotifyPropertyChanged("EnableViewNavigationReport"); } }

        private void InitilizeViewNavigation()
        {
            EnableViewNavigationSchedule = FunctionConfigure.EnableViewNavigationSchedule;
            EnableViewNavigationLocation = FunctionConfigure.EnableViewNavigationLocation;
            EnableViewNavigationLocationInDoor = FunctionConfigure.EnableViewNavigationLocationInDoor;
            EnableViewNavigationRecord = FunctionConfigure.EnableViewNavigationRecord;
            EnableViewNavigationJobTicket = FunctionConfigure.EnableViewNavigationJobTicket;
            EnableViewNavigationPatrol = FunctionConfigure.EnableViewNavigationPatrol;
            EnableViewNavigationReport = FunctionConfigure.EnableViewNavigationReport;
        }

        public ICommand ViewNavigation { get { return new Command(ViewNavigationExec); } }

        private void ViewNavigationExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                NavigationKey_t key = ((string)parameter).ToEnum<NavigationKey_t>();
                bool isopen = true;

                switch (key)
                {
                    case NavigationKey_t.Schedule:
                        EnableViewNavigationSchedule = !EnableViewNavigationSchedule;
                        isopen = EnableViewNavigationSchedule;
                        break;
                    case NavigationKey_t.Location:
                        EnableViewNavigationLocation = !EnableViewNavigationLocation;
                        isopen = EnableViewNavigationLocation;
                        break;
                    case NavigationKey_t.LocationInDoor:
                        EnableViewNavigationLocationInDoor = !EnableViewNavigationLocationInDoor;
                        isopen = EnableViewNavigationLocationInDoor;
                        break;
                    case NavigationKey_t.Record:
                        EnableViewNavigationRecord = !EnableViewNavigationRecord;
                        isopen = EnableViewNavigationRecord;
                        break;
                    case NavigationKey_t.JobTicket:
                        EnableViewNavigationJobTicket = !EnableViewNavigationJobTicket;
                        isopen = EnableViewNavigationJobTicket;
                        break;
                    case NavigationKey_t.Patrol:
                        EnableViewNavigationPatrol = !EnableViewNavigationPatrol;
                        isopen = EnableViewNavigationPatrol;
                        break;
                    case NavigationKey_t.Report:
                        EnableViewNavigationReport = !EnableViewNavigationReport;
                        isopen = EnableViewNavigationReport;
                        break;
                    default: return;
                }

                if (OnOperated != null) OnOperated(new OperatedEventArgs(isopen ? OperateType_t.OpenNavigation : OperateType_t.CloseNavigation, key));
            }
        }

        //ViewEvent
        public bool _enableViewEvent = FunctionConfigure.EnableViewEvent;
        public bool EnableViewEvent { get { return _enableViewEvent; } private set { _enableViewEvent = value; NotifyPropertyChanged("EnableViewEvent"); } }

         public ICommand ViewEvent { get { return new Command(ViewEventExec); } }

         private void ViewEventExec()
         {
             EnableViewEvent = !EnableViewEvent;
             if (OnOperated != null) OnOperated(new OperatedEventArgs(EnableViewEvent ? OperateType_t.OpenEvent : OperateType_t.CloseEvent));
         }


         private bool _enableViewNotifyAlarm;
         private bool _enableViewNotifyCalled;
         private bool _enableViewNotifyShortMessage;
         private bool _enableViewNotifyJobTicket;
         private bool _enableViewNotifyPatrol;

         public bool EnableViewNotifyAlarm { get { return _enableViewNotifyAlarm; } private set { _enableViewNotifyAlarm = value; NotifyPropertyChanged("EnableViewNotifyAlarm"); } }
         public bool EnableViewNotifyCalled { get { return _enableViewNotifyCalled; } private set { _enableViewNotifyCalled = value; NotifyPropertyChanged("EnableViewNotifyCalled"); } }
         public bool EnableViewNotifyShortMessage { get { return _enableViewNotifyShortMessage; } private set { _enableViewNotifyShortMessage = value; NotifyPropertyChanged("EnableViewNotifyShortMessage"); } }
         public bool EnableViewNotifyJobTicket { get { return _enableViewNotifyJobTicket; } private set { _enableViewNotifyJobTicket = value; NotifyPropertyChanged("EnableViewNotifyJobTicket"); } }
         public bool EnableViewNotifyPatrol { get { return _enableViewNotifyPatrol; } private set { _enableViewNotifyPatrol = value; NotifyPropertyChanged("EnableViewNotifyPatrol"); } }

         private void InitilizeViewNotify()
         {
             EnableViewNotifyAlarm = FunctionConfigure.EnableViewNotifyAlarm;
             EnableViewNotifyCalled = FunctionConfigure.EnableViewNotifyCalled;
             EnableViewNotifyShortMessage = FunctionConfigure.EnableViewNotifyShortMessage;
             EnableViewNotifyJobTicket = FunctionConfigure.EnableViewNotifyJobTicket;
             EnableViewNotifyPatrol = FunctionConfigure.EnableViewNotifyPatrol;
         }

         public ICommand ViewNotify { get { return new Command(ViewNotifyExec); } }

         private void ViewNotifyExec(object parameter)
         {
             if (parameter != null && parameter is string)
             {
                 NotifyKey_t key = ((string)parameter).ToEnum<NotifyKey_t>();
                 bool isopen = true;

                 switch (key)
                 {
                     case NotifyKey_t.Alarm:
                         EnableViewNotifyAlarm = !EnableViewNotifyAlarm;
                         isopen = EnableViewNotifyAlarm;
                         break;
                     case NotifyKey_t.Called:
                         EnableViewNotifyCalled = !EnableViewNotifyCalled;
                         isopen = EnableViewNotifyCalled;
                         break;
                     case NotifyKey_t.ShortMessage:
                         EnableViewNotifyShortMessage = !EnableViewNotifyShortMessage;
                         isopen = EnableViewNotifyShortMessage;
                         break;
                     case NotifyKey_t.JobTicket:
                         EnableViewNotifyJobTicket = !EnableViewNotifyJobTicket;
                         isopen = EnableViewNotifyJobTicket;
                         break;
                     case NotifyKey_t.Patrol:
                         EnableViewNotifyPatrol = !EnableViewNotifyPatrol;
                         isopen = EnableViewNotifyPatrol;
                         break;                    
                     default: return;
                 }

                 if (OnOperated != null) OnOperated(new OperatedEventArgs(isopen ? OperateType_t.OpenNotify : OperateType_t.CloseNotify, key));
             }
         }

         public ICommand UpdateNotifyOption { get { return new Command(UpdateNotifyOptionExec);} }
        private void UpdateNotifyOptionExec(object parameter)
        {
            if(parameter == null ||!(parameter is OperatedEventArgs))return;
            OperatedEventArgs args = parameter as OperatedEventArgs;
            switch ((NotifyKey_t)args.parameter)
                 {
                     case NotifyKey_t.Alarm:
                         EnableViewNotifyAlarm = args.Operate == OperateType_t.OpenNotify ? true:false;
                         break;
                     case NotifyKey_t.Called:
                         EnableViewNotifyCalled  = args.Operate == OperateType_t.OpenNotify ? true:false;
                         break;
                     case NotifyKey_t.ShortMessage:
                         EnableViewNotifyShortMessage = args.Operate == OperateType_t.OpenNotify ? true:false;
                         break;
                     case NotifyKey_t.JobTicket:
                         EnableViewNotifyJobTicket = args.Operate == OperateType_t.OpenNotify ? true:false;
                         break;
                     case NotifyKey_t.Patrol:
                         EnableViewNotifyPatrol = args.Operate == OperateType_t.OpenNotify ? true:false;
                         break;                    
                     default: return;
                 }
        }

        //Target
         private VMTarget _targetviewmodule;
         public VMTarget TargetViewModule { get { return _targetviewmodule; } private set { _targetviewmodule = value; NotifyPropertyChanged("TargetViewModule"); NotifyPropertyChanged("TagretVisible"); } }
         public Visibility TagretVisible { get { return _targetviewmodule == null ? Visibility.Collapsed : Visibility.Visible; } }

         public ICommand SetTarget { get { return new Command(SetTargetExec); } }
         public void SetTargetExec(object parameter)
         {
             TargetViewModule = parameter as VMTarget;
         }

         //Tools
         public ICommand ToolsFast { get { return new Command(ToolsFastExec); } }
         private void ToolsFastExec(object parameter)
         {
             if (parameter != null && parameter is string)
             {
                 QuickPanelType_t key = ((string)parameter).ToEnum<QuickPanelType_t>();
                 if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenNewFastWindow, key));
             };
         }

        //Help

         public ICommand Help { get { return new Command(HelpExec); } }
         private void HelpExec(object parameter)
         {
             if (parameter != null && parameter is string)
             {
                 HelpWindowType_t key = ((string)parameter).ToEnum<HelpWindowType_t>();
                 if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenHelpWindow, key));
             };
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

        #endregion INotifyPropertyChanged Members
    }
}