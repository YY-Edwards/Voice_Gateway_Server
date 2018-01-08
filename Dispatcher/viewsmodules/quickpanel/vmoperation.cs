using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;

using Sigmar;
using Dispatcher;
using Dispatcher.Service;
using Dispatcher.Modules;

namespace Dispatcher.ViewsModules
{
    public class VMOperation :INotifyPropertyChanged
    {
        private List<VMTarget> _targets;
        private COperation _operation;

        public List<VMTarget> Targets { get { 
            return _targets; }
            set { 
                _targets = value;
                NotifyPropertyChanged("Targets");
                NotifyPropertyChanged("EnableCallOrStop");
                NotifyPropertyChanged("IsEnable");
                NotifyPropertyChanged("OnlineOpacity");
            }
        }
        public COperation Operation { get { return _operation; } }

        public ICollectionView AllTarget { get { return new ListCollectionView(ResourcesMgr.Instance().All); } }
        public bool IsMultiple { get { return _operation.Type == TaskType_t.Schedule ? false:true;} }

        public int TypeIndex { get { return (int)_operation.Type;} }
        public VMOperation(COperation operation, List<VMTarget> targets = null)
        {
            _operation = operation;
            _targets = targets;
            if (_targets == null) _targets = new List<VMTarget>();

            VMTarget.StaticPropertyChanged += OnTargetStatusChanged;
            SystemStatus.PropertyChanged += OnTargetStatusChanged;
        }

        public VMOperation()
        {
            _operation = new COperation(TaskType_t.Schedule);
            if (_targets == null) _targets = new List<VMTarget>();
            VMTarget.StaticPropertyChanged += OnTargetStatusChanged;
            SystemStatus.PropertyChanged += OnTargetStatusChanged;
        }

        public int TaskTypeIndex
        {
            get { return (int)_operation.Type; }
            set
            {
                _operation.Type = (TaskType_t)value;

                switch (_operation.Type)
                {
                    case TaskType_t.Schedule: _operation.Args = new CallArgs(); break;
                    case TaskType_t.ShortMessage: _operation.Args = new ShortMessageArgs(); break;
                    case TaskType_t.Controler: _operation.Args = new ControlArgs(); break;
                    case TaskType_t.Location: _operation.Args = new LocationArgs(); break;
                    case TaskType_t.LocationInDoor: _operation.Args = new LocationInDoorArgs(); break;
                    //case TaskType_t.JobTicket: _operation.Args = new CallArgs(); break;
                    //case TaskType_t.Patrol: _operation.Args = new CallArgs(); break;
                }

            }
        }

        public ImageSource Icon
        {
            get
            {
                if (_operation != null)
                {
                    switch (_operation.Type)
                    {
                        case TaskType_t.Schedule:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/call_33_34.png"));
                        case TaskType_t.ShortMessage:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/message_34_34.png"));
                        case TaskType_t.Controler:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/control_43_43.png"));
                        case TaskType_t.Location:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/positon_29_38.png"));
                        case TaskType_t.LocationInDoor:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/locationindoor_32_32.png"));
                        case TaskType_t.JobTicket:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/job_tickets_29_36.png"));
                        case TaskType_t.Patrol:
                            return null;
                        default: return null;
                    }
                }
                else return null;
            }
        }
     
        public string Name { get { return _operation != null ? _operation.Name : ""; } }
        public string TargetName 
        {
            get
            {
                if (_targets == null) return "";
                string name = "";
                for (int i = 0; i < _targets.Count; i++)
                {
                    if(i == 0)
                    {
                        name += _targets[i].Name;
                    }                    
                    else if (i >= 2)
                    {
                        name += "...";
                        break;
                    }
                    else
                    {
                        name += ", " + _targets[i].Name;
                    }
                    
                }

                return name;
            }
        }

        public string FullName { get { return Name + "->" + TargetName; } }

        private void OnTargetStatusChanged(object sender, PropertyChangedEventArgs e)
        {
            NotifyPropertyChanged("EnableCallOrStop");
            NotifyPropertyChanged("IsEnable");
            NotifyPropertyChanged("OnlineOpacity"); 
        }

        public double OnlineOpacity
        {
            get { return IsEnable ? 1 : 0.5; }
        }

        public bool EnableCallOrStop
        {
            get
            {
                if (_operation != null && _operation.Type == TaskType_t.Schedule && _targets != null && _targets.Count >= 1)
                {
                    if (_targets[0].IsInCall) return true;
                    else return !SystemStatus.IsSystemInCall && _targets[0].IsOnlineIfNeed && !_targets[0].IsInCall ? true : false;
                }
                else return false;               
            }
        }

        public bool IsEnable
        {
            get
            {
                if (_operation != null)
                {
                    switch (_operation.Type)
                    {
                        case TaskType_t.Schedule:
                            if (_targets != null && _targets.Find(p => p.CanCall) != null) return true;
                            else break;
                        case TaskType_t.ShortMessage:
                            if (_targets != null && _targets.Find(p => p.CanShortMessage) != null) return true;
                            else break;
                        case TaskType_t.Controler:
                            if (IsEnableControl()) return true;
                            else break;
                        case TaskType_t.Location:
                            if (IsEnableLocation()) return true;
                            else break;
                        case TaskType_t.LocationInDoor:
                            if (IsEnableLocationInDoor()) return true;
                            else break;
                    }
                }
                return false;
            }
        }

        private bool IsEnableControl()
        {
            if (_operation.Args == null) return false;
            switch ((_operation.Args as ControlArgs).Type)
            {
                case ControlerType_t.Check:
                    if (_targets != null && _targets.Find(p => p.CanCheckOnline) != null) return true;
                    else break;
                case ControlerType_t.Monitor:
                    if (_targets != null && _targets.Find(p => p.CanMonitor) != null) return true;
                    else break;
                case ControlerType_t.ShutDown:
                    if (_targets != null && _targets.Find(p => p.CanShutDown) != null) return true;
                    else break;
                case ControlerType_t.StartUp:
                    if (_targets != null && _targets.Find(p => p.CanShartUp) != null) return true;
                    else break;
            }
            return false;
        }

        private bool IsEnableLocation()
        {
            if (_operation.Args == null) return false;
            switch ((_operation.Args as LocationArgs).Type)
            {
                case LocationType_t.Immediate:
                case LocationType_t.StartTriggered:
                    if (_targets != null && _targets.Find(p => p.CanLocation) != null) return true;
                    else break;
                case LocationType_t.StopTriggered:
                    if (_targets != null && _targets.Find(p => p.CanStopLocation) != null) return true;
                    else break;
                case LocationType_t.Track:
                    return true;
            }
            return false;
        }

        private bool IsEnableLocationInDoor()
        {
            if (_operation.Args == null) return false;
            switch ((_operation.Args as LocationInDoorArgs).Type)
            {
                case LocationType_t.StartTriggered:
                     if (_targets != null && _targets.Find(p => p.CanLocationInDoor) != null) return true;
                    else break;
                case LocationType_t.StopTriggered:
                     if (_targets != null && _targets.Find(p => p.CanStopLocationInDoor) != null) return true;
                    else break;
            }
            return false;
        }

        public string ShortMessageContents
        {
            get
            {
                if (_operation == null || _operation.Args == null || !(_operation.Args is ShortMessageArgs)) return "";
                else return (_operation.Args as ShortMessageArgs).Contents;
            }
            set
            {
                if (_operation != null && _operation.Args != null) (_operation.Args as ShortMessageArgs).Contents = value;
            }
        }

        public int ControlTypeIndex 
        {
            get
            {
                if (_operation == null || _operation.Args == null || !(_operation.Args is ControlArgs)) return -1;
                else return (int)(_operation.Args as ControlArgs).Type;
            }
            set
            {
                if (_operation != null && _operation.Args != null) (_operation.Args as ControlArgs).Type = (ControlerType_t)value;
                NotifyPropertyChanged("EnableCallOrStop");
                NotifyPropertyChanged("IsEnable");
                NotifyPropertyChanged("OnlineOpacity"); 
            }
        }

        public int LocationTypeIndex
        {
            get
            {
                if (_operation == null || _operation.Args == null || !(_operation.Args is LocationArgs)) return -1;
                else return (int)(_operation.Args as LocationArgs).Type;
            }
            set
            {
                if (_operation != null && _operation.Args != null) (_operation.Args as LocationArgs).Type = (LocationType_t)value;
                NotifyPropertyChanged("EnableCallOrStop");
                NotifyPropertyChanged("IsEnable");
                NotifyPropertyChanged("OnlineOpacity");
            }
        }

        public double LocationCycle
        {
            get
            {
                if (_operation == null || _operation.Args == null || !(_operation.Args is LocationArgs)) return 0;
                else return (_operation.Args as LocationArgs).Cycle;
            }
            set
            {
                if (_operation != null && _operation.Args != null) (_operation.Args as LocationArgs).Cycle = value;
            }
        }

        public int LocationInDoorTypeIndex
        {
            get
            {
                if (_operation == null || _operation.Args == null || !(_operation.Args is LocationInDoorArgs)) return -1;
                else return (int)(_operation.Args as LocationInDoorArgs).Type;
            }
            set
            {
                if (_operation != null && _operation.Args != null) (_operation.Args as LocationInDoorArgs).Type = (LocationType_t)value;
                NotifyPropertyChanged("EnableCallOrStop");
                NotifyPropertyChanged("IsEnable");
                NotifyPropertyChanged("OnlineOpacity");
            }
        }


        public ICommand Execute { get { return new Command(ExecuteExec); } }
        private void ExecuteExec()
        {
            if (_operation != null &&  _targets != null && _targets.Count >= 1)
            {
                switch (_operation.Type)
                {
                    case TaskType_t.Schedule:
                        if (_targets[0].CanCall) _targets[0].Call.Execute(CallOperatedType_t.Start);
                        break;
                    case TaskType_t.ShortMessage:
                        foreach (VMTarget target in _targets) if(target.CanShortMessage)target.SendShortMessage.Execute(_operation.Args);
                        break;
                    case TaskType_t.Controler:
                        switch ((_operation.Args as ControlArgs).Type)
                        {
                            case ControlerType_t.Check:
                                foreach (VMTarget target in _targets) if (target.CanCheckOnline) target.Control.Execute(_operation.Args);
                                break;
                            case ControlerType_t.Monitor:
                                 foreach (VMTarget target in _targets) if (target.CanMonitor) target.Control.Execute(_operation.Args);
                                break;
                            case ControlerType_t.ShutDown:
                                foreach (VMTarget target in _targets) if (target.CanShutDown) target.Control.Execute(_operation.Args);
                                break;
                            case ControlerType_t.StartUp:
                                foreach (VMTarget target in _targets) if (target.CanShartUp) target.Control.Execute(_operation.Args);
                                break;
                        }                      
                        break;
                    case TaskType_t.Location:
                        switch ((_operation.Args as LocationArgs).Type)
                        {
                            case LocationType_t.Immediate:
                            case LocationType_t.StartTriggered:
                                foreach (VMTarget target in _targets) if (target.CanLocation) target.Location.Execute(_operation.Args);                                
                                break;
                            case LocationType_t.StopTriggered:
                                foreach (VMTarget target in _targets) if (target.CanStopLocation) target.Location.Execute(_operation.Args);      
                                break;
                        }
                        break;
                    case TaskType_t.LocationInDoor:
                        switch ((_operation.Args as LocationInDoorArgs).Type)
                        {
                            case LocationType_t.StartTriggered:
                                foreach (VMTarget target in _targets) if (target.CanLocationInDoor) target.LocationInDoor.Execute(_operation.Args);    
                                break;
                            case LocationType_t.StopTriggered:
                                foreach (VMTarget target in _targets) if (target.CanStopLocationInDoor) target.LocationInDoor.Execute(_operation.Args);    
                                break;
                        }                       
                        break;
                    case TaskType_t.JobTicket:                     
                    case TaskType_t.Patrol:
                        break;

                }            
            }
        }
        public ICommand Abort { get { return new Command(AbortExec); } }
        private void AbortExec()
        {
            if (_operation != null)
            {
                switch (_operation.Type)
                {
                    case TaskType_t.Schedule:
                       if(_targets != null && _targets.Count >= 1)
                       {
                           if (_targets[0].CanStopCall) _targets[0].Call.Execute(CallOperatedType_t.Stop);
                       }
                       break;                  
                }
               
            }
        }

        public bool Equal(VMOperation dest)
        {
            if (dest == null) return false;
            if (!Operation.Equal(dest.Operation)) return false;
            if (Targets.Count != dest.Targets.Count) return false;
            else
            {
                if (Targets.FindAll(p => dest.Targets.Contains(p)).Count == Targets.Count) return true;
                else return false;
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
