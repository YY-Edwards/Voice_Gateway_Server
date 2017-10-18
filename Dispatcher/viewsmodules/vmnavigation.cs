using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using Sigmar.Logger;
using Sigmar.Extension;

using Dispatcher.Service;


namespace Dispatcher.ViewsModules
{
    public class VMNavigation : INotifyPropertyChanged
    {
        public event OperatedEventHandler OnOperated;

        public VMNavigation()
        {
            InitilizeViewNavigation();
        }

        private bool _enableViewNavigationSchedule;
        private bool _enableViewNavigationLocation;
        private bool _enableViewNavigationLocationInDoor;
        private bool _enableViewNavigationRecord;
        private bool _enableViewNavigationJobTicket;
        private bool _enableViewNavigationPatrol;
        private bool _enableViewNavigationReport;

        public Visibility ViewNavigationScheduleVisible { get { return _enableViewNavigationSchedule ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationLocationVisible { get { return _enableViewNavigationLocation ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationLocationInDoorVisible { get { return _enableViewNavigationLocationInDoor ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationRecordVisible { get { return _enableViewNavigationRecord ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationJobTicketVisible { get { return _enableViewNavigationJobTicket ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationPatrolVisible { get { return _enableViewNavigationPatrol ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewNavigationReportVisible { get { return _enableViewNavigationReport ? Visibility.Visible : Visibility.Collapsed; } }

        private void InitilizeViewNavigation()
        {
            _enableViewNavigationSchedule = FunctionConfigure.EnableViewNavigationSchedule;
            _enableViewNavigationLocation = FunctionConfigure.EnableViewNavigationLocation;
            _enableViewNavigationLocationInDoor = FunctionConfigure.EnableViewNavigationLocationInDoor;
            _enableViewNavigationRecord = FunctionConfigure.EnableViewNavigationRecord;
            _enableViewNavigationJobTicket = FunctionConfigure.EnableViewNavigationJobTicket;
            _enableViewNavigationPatrol = FunctionConfigure.EnableViewNavigationPatrol;
            _enableViewNavigationReport = FunctionConfigure.EnableViewNavigationReport;

            NotifyPropertyChanged("ViewNavigationScheduleVisible");
            NotifyPropertyChanged("ViewNavigationLocationVisible");
            NotifyPropertyChanged("ViewNavigationLocationInDoorVisible");
            NotifyPropertyChanged("ViewNavigationRecordVisible");
            NotifyPropertyChanged("ViewNavigationJobTicketVisible");
            NotifyPropertyChanged("ViewNavigationPatrolVisible");
            NotifyPropertyChanged("ViewNavigationReportVisible");
        }

        public void UpdateView(OperatedEventArgs e)
        {
            try
            {
                switch ((NavigationKey_t)e.parameter)
                {
                    case NavigationKey_t.Schedule:
                        _enableViewNavigationSchedule = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationScheduleVisible");
                        break;
                    case NavigationKey_t.Location:
                        _enableViewNavigationLocation = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationLocationVisible");
                        break;
                    case NavigationKey_t.LocationInDoor:
                        _enableViewNavigationLocationInDoor = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationLocationInDoorVisible");
                        break;
                    case NavigationKey_t.Record:
                        _enableViewNavigationRecord = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationRecordVisible");
                        break;
                    case NavigationKey_t.JobTicket:
                        _enableViewNavigationJobTicket = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationJobTicketVisible");
                        break;
                    case NavigationKey_t.Patrol:
                        _enableViewNavigationPatrol = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationPatrolVisible");
                        break;
                    case NavigationKey_t.Report:
                        _enableViewNavigationReport = e.Operate == OperateType_t.OpenNavigation ? true : false;
                        NotifyPropertyChanged("ViewNavigationReportVisible");
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

        public ICommand Work { get { return new Command(WorkExec); } }

        private void WorkExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                NavigationKey_t key = ((string)parameter).ToEnum<NavigationKey_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.NavigationWork, key));
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
