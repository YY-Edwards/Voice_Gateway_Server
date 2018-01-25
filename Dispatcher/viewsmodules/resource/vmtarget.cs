using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Threading.Tasks;

using Dispatcher.Modules;
using Dispatcher.Service;
using Dispatcher;
using Sigmar;
using Sigmar.Extension;

namespace Dispatcher.ViewsModules
{
    public class VMTarget : INotifyPropertyChanged
    {
        private string _messageBeginFormat = "{0}  {1}。";
        private string _messageFailureFormat = "{0}失败  {1}。";

        private CDispatcher _dispatcher; 

        private CGroup _group = null;
        private bool _iscurrentrx = false;
        //private List<BeaconReport> _lastbeacons;
        private CMember _member = null;
        private TargetType_t? _type = null;
        public VMTarget(CMember member)
        {
            _member = member;
            _type = TargetType_t.Member;
            InitializeDispatcher();
        }

        public VMTarget(CGroup group)
        {
            _group = group;
            _type = TargetType_t.Group;
            InitializeDispatcher();
        }

        public event OperatedEventHandler OnOperated;

        public enum TargetType_t
        {
            Member,
            Group,
        }

        public ICommand AddFastPanel { get { return new Command(AddFastPanelExec); } }
        public ImageSource BigIcon
        {
            get
            {
                try
                {
                    if (_type == TargetType_t.Member && _member != null)
                    {
                        switch (_member.MemberType)
                        {
                            case CMember.MemberType_t.Staff:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_32_32_on.png"));
                            case CMember.MemberType_t.Vehicle:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_35_29_on.png"));
                            case CMember.MemberType_t.Handset:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_23_47_on.png"));
                            case CMember.MemberType_t.VehicleStation:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_34_40_on.png"));
                            default:
                                return null;
                        }
                    }
                    else if (_type == TargetType_t.Group) return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_37_32_on.png"));
                    else return null;
                }
                catch
                {
                    return null;
                }
            }
        }

        public string ByName { get { return _type == TargetType_t.Group && _group != null && _group.IsAllTarget ? _group.ByName : FullName; } }
        public ICommand Call { get { return new Command(CallExec); } }
        public bool CanCall
        {
            get
            {
                return ServerStatus.Instance().VoiceBusiness.IsConnected && !SystemStatus.IsSystemInCall && IsOnlineIfNeed && !IsInCall ? true : false;
            }
        }

        public bool CanCheckOnline { get { return ServerStatus.Instance().DataBusiness.IsConnected && _type == TargetType_t.Member && _member != null && _member.HasDevice ? true : false; } }
        public bool CanLocation
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return _member != null && _member.HasDevice && IsOnlineIfNeed && !IsInLocation && _member.HasLocation ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool CanLocationInDoor
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return _member != null && _member.HasDevice && IsOnlineIfNeed && !IsInLocationInDoor && _member.HasLocatinInDoor ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool CanMonitor { get { return ServerStatus.Instance().DataBusiness.IsConnected && _type == TargetType_t.Member && _member != null && _member.HasDevice && IsOnlineIfNeed ? true : false; } }
        public bool CanShartUp { get { return ServerStatus.Instance().DataBusiness.IsConnected && _type == TargetType_t.Member && _member != null && _member.HasDevice && IsOnlineIfNeed && IsShutDown ? true : false; } }
        public bool CanShortMessage
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return _member != null && _member.HasDevice && IsOnlineIfNeed && _member.HasScreen ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool CanShutDown { get { return ServerStatus.Instance().DataBusiness.IsConnected && _type == TargetType_t.Member && _member != null && _member.HasDevice && IsOnlineIfNeed && !IsShutDown ? true : false; } }
        public bool CanStopCall
        {
            get
            {
                return ServerStatus.Instance().VoiceBusiness.IsConnected && SystemStatus.IsSystemInCall && IsOnlineIfNeed && IsInCall ? true : false;
            }
        }

        public bool CanStopLocation
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return IsOnlineIfNeed && IsInLocation ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool CanStopLocationInDoor
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return IsOnlineIfNeed && IsInLocationInDoor ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public ICommand ChangeValue { get { return new Command(ChangeValueExec); } }
        public Visibility ContextMenuVisible { get { return _type == TargetType_t.Group && _group != null && _group.GroupID <= 0 ? Visibility.Collapsed : Visibility.Visible; } }
        public ICommand Control { get { return new Command(ControlExec); } }
        public string DeviceName { get { return _type == TargetType_t.Member && _member != null ? _member.DeviceName : ""; } }
        //can operate
        public bool EnableCallOrStop
        {
            get
            {
                if(IsInCall) return true;
                else return ServerStatus.Instance().VoiceBusiness.IsConnected &&  !SystemStatus.IsSystemInCall && IsOnlineIfNeed && !IsInCall ? true : false;
            }
        }

        public bool EnableControlFunc
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member && _member != null) return true;
                else return false;
            }
        }

        public bool EnableLocationFunc
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return _member != null && IsOnlineIfNeed && _member.HasLocation ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool EnableLocationInDoorFunc
        {
            get
            {
                if (!ServerStatus.Instance().DataBusiness.IsConnected) return false;
                if (_type == TargetType_t.Member) return _member != null && IsOnlineIfNeed && _member.HasLocatinInDoor ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public string FullName
        {

            get
            {
                if (_type == TargetType_t.Member) return _member != null ? _member.FullName : "";
                else if (_type == TargetType_t.Group)
                {
                    if (_group == null) return "";
                    if (_group.GroupID <= 0) return _group.Name;
                    else return _group.Name + "(ID:" + _group.GroupID.ToString() + ")";
                }
                else return "";
            }
        }

        public CGroup Group { get { return _group; } }
        public ImageSource Icon
        {
            get
            {
                if (_type == TargetType_t.Member && _member != null)
                {
                    switch (_member.MemberType)
                    {
                        case CMember.MemberType_t.Staff:
                            return IsOnlineIfNeed
                                ? new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_18_18_on.png"))
                                : new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_18_18_off.png"));
                        case CMember.MemberType_t.Vehicle:
                            return IsOnlineIfNeed
                                ? new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_18_18_on.png"))
                                : new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_18_18_off.png"));
                        case CMember.MemberType_t.Handset:
                            return IsOnlineIfNeed
                                ? new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_18_18_on.png"))
                                : new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_18_18_off.png"));
                        case CMember.MemberType_t.VehicleStation:
                            return IsOnlineIfNeed
                                ? new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_18_18_on.png"))
                                : new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_18_18_off.png"));
                        default:
                            return null;
                    }
                }
                else if (_type == TargetType_t.Group) return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_18_18_on.png"));
                else return null;
            }
        }

        public long ID
        {
            get
            {
                if (_type == TargetType_t.Member) return _member != null ? _member.ID : 0;
                else if (_type == TargetType_t.Group) return _group != null ? _group.ID : 0;
                else return 0;
            }
        }

        public Visibility InCallStatusVisible { get { return IsOnlineIfNeed && IsInCall ? Visibility.Visible : Visibility.Collapsed; } }
        public bool IsCurrentRx { get { return _iscurrentrx; } set { _iscurrentrx = value; NotifyPropertyChanged("IsCurrentRx"); } }
        public bool IsInCall
        {
            get
            {
                if (_type == TargetType_t.Member) return _member != null && _member.CallStatus != CallStatus_t.Idle ? true : false;
                else if (_type == TargetType_t.Group) return _group != null && _group.CallStatus != CallStatus_t.Idle ? true : false;
                else return false;
            }
        }

        public bool IsInLocation { get { return _type == TargetType_t.Member && _member != null && _member.LocationGpsStatus != LocationStatus_t.Idle ? true : false; } }
        public bool IsInLocationInDoor { get { return _type == TargetType_t.Member && _member != null && _member.LocationInDoorStatus != LocationStatus_t.Idle ? true : false; } }

        public bool IsOnline
        {
            get
            {
                if (_type == TargetType_t.Member) return _member == null || !_member.HasDevice ? false : _member.IsOnline;
                else return true;
            }

            private set
            {
                if (_type == TargetType_t.Member && _member != null) _member.IsOnline = value; NotifyPropertyChanged("IsOnline");
            }
        }
        
        public bool IsOnlineIfNeed
        {
            get
            {
                return true;

                if (_type == TargetType_t.Member) return _member == null || !_member.HasDevice ? false : _member.IsOnline;
                else return true;
            }
        }

        //status
        public bool IsShutDown
        {
            get { return _type == TargetType_t.Member && _member != null ? _member.IsShutDown : false; }
            private set
            {
                if (_type == TargetType_t.Member && _member != null)
                {
                    _member.IsShutDown = value;
                    NotifyPropertyChanged("IsShutDown");
                    NotifyPropertyChanged("ShutDownStatusVisible");
                    NotifyPropertyChanged("CanShutDown");
                    NotifyPropertyChanged("CanShartUp");
                }
            }
        }

        public ICommand Location { get { return new Command(LocationExec); } }
        public ICommand LocationInDoor { get { return new Command(LocationInDoorExec); } }
        public Visibility LocationInDoorStatusVisible { get { return IsOnline && IsInLocationInDoor ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility LocationStatusVisible { get { return IsOnline && IsInLocation ? Visibility.Visible : Visibility.Collapsed; } }
        public CMember Member { get { return _member; } }
        public string Name
        {
            get
            {
                if (_type == TargetType_t.Member) return _member != null ? _member.Name : "";
                else if (_type == TargetType_t.Group)
                {
                    if (_group == null) return "";
                    else return _group.Name;
                }
                else return "";
            }
        }

        public double OnlineOpacity
        {
            get { return IsOnline ? 1 : 0.5; }
        }

        private bool _hasFailure = false;

        public void ClearFailure()
        {
            _hasFailure = false;
            NotifyPropertyChanged("FailureIconVisible");
        }

        private CDispatcher.Status _dispatcherStatus = CDispatcher.Status.Completed;

        public Visibility WaitIconVisible { get { return _notices.Contains(p=>p.DispatcherStatus == CDispatcher.Status.Begin) ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility FailureIconVisible {get{return _hasFailure ? Visibility.Visible : Visibility.Collapsed;}}
       

        public ICommand Abnormal { get {return  new Command(AbnormalExec); } }
        private void AbnormalExec()
        {
            int i = 0;
        }

        public ICommand OpenOperateWindow { get { return new Command(OpenOperateWindowExec); } }
        public ICommand PlayVoice { get { return new Command(PlayVoiceExec); } }
        public ICommand QueryLocation { get { return new Command(QueryLocationExec); } }
        //
        public int RadioID
        {
            get
            {
                if (_type == TargetType_t.Member) return _member != null ? _member.RadioID : 0;
                else return 0;
            }
        }

        public ICommand SendShortMessage { get { return new Command(SendShortMessageExec); } }
        public Visibility ShutDownStatusVisible { get { return IsOnline && IsShutDown ? Visibility.Visible : Visibility.Collapsed; } }
        public TargetType_t? TargetType { get { return _type; } }
        public void  InitializeDispatcher()
        {
            if (_dispatcher == null)
            {
                if (FunctionConfigure.WorkMode == FunctionConfigure.Mode_t.Repeater || FunctionConfigure.WorkMode == FunctionConfigure.Mode_t.RepeaterWithMnis)
                {
                    _dispatcher = RepeaterDispatcher.Instance();
                }
                else if (FunctionConfigure.WorkMode == FunctionConfigure.Mode_t.VehicleStation || FunctionConfigure.WorkMode == FunctionConfigure.Mode_t.VehicleStationWithMnis)
                {
                    _dispatcher = VehicleStationDispatcher.Instance();
                }
                else
                {
                    //for test
                    _dispatcher = VehicleStationDispatcher.Instance();
                }
           

                if (_dispatcher == null) return;

                _dispatcher.DispatcherBegin += new Action<CDispatcher.OperateContent_t>(OnDispatcherBegin);
                _dispatcher.DispatcherCompleted += new Action<CDispatcher.OperateContent_t>(OnDispatcherCompleted);
                _dispatcher.DispatcherFailure += new Action<CDispatcher.OperateContent_t, CDispatcher.Status>(OnDispatcherFailure);


                _dispatcher.CallResponse += new CallResponseHandler(OnCallResponse);
                _dispatcher.CallStatusChanged += new CallResponseHandler(OnCallStatusChanged);
                _dispatcher.CallRequest += new CallRequestHandler(OnCallRequest);

                _dispatcher.ShortMessageResponse += new ShortMessageResponseHandler(OnShortMessageResponse);
                _dispatcher.ShortMessageRequest += new ShortMessageRequestHandler(OnShortMessageRequest);

                _dispatcher.ArsRequest += new ArsRequestHandler(OnArsRequest);

                _dispatcher.ControlResponse += new ControlResponseHandler(OnControlResponse);
                _dispatcher.ControlResult += new ControlResultHandler(OnControlResult);

                _dispatcher.LocationResponse += new LocationResponseHandler(OnLocationResponse);
                _dispatcher.LocationReport += new LocationReportHandler(OnLocationReport);

                _dispatcher.LocationInDoorResponse += new LocationInDoorResponseHandler(OnLocationInDoorResponse);
                _dispatcher.LocationInDoorReport += new LocationInDoorReportHandler(OnLocationInDoorReport);
            }

            ServerStatus.Instance().StatusChanged += delegate { UpdateAllStatus(); };

            new Task(StatusCheckProcess).Start();
        }

        private long LastWaitTimeoutTicks = 0;
        private long LastProcessTicks = 0;
        private void StatusCheckProcess()
        {
            while(true)
            {
                if (LastWaitTimeoutTicks > LastProcessTicks && DateTime.Now.Ticks > LastWaitTimeoutTicks)
                {
                    LastProcessTicks = LastWaitTimeoutTicks;
                    _dispatcherStatus = CDispatcher.Status.Timeout;

                    lock (_notices)
                    {
                        if (_notices.Count(p => p.DispatcherStatus == CDispatcher.Status.Begin) > 0)
                        {
                            for(int i = 0; i < _notices.Count; ++i)
                            {
                                if(_notices[i].DispatcherStatus == CDispatcher.Status.Begin)
                                {
                                    _notices[i].DispatcherStatus = CDispatcher.Status.Timeout;
                                    NotifyPropertyChanged("INotices");
                                }
                            }

                            NotifyPropertyChanged("WaitIconVisible");
                            NotifyPropertyChanged("FailureIconVisible");
                        }
                    }                    
                }

                System.Threading.Thread.Sleep(500);
            }
        }

        private void OnDispatcherBegin(CDispatcher.OperateContent_t operate)
        {
            if (operate == null || operate.Parameter == null || !IsThis(operate.Parameter.TargetMode, operate.Parameter.TargetId)) return;

            _dispatcherStatus = CDispatcher.Status.Begin;
           
            if (operate.OperationName == null || operate.OperationName == string.Empty) return;

            Log.Message(string.Format(_messageBeginFormat, operate.OperationName, FullName));


            NotifyKey_t notifyType = NotifyKey_t.Alarm;

            switch (operate.Opcode)
            {
                case RequestOpcode.call: notifyType = NotifyKey_t.Called; break;
                case RequestOpcode.message: notifyType = NotifyKey_t.ShortMessage; break;
                case RequestOpcode.location:
                    LocationParameter param = operate.Parameter as LocationParameter;
                    if (param == null) return;
                    if (param.Type == QueryLocationType_t.LocationInDoor) notifyType = NotifyKey_t.LocationInDoor;
                    else notifyType = NotifyKey_t.Location;
                    break;
                case RequestOpcode.control: notifyType = NotifyKey_t.Control; break;
                case RequestOpcode.wlCall: notifyType = NotifyKey_t.Called; break;
                default: return;

            };

            CNotice notice = new CNotice()
            {
                Time = DateTime.Now,
                Contents = operate.OperationName,
                Type = notifyType,
            };

            lock (_notices)
            {
                if (!_notices.Contains(p => p.OperateSessionId == operate.Parameter.guid))
                {
                    _notices.Add(new VMNotify.VMNotice(operate.Parameter.guid, this, notice, true, CDispatcher.Status.Begin));

                    LastWaitTimeoutTicks = DateTime.Now.AddSeconds(FunctionConfigure.TimeoutSeconds).Ticks;
                    NotifyPropertyChanged("INotices");
                }
            }

            NotifyPropertyChanged("WaitIconVisible");
            NotifyPropertyChanged("FailureIconVisible");
        }
        private void OnDispatcherCompleted(CDispatcher.OperateContent_t operate)
        {
            if (operate == null || operate.Parameter == null || !IsThis(operate.Parameter.TargetMode, operate.Parameter.TargetId)) return;
            _dispatcherStatus = CDispatcher.Status.Completed;
           

            lock (_notices)
            {
                int index = _notices.FindIndex(p => p.OperateSessionId == operate.Parameter.guid);

                if (index >= 0 && index < _notices.Count)
                {
                    _notices[index].DispatcherStatus = CDispatcher.Status.Completed;
                    NotifyPropertyChanged("INotices");
                }
            }

            NotifyPropertyChanged("WaitIconVisible");
            NotifyPropertyChanged("FailureIconVisible");
        }
        private void OnDispatcherFailure(CDispatcher.OperateContent_t operate, CDispatcher.Status status)
        {
            if (operate == null || operate.Parameter == null || !IsThis(operate.Parameter.TargetMode, operate.Parameter.TargetId)) return;
            _dispatcherStatus = status;
            _hasFailure = true;

            Log.Message(string.Format(_messageFailureFormat, operate.OperationName, FullName));

            lock (_notices)
            {
                int index = _notices.FindIndex(p => p.OperateSessionId == operate.Parameter.guid);

                if (index >= 0 && index < _notices.Count)
                {
                    _notices[index].DispatcherStatus = status;
                    NotifyPropertyChanged("INotices");
                }
            }

            RecoverStatus(operate);

            NotifyPropertyChanged("WaitIconVisible");
            NotifyPropertyChanged("FailureIconVisible");
        }


        private void RecoverStatus(CDispatcher.OperateContent_t operate)
        {
            switch (operate.Opcode)
            {
                case RequestOpcode.call:
                    CallParameter callParam = operate.Parameter as CallParameter;
                    if(callParam.Operate == ExecType_t.Start) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                    else if (callParam.Operate == ExecType_t.Stop) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                    break;
                case RequestOpcode.wlCall:
                    RepeaterCallParameter repeatercallParam = operate.Parameter as RepeaterCallParameter;
                    if (repeatercallParam.operate == ExecType_t.Start) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                    else if (repeatercallParam.operate == ExecType_t.Stop) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                    break;
                case RequestOpcode.message:
                    ShortMessageParameter shortMessageParam = operate.Parameter as ShortMessageParameter;
                    break;
                case RequestOpcode.location:
                    LocationParameter locationParam = operate.Parameter as LocationParameter;
                    if (locationParam != null)
                    {
                        switch (locationParam.Type)
                        {
                            case QueryLocationType_t.LocationGps:
                                if (locationParam.Operate == LocationType_t.Immediate) { }
                                else if (locationParam.Operate == LocationType_t.StartTriggered) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Idle));
                                else if (locationParam.Operate == LocationType_t.StopTriggered) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Cycle));
                                break;
                            case QueryLocationType_t.LocationInDoor:
                                if (locationParam.Operate == LocationType_t.Immediate) { }
                                else if (locationParam.Operate == LocationType_t.StartTriggered) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Idle));
                                else if (locationParam.Operate == LocationType_t.StopTriggered) ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Cycle));
                                break;
                        }
                    }
                    break;
                case RequestOpcode.control:
                    ControlParameter controlParam = operate.Parameter as ControlParameter;                  
                    break;
            }
        }
        private void ChangeValueExec(object parameter)
        {
            if (parameter != null && parameter is TargetStatusChangedEventArgs)
            {
                TargetStatusChangedEventArgs e = parameter as TargetStatusChangedEventArgs;
                switch (e.Key)
                {
                    case ChangedKey_t.OnlineStatus:
                        if (_type == TargetType_t.Member && _member != null)
                        {
                            IsOnline = (bool)e.NewValue;
                            UpdateAllStatus();
                        }

                        break;
                    case ChangedKey_t.LocationStatus:
                        if (_type == TargetType_t.Member && _member != null)
                        {
                            _member.LocationGpsStatus = (LocationStatus_t)e.NewValue;
                            NotifyPropertyChanged("IsInLocation");
                            NotifyPropertyChanged("LocationStatusVisible");
                            NotifyPropertyChanged("CanLocation");
                            NotifyPropertyChanged("CanStopLocation");
                        }
                        break;
                    case ChangedKey_t.LocationInDoorStatus:
                        if (_type == TargetType_t.Member && _member != null)
                        {
                            _member.LocationInDoorStatus = (LocationStatus_t)e.NewValue;
                            NotifyPropertyChanged("IsInLocationInDoor");
                            NotifyPropertyChanged("LocationInDoorStatusVisible");
                            NotifyPropertyChanged("CanLocationInDoor");
                            NotifyPropertyChanged("CanStopLocationInDoor");
                        }
                        break;
                    case ChangedKey_t.ShutDownStatus:
                        IsShutDown = (bool)e.NewValue;
                        break;
                    case ChangedKey_t.CallStatus:
                        if (_type == TargetType_t.Member && _member != null)
                        {
                            _member.CallStatus = (CallStatus_t)e.NewValue;
                        }
                        else if (_type == TargetType_t.Group && _group != null)
                        {
                            _group.CallStatus = (CallStatus_t)e.NewValue;
                        }

                        SystemStatus.SystemCallStatus = (CallStatus_t)e.NewValue;
                        NotifyPropertyChanged("EnableCallOrStop");
                        NotifyPropertyChanged("IsInCall");
                        NotifyPropertyChanged("InCallStatusVisible");
                        NotifyPropertyChanged("CanCall");
                        NotifyPropertyChanged("CanStopCall");
                        break;
                    default:
                        break;

                }
            }
        }


        private void AddFastPanelExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.NewFast, this));
        }

        private void CallExec(object parameter)
        {
            if (parameter == null) return;
            CallArgs args = null;
            if (parameter is CallArgs)
            {
                args = parameter as CallArgs;
            }
            else
            {
                CallOperatedType_t? _type = null;
                if (parameter is string) _type = (parameter as string).ToEnum<CallOperatedType_t>();
                else if (parameter is CallOperatedType_t) _type = (CallOperatedType_t)parameter;

                if (_type == null) return;

                args = new CallArgs((CallOperatedType_t)_type);
            }

            if (args == null) return;

            switch (args.Type)
            {
                case CallOperatedType_t.Start:

                    if (_dispatcher != null)
                    {
                        if (_type == TargetType_t.Group && Group.IsAllTarget) _dispatcher.AllCall();
                        else if (_type == TargetType_t.Group) _dispatcher.GroupCall(Group.GroupID);
                        else if (Member.HasDevice) _dispatcher.PrivateCall(Member.RadioID);
                        else return;
                        //Log.Message("发起呼叫  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "发起呼叫" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                    }
                    break;
                case CallOperatedType_t.Stop:
                    if (_dispatcher != null && IsInCall)
                    {
                        if (_type == TargetType_t.Group && Group.IsAllTarget) _dispatcher.StopCall(-1, TargetMode_t.All);
                        else if (_type == TargetType_t.Group) _dispatcher.StopCall(Group.GroupID, TargetMode_t.Group);
                        else if (Member.HasDevice) _dispatcher.StopCall(Member.RadioID, TargetMode_t.Private);
                        else return;
                        //Log.Message("结束呼叫  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                    }
                    break;
            }
        }
        private void OnCallStatusChanged(CallResponseArgs e)
        {
            if (e == null || !IsThis(e.Type, e.Target)) return;

            if (e.Opcode == ExecType_t.Start)
            {
                if (!IsInCall && e.Status == OperationStatus_t.Success)
                {
                    Log.Message("开始呼叫  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "开始呼叫" });

                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                }             
            }
            else if (e.Opcode == ExecType_t.Stop)
            {
                if (IsInCall && e.Status == OperationStatus_t.Success)
                {
                    Log.Message("结束呼叫  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫" });

                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                }            
            }
        }

        private void OnCallResponse(CallResponseArgs e)
        {
            if (e == null || !IsThis(e.Type, e.Target)) return;
            if (IsInCall && e.Opcode == ExecType_t.Start)
            {
                if (e.Status == OperationStatus_t.Success)
                {
                    //Log.Message("开始呼叫  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "开始呼叫" });

                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                }
                else
                {
                    //Log.Message("呼叫失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "呼叫失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                }
            }
            else if (!IsInCall && e.Opcode == ExecType_t.Stop)
            {
                if (e.Status == OperationStatus_t.Success)
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                }
                else
                {
                    //Log.Message("结束呼叫失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                }
            }
        }


       
        private void OnCallRequest(CallRequestArgs e)
        {
            if (e == null) return;
            if (_type == TargetType_t.Group && !IsThis(e.Type, e.Target)) return;
            if (_type == TargetType_t.Member && !IsThis(e.Type, e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));


            IsCurrentRx = e.IsCurrent;

            if (e.Opcode == ExecType_t.Start)
            {
                Log.Message(FullName + "  开始呼叫");
                AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "开始呼叫" }, true);
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Rx));
                SystemStatus.Add(this);
            }
            else if (e.Opcode == ExecType_t.Stop)
            {
                Log.Message(FullName + "  结束呼叫");
                AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫" }, true);
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                SystemStatus.Remove(this);
            }
        }


        private void SendShortMessageExec(object parameter)
        {
            if (parameter == null && parameter is ShortMessageArgs) return;
            ShortMessageArgs args = parameter as ShortMessageArgs;
            //Log.Message("发送短消息（内容：" + args.Contents + "）  " + FullName);
            //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = args.Contents });

            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    List<VMTarget> UnBounded = ResourcesMgr.Instance().Members.FindAll(p => true);
                    foreach (VMTarget group in ResourcesMgr.Instance().Groups)
                    {
                        if (group.CanShortMessage) _dispatcher.SendGroupShortMessage(group._group.GroupID, args.Contents);
                        UnBounded.RemoveAll(p => p._member.GroupID == group._group.GroupID);
                    }

                    foreach (VMTarget member in UnBounded)
                    {
                        if (member.Member.HasDevice && member.CanShortMessage) _dispatcher.SendPrivateShortMessage(member.Member.RadioID, args.Contents);
                    }
                }
                else if (_type == TargetType_t.Group)
                {
                    if (CanShortMessage) _dispatcher.SendGroupShortMessage(_group.GroupID, args.Contents);
                }
                else if (Member.HasDevice) _dispatcher.SendPrivateShortMessage(Member.RadioID, args.Contents);
            }

        }


        private void OnShortMessageResponse(ShortMessageResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;

            if (e.Status == OperationStatus_t.Success)
            {
                //Log.Message("发送短消息成功  " + FullName);
                if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
            }
            else
            {
                //Log.Message("发送短消息失败  " + FullName);
                //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = "发送短消息失败" });
            }
        }


        private void OnShortMessageRequest(ShortMessageRequestArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
            Log.Message(FullName + "  发送短消息（内容：" + e.Contents + "）");
            AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = e.Contents }, true);
        }

      
        private void ControlExec(object parameter)
        {
            if (parameter == null) return;

            ControlArgs args = null;
            if (parameter is ControlArgs)
            {
                args = parameter as ControlArgs;
            }
            else
            {
                ControlerType_t? _type = null;
                if (parameter is string) _type = (parameter as string).ToEnum<ControlerType_t>();
                else if (parameter is CallOperatedType_t) _type = (ControlerType_t)parameter;

                if (_type == null) return;

                args = new ControlArgs((ControlerType_t)_type);
            }

            if (args == null) return;

            switch (args.Type)
            {
                case ControlerType_t.Check:
                    //Log.Message("在线检测  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测" });
                    break;
                case ControlerType_t.Monitor:
                    //Log.Message("远程监听  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "远程监听" });
                    break;
                case ControlerType_t.ShutDown:
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, true));
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥毙" });
                    //Log.Message("遥毙  " + FullName);
                    break;
                case ControlerType_t.StartUp:
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, false));
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥开" });
                    //Log.Message("遥开  " + FullName);
                    break;
                default:
                    return;
            }

            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    foreach (VMTarget member in ResourcesMgr.Instance().Members) if (member.Member.HasDevice) _dispatcher.Control(member.Member.RadioID, args.Type);
                }
                else if (_type == TargetType_t.Group)
                {
                    List<VMTarget> members = ResourcesMgr.Instance().Members.FindAll(p => p.TargetType == TargetType_t.Member && Member.GroupID == Group.GroupID);
                    foreach (VMTarget member in members) if (member.Member.HasDevice) _dispatcher.Control(member.Member.RadioID, args.Type);
                }
                else if (Member.HasDevice) _dispatcher.Control(Member.RadioID, args.Type);
            }


            Log.Debug("Control " + parameter.ToString() + " " + FullName);
        }


        private void OnControlResponse(ControlResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;

            switch (e.Type)
            {
                case ControlerType_t.Check:
                    if (e.Status == ControlResponsetatus_t.Failure)
                    {
                        //Log.Message("在线检测失败  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测失败" });
                    }
                    else
                    {
                        Log.Message(FullName + "  在线检测：" + (e.Status == ControlResponsetatus_t.Online ? "在线" : "离线"));
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测:" + (e.Status == ControlResponsetatus_t.Online ? "在线" : "离线") }, true);
                        ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, (e.Status == ControlResponsetatus_t.Online ? true : false)));
                    }
                    break;
                case ControlerType_t.Monitor:
                    OperationStatus_t monitorstate = (OperationStatus_t)(int)e.Status;
                    if (monitorstate == OperationStatus_t.Failure)
                    {
                        //Log.Message("远程监听失败  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "远程监听失败" });
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                case ControlerType_t.ShutDown:
                    if (!IsShutDown) return;
                    OperationStatus_t shutstate = (OperationStatus_t)(int)e.Status;
                    if (shutstate == OperationStatus_t.Failure)
                    {
                        //Log.Message("遥毙失败  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥毙失败" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, false));
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                case ControlerType_t.StartUp:
                    if (IsShutDown) return;
                    OperationStatus_t startstate = (OperationStatus_t)(int)e.Status;
                    if (startstate == OperationStatus_t.Failure)
                    {
                        //Log.Message("遥开失败  " + FullName);
                        //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥开失败" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, true));
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                default:
                    return;
            }
        }

        private void OnControlResult(ControlResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;

            switch (e.Type)
            {
                case ControlerType_t.Check:
                    if (e.Status == ControlResponsetatus_t.Failure)
                    {
                        Log.Message("在线检测失败  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测失败" });
                    }
                    else
                    {
                        Log.Message(FullName + "  在线检测：" + (e.Status == ControlResponsetatus_t.Online ? "在线" : "离线"));
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测:" + (e.Status == ControlResponsetatus_t.Online ? "在线" : "离线") }, true);
                        ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, (e.Status == ControlResponsetatus_t.Online ? true : false)));
                    }
                    break;
                case ControlerType_t.Monitor:
                    OperationStatus_t monitorstate = (OperationStatus_t)(int)e.Status;
                    if (monitorstate == OperationStatus_t.Failure)
                    {
                        Log.Message("远程监听失败  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "远程监听失败" });
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                case ControlerType_t.ShutDown:
                    if (!IsShutDown) return;
                    OperationStatus_t shutstate = (OperationStatus_t)(int)e.Status;
                    if (shutstate == OperationStatus_t.Failure)
                    {
                        Log.Message("遥毙失败  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥毙失败" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, false));
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                case ControlerType_t.StartUp:
                    if (IsShutDown) return;
                    OperationStatus_t startstate = (OperationStatus_t)(int)e.Status;
                    if (startstate == OperationStatus_t.Failure)
                    {
                        Log.Message("遥开失败  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥开失败" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, true));
                    }
                    else
                    {
                        if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    }
                    break;
                default:
                    return;
            }
        }


        private void LocationRequest(QueryLocationType_t query, LocationType_t type)
        {

            double cycle = 30;
            if (query == QueryLocationType_t.LocationInDoor) if (FunctionConfigure.LocationInDoorSetting != null) cycle = FunctionConfigure.LocationInDoorSetting.Interval;
                else if (FunctionConfigure.LocationSetting != null) cycle = FunctionConfigure.LocationSetting.Interval;

            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    foreach (VMTarget member in ResourcesMgr.Instance().Members) if (member.Member.HasDevice) _dispatcher.Location(type, member.Member.RadioID, query, type == LocationType_t.StartTriggered ? cycle : 0);
                }
                else if (_type == TargetType_t.Group)
                {
                    List<VMTarget> members = ResourcesMgr.Instance().Members.FindAll(p => p.TargetType == TargetType_t.Member && p.Member.GroupID == Group.GroupID);
                    foreach (VMTarget member in members) if (member.Member.HasDevice) _dispatcher.Location(type, member.Member.RadioID, query, type == LocationType_t.StartTriggered ? cycle : 0);
                }
                else if (Member.HasDevice) _dispatcher.Location(type, Member.RadioID, query, type == LocationType_t.StartTriggered ? cycle : 0);
            }
        }
        private void LocationExec(object parameter)
        {
            LocationType_t type = (parameter as string).ToEnum<LocationType_t>();
            if(type == LocationType_t.StartTriggered)
            {
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Cycle));
            }
            else if(type == LocationType_t.StopTriggered)
            {
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Idle));
            }

            LocationRequest(QueryLocationType_t.LocationGps, (parameter as string).ToEnum<LocationType_t>());
        }

        private void OnLocationReport(LocationReportArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
            SystemStatus.DrawAmapPoint(new DrawLocationReportArgs(this, e.Report));
        }

        private void OnLocationResponse(LocationResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;
            if (e.Opcode == LocationType_t.Immediate)
            {
                if (e.Status == OperationStatus_t.Success)
                {
                    OnLocationReport(new LocationReportArgs(e.Target, e.Report as GpsReport));
                }
            }
            else if (!IsInLocation && e.Opcode == LocationType_t.StartTriggered)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    //Log.Message("结束位置查询失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Idle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
            else if (!IsInLocation && e.Opcode == LocationType_t.StopTriggered)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    //Log.Message("结束位置查询失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Cycle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
        }



        private void LocationInDoorExec(object parameter)
        {
            LocationType_t type = (parameter as string).ToEnum<LocationType_t>();
            if (type == LocationType_t.StartTriggered)
            {
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Cycle));
            }
            else if (type == LocationType_t.StopTriggered)
            {
                ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Idle));
            }


            LocationRequest(QueryLocationType_t.LocationInDoor, (parameter as string).ToEnum<LocationType_t>());
        }

        private BeaconReport FilterLocationInDoorReport(List<BeaconReport> Report)
        {
            if (Report != null && Report.Count > 0) return Report[0];
            else return null;



            //BeaconReport maxpower = Report.OrderByDescending(n => n.txpower).Take(1).ToList()[0];
            //if (_lastbeacons == null || _lastbeacons.Count <= 0 || _lastbeacons.Contains(maxpower))
            //{
            //    _lastbeacons = Report;
            //    return maxpower;
            //}
            //else
            //{
            //    _lastbeacons = Report;
            //    return null;
            //}
        }

        private void OnLocationInDoorReport(LocationInDoorReportArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));


            BeaconReport report = FilterLocationInDoorReport(e.Report);
            if (report == null) return;

            //removeorginpoint
            VMBeacon orginbeacon = ResourcesMgr.Instance().Beacons.Find(p => p.TargetList.Contains(this));
            if (orginbeacon != null) orginbeacon.RemovePos.Execute(this);



            VMBeacon beacon = ResourcesMgr.Instance().Beacons.Find(p => p.Beacon != null && p.Beacon.Major == report.major && p.Beacon.Minor == report.minor);
            if (beacon != null)
            {
                beacon.AddPos.Execute(this);

                Log.Debug(FullName + "  室内定位信息(信标：" + beacon.Beacon.Major.ToString() + "," + beacon.Beacon.Minor.ToString() + ")");  
            }
        }

        private void OnLocationInDoorResponse(LocationInDoorResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;
            if (e.Opcode == LocationType_t.Immediate)
            {
                if (e.Status == OperationStatus_t.Success)
                {
                    OnLocationInDoorReport(new LocationInDoorReportArgs(e.Target, new List<BeaconReport>() { e.Report }));
                }
            }
            else if (!IsInLocation && e.Opcode == LocationType_t.StartTriggered)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    //Log.Message("结束位置查询失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Idle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
            else if (!IsInLocation && e.Opcode == LocationType_t.StopTriggered)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    //Log.Message("结束位置查询失败  " + FullName);
                    //AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationStatus_t.Cycle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }

        }



        private bool IsThis(TargetMode_t type, int id)
        {
            if (type == TargetMode_t.All)
            {
                return this._type == TargetType_t.Group && Group.IsAllTarget;
            }
            else if (type == TargetMode_t.Group)
            {
                return this._type == TargetType_t.Group && Group.GroupID == id;
            }
            else if (type == TargetMode_t.Private)
            {
                return this._type == TargetType_t.Member && Member.HasDevice && Member.RadioID == id;
            }
            else return false;
        }

        private bool IsThis(int id)
        {
            return IsThis(TargetMode_t.Private, id);
        }


        


        private void OnArsRequest(ArsRequestArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != e.IsOnline)ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, e.IsOnline));

            Log.Message(FullName + (e.IsOnline ? "  上线" : "  下线"));
            AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = e.IsOnline ? "上线" : "下线" }, true);
        }

       

        private void OpenOperateWindowExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                TaskType_t key = ((string)parameter).ToEnum<TaskType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenOperateWindow, new TargetOperateArgs(key, this)));
                ClearFailure();
            }
        }

        private void PlayVoiceExec(object parameter)
        {
            if (_dispatcher == null || parameter == null || !(parameter is System.Windows.Controls.RadioButton)) return;
            System.Windows.Controls.RadioButton rad = parameter as System.Windows.Controls.RadioButton;

            if (!(bool)rad.IsChecked)
            {
                rad.IsChecked = !rad.IsChecked;
                return;
            }

            if (_type == TargetType_t.Group && Group.IsAllTarget) _dispatcher.PlayVoice(-2);
            else if (_type == TargetType_t.Group) _dispatcher.PlayVoice(Group.GroupID);
            else if (Member.HasDevice) _dispatcher.PlayVoice(-1);
        }

        private void QueryLocationExec(object parameter)
        {
            LocationExec(parameter);
        }

       
        private void UpdateAllStatus()
        {
            NotifyPropertyChanged("IsOnline");
            NotifyPropertyChanged("IsOnlineIfNeed");

            NotifyPropertyChanged("EnableCallOrStop");
            NotifyPropertyChanged("CanCall");
            NotifyPropertyChanged("CanShortMessage");

            NotifyPropertyChanged("CanLocation");
            NotifyPropertyChanged("CanStopLocation");

            NotifyPropertyChanged("CanLocationInDoor");
            NotifyPropertyChanged("CanStopLocationInDoor");


            NotifyPropertyChanged("CanCheckOnline");
            NotifyPropertyChanged("CanMonitor");
            NotifyPropertyChanged("CanShutDown");
            NotifyPropertyChanged("CanShartUp");
            NotifyPropertyChanged("CanSleep");
            NotifyPropertyChanged("CanWeek");

            NotifyPropertyChanged("Icon");
            NotifyPropertyChanged("OnlineOpacity");

            NotifyPropertyChanged("LocationStatusVisible");
            NotifyPropertyChanged("LocationInDoorStatusVisible");
            NotifyPropertyChanged("ShutDownStatusVisible");
            NotifyPropertyChanged("SleepStatusVisible");
            NotifyPropertyChanged("InCallStatusVisible");

            NotifyPropertyChanged("EnableLocationFunc");
            NotifyPropertyChanged("EnableLocationInDoorFunc");
            NotifyPropertyChanged("EnableControlFunc");
        }
        #region INotifyPropertyChanged Members

        private static int MaxNotifyCount = 100;

        private int _currentnotifyindex = 0;

        //Notify
        private List<VMNotify.VMNotice> _notices = new List<VMNotify.VMNotice>();

        private int _unreadnotifyindex = 0;

        public static event PropertyChangedEventHandler StaticPropertyChanged;

        public event PropertyChangedEventHandler PropertyChanged;
        public int CurrentNotifyIndex { get { return _currentnotifyindex; } set { _currentnotifyindex = value; NotifyPropertyChanged("CurrentNotifyIndex"); } }

        public ICollectionView INotices { get { return new ListCollectionView(_notices); } }

        public List<VMNotify.VMNotice> Notices { get { return _notices; } }

        public int UnReadNotifyIndex { get { return _unreadnotifyindex; } set { _unreadnotifyindex = value; NotifyPropertyChanged("UnReadNotifyIndex"); } }

        public void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }

            if (StaticPropertyChanged != null) StaticPropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
        private void AddNotify(CNotice notice, bool isrx = false)
        {
            if (notice == null) return;

            if (_notices.Count >= MaxNotifyCount)
            {
                _notices.RemoveAt(0);
                if (UnReadNotifyIndex > 0) UnReadNotifyIndex -= 1;
            }

            VMNotify.VMNotice notiveviewmodule = new VMNotify.VMNotice("", this, notice, !isrx);
            _notices.Add(notiveviewmodule);
  
            NotifyPropertyChanged("INotices");

            if (isrx && OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.AddNotify, notiveviewmodule));
            //if(isrx)
            //{
            //    switch(notice.Type)
            //    {
            //        case NotifyKey_t.Alarm:
            //            Log.Message("报警信息（" + notiveviewmodule.Remote.Name + "）：" + notiveviewmodule.ContentBiref);
            //            break;
            //        case NotifyKey_t.ShortMessage:
            //            Log.Message("接收短信（" + notiveviewmodule.Remote.Name + "）：" + notiveviewmodule.ContentBiref);
            //            break;
            //    }
            //}
        }
            

        #endregion
    }
}
