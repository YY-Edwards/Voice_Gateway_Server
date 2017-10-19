﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;

using Dispatcher.Modules;
using Dispatcher.Service;
using Sigmar.Logger;
using Sigmar;
using Sigmar.Extension;

namespace Dispatcher.ViewsModules
{
    public class VMTarget : INotifyPropertyChanged
    {
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
                if (IsInCall) return true;
                else return !SystemStatus.IsSystemInCall && IsOnlineIfNeed && !IsInCall ? true : false;
            }
        }

        public bool EnableControlFunc
        {
            get
            {
                if (_type == TargetType_t.Member && _member != null) return true;
                else return false;
            }
        }

        public bool EnableLocationFunc
        {
            get
            {
                if (_type == TargetType_t.Member) return _member != null && IsOnlineIfNeed && _member.HasLocation ? true : false;
                else if (_type == TargetType_t.Group) return _group != null ? true : false;
                else return false;
            }
        }

        public bool EnableLocationInDoorFunc
        {
            get
            {
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

        public int ID
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

        public bool IsInLocation { get { return _type == TargetType_t.Member && _member != null && _member.LocationStatus != LocationStatus_t.Idle ? true : false; } }
        public bool IsInLocationInDoor { get { return _type == TargetType_t.Member && _member != null && _member.LocationInDoorStatus != LocationInDoorStatus_t.Idle ? true : false; } }

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
                if (RunAccess.Mode == RunAccess.Mode_t.CPC || RunAccess.Mode == RunAccess.Mode_t.IPSC || RunAccess.Mode == RunAccess.Mode_t.LCP)
                {
                    _dispatcher = RepeaterDispatcher.Instance();
                }
                else if (RunAccess.Mode == RunAccess.Mode_t.VehicleStation || RunAccess.Mode == RunAccess.Mode_t.VehicleStationWithMnis)
                {
                    _dispatcher = VehicleStationDispatcher.Instance();
                }

                if (_dispatcher == null) return;
                _dispatcher.CallResponse += new CallResponseHandler(OnCallResponse);
                _dispatcher.CallRequest += new CallRequestHandler(OnCallRequest);

                _dispatcher.ShortMessageResponse += new ShortMessageResponseHandler(OnShortMessageResponse);
                _dispatcher.ShortMessageRequest += new ShortMessageRequestHandler(OnShortMessageRequest);

                _dispatcher.ArsRequest += new ArsRequestHandler(OnArsRequest);

                _dispatcher.ControlResponse += new ControlResponseHandler(OnControlResponse);

                _dispatcher.LocationResponse += new LocationResponseHandler(OnLocationResponse);
                _dispatcher.LocationReport += new LocationReportHandler(OnLocationReport);

                _dispatcher.LocationInDoorResponse += new LocationInDoorResponseHandler(OnLocationInDoorResponse);
                _dispatcher.LocationInDoorReport += new LocationInDoorReportHandler(OnLocationInDoorReport);
            }

            ServerStatus.Instance().StatusChanged += delegate { UpdateAllStatus(); };
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
                        Log.Message("发起呼叫  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "发起呼叫" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                    }
                    break;
                case CallOperatedType_t.Stop:
                    if (_dispatcher != null)
                    {
                        if (_type == TargetType_t.Group && Group.IsAllTarget) _dispatcher.StopCall(-1);
                        else if (_type == TargetType_t.Group) _dispatcher.StopCall(Group.GroupID);
                        else if (Member.HasDevice) _dispatcher.StopCall(Member.RadioID);
                        else return;
                        Log.Message("结束呼叫  " + FullName);
                        AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫" });
                        ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                    }
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
                            _member.LocationStatus = (LocationStatus_t)e.NewValue;
                            NotifyPropertyChanged("IsInLocation");
                            NotifyPropertyChanged("LocationStatusVisible");
                            NotifyPropertyChanged("CanLocation");
                            NotifyPropertyChanged("CanStopLocation");
                        }
                        break;
                    case ChangedKey_t.LocationInDoorStatus:
                        if (_type == TargetType_t.Member && _member != null)
                        {
                            _member.LocationInDoorStatus = (LocationInDoorStatus_t)e.NewValue;
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
                    Log.Message("在线检测  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "在线检测" });
                    break;
                case ControlerType_t.Monitor:
                    Log.Message("远程监听  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "远程监听" });
                    break;
                case ControlerType_t.ShutDown:
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, true));
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥毙" });
                    Log.Message("遥毙  " + FullName);
                    break;
                case ControlerType_t.StartUp:
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.ShutDownStatus, false));
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Control, Contents = "遥开" });
                    Log.Message("遥开  " + FullName);
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

        private void LocationExec(object parameter)
        {
            if (parameter == null || !(parameter is LocationArgs)) return;
            LocationArgs args = parameter as LocationArgs;

            if (args.Type == LocationType_t.Cycle || args.Type == LocationType_t.CsbkCycle || args.Type == LocationType_t.EnhCsbkCycle)
            {
                if (args.Cycle <= 0)
                {
                    Log.Message("位置查询错误，周期不能为零  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "位置查询错误，周期不能为零" });
                    return;
                }
            }

            switch (args.Type)
            {
                case LocationType_t.Query:
                    Log.Message("单次查询  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "单次查询" });
                    break;
                case LocationType_t.CsbkQuery:
                    Log.Message("CSBK单次查询  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "CSBK单次查询" });
                    break;
                case LocationType_t.EnhCsbkQuery:
                    Log.Message("增强型CSBK单次查询  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "增强型CSBK单次查询" });
                    break;
                case LocationType_t.StopCycle:
                    Log.Message("停止周期查询  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "停止周期查询" });
                    break;
                case LocationType_t.Track:
                    Log.Message("轨迹回放  " + Name);
                    return;
                case LocationType_t.Cycle:
                    Log.Message("周期查询（周期：" + args.Cycle.ToString() + "）  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "周期查询（周期：" + args.Cycle.ToString() + "）" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Cycle));

                    break;
                case LocationType_t.CsbkCycle:
                    Log.Message("CSBK周期查询（周期：" + args.Cycle.ToString() + "）  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "CSBK周期查询（周期：" + args.Cycle.ToString() + "）" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.CsbkCycle));
                    break;
                case LocationType_t.EnhCsbkCycle:
                    Log.Message("增强型CSBK周期查询（周期：" + args.Cycle.ToString() + "）  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Location, Contents = "增强型CSBK周期查询（周期：" + args.Cycle.ToString() + "）" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.EnhCsbkCycle));

                    break;
                default: return;
            }

            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    foreach (VMTarget member in ResourcesMgr.Instance().Members) if (member.Member.HasDevice) _dispatcher.Location(member.Member.RadioID, args);
                }
                else if (_type == TargetType_t.Group)
                {
                    List<VMTarget> members = ResourcesMgr.Instance().Members.FindAll(p => p.TargetType == TargetType_t.Member && p.Member.GroupID == Group.GroupID);
                    foreach (VMTarget member in members) if (member.Member.HasDevice) _dispatcher.Location(member.Member.RadioID, args);
                }
                else if (Member.HasDevice) _dispatcher.Location(Member.RadioID, args);
            }
        }

        private void LocationInDoorExec(object parameter)
        {
            if (parameter == null) return;
            LocationInDoorArgs args = null;

            if (parameter is LocationInDoorArgs)
            {
                args = parameter as LocationInDoorArgs;
            }
            else
            {
                LocationInDoorType_t? _type = null;
                if (parameter is string) _type = (parameter as string).ToEnum<LocationInDoorType_t>();
                else if (parameter is LocationInDoorType_t) _type = (LocationInDoorType_t)parameter;

                if (_type == null) return;

                args = new LocationInDoorArgs((LocationInDoorType_t)_type);
            }

            if (args == null) return;
            switch (args.Type)
            {
                case LocationInDoorType_t.Start:
                    Log.Message("启动室内定位  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "启动室内定位" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationInDoorStatus_t.Cycle));

                    break;
                case LocationInDoorType_t.Stop:
                    Log.Message("停止室内定位  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "停止室内定位" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationInDoorStatus_t.Idle));
                    break;
            }


            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    foreach (VMTarget member in ResourcesMgr.Instance().Members) if (member.Member.HasDevice) _dispatcher.LocationInDoor(member.Member.RadioID, args);
                }
                else if (_type == TargetType_t.Group)
                {
                    List<VMTarget> members = ResourcesMgr.Instance().Members.FindAll(p => p.TargetType == TargetType_t.Member && p.Member.GroupID == Group.GroupID);
                    foreach (VMTarget member in members) if (member.Member.HasDevice) _dispatcher.LocationInDoor(member.Member.RadioID, args);
                }
                else if (Member.HasDevice) _dispatcher.LocationInDoor(Member.RadioID, args);
            }
        }

        private void OnArsRequest(ArsRequestArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != e.IsOnline)ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, e.IsOnline));

            Log.Message(FullName + (e.IsOnline ? "  上线" : "  下线"));
            AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = e.IsOnline ? "上线" : "下线" }, true);
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

        private void OnCallResponse(CallResponseArgs e)
        {
            if (e == null || !IsThis(e.Type, e.Target)) return;
            if (IsInCall && e.Opcode == ExecType_t.Start)
            {
                if (e.Status == OperationStatus_t.Success)
                {                 
                    Log.Message("开始呼叫  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "开始呼叫" });

                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                }
                else
                {
                    Log.Message("呼叫失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "呼叫失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                }
            }
            else if (!IsInCall &&  e.Opcode == ExecType_t.Stop)
            {
                if (e.Status == OperationStatus_t.Success)
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Idle));
                }
                else
                {
                    Log.Message("结束呼叫失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.Called, Contents = "结束呼叫失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.CallStatus, CallStatus_t.Tx));
                }
            }
        }

        private void OnControlResponse(ControlResponseArgs e)
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

        private void OnLocationInDoorReport(LocationInDoorReportArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));


            BeaconReport report = FilterLocationInDoorReport(e.Report);
            if (report == null) return;

            VMBeacon beacon = ResourcesMgr.Instance().Beacons.Find(p => p.Beacon != null && p.Beacon.Major == report.major && p.Beacon.Minor == report.minor);
            if (beacon != null)
            {
                beacon.AddPos.Execute(this);

                Log.Message(FullName + "  室内定位信息(信标：" + beacon.Beacon.Major.ToString() + "," + beacon.Beacon.Minor.ToString() + ")");
                AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "室内位置信息(" + beacon.Beacon.Major.ToString() + "," + beacon.Beacon.Minor.ToString() + ")" }, true);

                //removeorginpoint
                VMBeacon orginbeacon = ResourcesMgr.Instance().Beacons.Find(p => p.TargetList.Contains(this));
                if (orginbeacon != null) orginbeacon.RemovePos.Execute(this);
            }
        }

        private void OnLocationInDoorResponse(LocationInDoorResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;

            if (IsInLocationInDoor &&  e.Opcode == ExecType_t.Start)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    Log.Message("启动室内定位失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "启动室内定位失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationInDoorStatus_t.Idle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
            else if (!IsInLocationInDoor && e.Opcode == ExecType_t.Stop)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    Log.Message("结束室内定位失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束室内定位失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, LocationInDoorStatus_t.Cycle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
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
            if (e.Opcode == ExecType_t.Start)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    Log.Message("位置查询失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Idle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
            else if (!IsInLocation && e.Opcode == ExecType_t.Stop)
            {
                if (e.Status != OperationStatus_t.Success)
                {
                    Log.Message("结束位置查询失败  " + FullName);
                    AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.LocationInDoor, Contents = "结束位置查询失败" });
                    ChangeValueExec(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, LocationStatus_t.Cycle));
                }
                else
                {
                    if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
                }
            }
        }

        private void OnShortMessageRequest(ShortMessageRequestArgs e)
        {
            if (e == null || !IsThis(e.Source)) return;

            if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
            Log.Message(FullName + "  发送短消息（内容：" + e.Contents + "）");
            AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = e.Contents }, true);
        }

        private void OnShortMessageResponse(ShortMessageResponseArgs e)
        {
            if (e == null || !IsThis(e.Target)) return;

            if (e.Status == OperationStatus_t.Success)
            {
                Log.Message("发送短消息成功  " + FullName);
                if (IsOnline != true) ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, true));
            }
            else
            {
                Log.Message("发送短消息失败  " + FullName);
                AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = "发送短消息失败" });
            }
        }

        private void OpenOperateWindowExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                TaskType_t key = ((string)parameter).ToEnum<TaskType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenOperateWindow, new TargetOperateArgs(key, this)));
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
            if (parameter == null) return;

            LocationArgs args = null;
            if (parameter is LocationArgs)
            {
                args = parameter as LocationArgs;
                if (args != null) LocationExec(args);
            }
            else
            {
                LocationType_t? _type = null;
                if (parameter is string) _type = (parameter as string).ToEnum<LocationType_t>();
                else if (parameter is LocationType_t) _type = (LocationType_t)parameter;

                if (_type == null) return;

                args = new LocationArgs((LocationType_t)_type);


                if (args == null) return;

                switch (args.Type)
                {
                    case LocationType_t.Query:
                    case LocationType_t.CsbkQuery:
                    case LocationType_t.EnhCsbkQuery:
                    case LocationType_t.StopCycle:
                    case LocationType_t.Track:
                        LocationExec(args);
                        break;
                    case LocationType_t.Cycle:
                    case LocationType_t.CsbkCycle:
                    case LocationType_t.EnhCsbkCycle:
                        args.Cycle = 30;
                        LocationExec(args);
                        //if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenOperateWindow, new TargetOperateArgs(TaskType_t.Location, this, args)));
                        break;
                    default: break;
                }
            }


        }

        private void SendShortMessageExec(object parameter)
        {
            if (parameter == null && parameter is ShortMessageArgs) return;
            ShortMessageArgs args = parameter as ShortMessageArgs;
            Log.Message("发送短消息（内容：" + args.Contents + "）  " + FullName);
            AddNotify(new CNotice() { Time = DateTime.Now, Type = NotifyKey_t.ShortMessage, Contents = args.Contents });

            if (_dispatcher != null)
            {
                if (_type == TargetType_t.Group && Group.IsAllTarget)
                {
                    List<VMTarget> UnBounded = ResourcesMgr.Instance().Members.FindAll(p=>true);
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

            VMNotify.VMNotice notiveviewmodule = new VMNotify.VMNotice(this, notice, !isrx);
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
