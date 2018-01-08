using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using System.Threading;

using Sigmar;
using Dispatcher;

using Dispatcher.Service;

namespace Dispatcher.ViewsModules
{
    public class VMStatus : INotifyPropertyChanged
    {
        private ServerStatus _status;
        public event OperatedEventHandler OnOperated;

        public DateTime SystemTime { get { return DateTime.Now; } }

        public bool _isWait = false;
        public Visibility WaitVisible { get{return _isWait ? Visibility.Visible:Visibility.Collapsed;}  }

        public void SetStatusWait(bool iswait)
        {
            _isWait = iswait;
            NotifyPropertyChanged("WaitVisible");
        }
        public string StatusContent
        {
            get 
            {
                if (_status == null || _status.TServer == null || !_status.TServer.IsConnected || _status.LogServer == null || !_status.LogServer.IsConnected)
                    return "服务未连接";
                else if (_status.SystemStatus == null)
                {
                    SetStatusWait(true);
                    return "获取系统信息...";
                }
                else
                {
                    SetStatusWait(false);
                    return _status.SystemStatus.ToString();
                }


                //if (_status == null || _status.TServer == null || !_status.TServer.IsConnected || _status.LogServer == null || !_status.LogServer.IsConnected) 
                //    return "服务未连接";
                //else
                //{
                //    string status = "";
                //    switch (FunctionConfigure.WorkMode)
                //    {

                //        case FunctionConfigure.Mode_t.Debug:
                //            if (!_status.Repeater.IsConnected || !_status.VehicleStation.IsConnected || !_status.Mnis.IsConnected) return "设备未连接";
                //            else return "设备已连接";
                //        case FunctionConfigure.Mode_t.Repeater:
                //            return string.Format(
                //                "中继台{0}连接{1}", 
                //                _status.Repeater.IsConnected ? "已" : "未",
                //                _status.Repeater.IsConnected ?  string.Format("：{0}：{1}",_status.Repeater.Host, _status.Repeater.Port) : "");

                //        case FunctionConfigure.Mode_t.RepeaterWithMnis:
                //            return string.Format(
                //                "中继台{0}连接{1}\tMNIS{2}连接{3}",
                //                _status.Repeater.IsConnected ? "已" : "未",
                //                _status.Mnis.IsConnected ?string.Format("{0}：{1}", _status.Repeater.Host, _status.Repeater.Port) : "",
                //                _status.Mnis.IsConnected ? "已" : "未",
                //                _status.Mnis.IsConnected ? string.Format("{0}", _status.Mnis.Host) : ""  );

                //        case FunctionConfigure.Mode_t.VehicleStation:
                //            return string.Format(
                //                "车载台{0}连接{1}",
                //                _status.VehicleStation.IsConnected ? "已" : "未",
                //                _status.VehicleStation.IsConnected ? string.Format("：{0}", _status.VehicleStation.Host) : "");

                //        case FunctionConfigure.Mode_t.VehicleStationWithMnis:

                //            return string.Format(
                //               "车载台{0}连接{1}\tMNIS{2}连接{3}",
                //               _status.VehicleStation.IsConnected ? "已" : "未",
                //               _status.VehicleStation.IsConnected ? string.Format("{0}", _status.Repeater.Host) : "",
                //               _status.Mnis.IsConnected ? "已" : "未",
                //               _status.Mnis.IsConnected ? string.Format("{0}", _status.Mnis.Host) : "");                          
                //        default:
                //            return "离线模式";
                //    }
                //}
            }
        }


        public void UpdateGlobalStatus(GlobalStatus globalStatus)
        {
            _status.SystemStatus = globalStatus;
            NotifyPropertyChanged("StatusContent");
        }
        public Visibility ReconnectedVisible 
        {
            get 
            {
                if (_status != null && _status.TServer.IsConnected && _status.LogServer.IsConnected) return Visibility.Collapsed;
                else return Visibility.Visible;
            }
        }

        public VMStatus()
        {
            if (_status == null)
            {
                _status = ServerStatus.Instance();
                _status.StatusChanged += delegate { NotifyPropertyChanged("StatusContent"); };
                _status.WaitStatusChanged += delegate(object sender, bool iswait) { SetStatusWait(iswait); };
            }

            StartUpdateSystemTime();
        }

        private void StartUpdateSystemTime()
        {
            new Thread(new ThreadStart(delegate()
            {
                while (true)
                {
                    NotifyPropertyChanged("SystemTime");
                    Thread.Sleep(900);
                }
            })).Start();           
        }

        //public void UpdateStatus(ServerStatusChangedEventArgs e)
        //{
        //    if (_status == null) 
        //    {
        //         _status = ServerStatus.Instance();
        //        Log.Warning("_status is null");
        //    }

        //    _status.Update(e);
        //    NotifyPropertyChanged("StatusContent");
        //    NotifyPropertyChanged("ReconnectedVisible");
        //}

        public ICommand Reconnect { get { return new Command(ReconnectExec); } }
        private void ReconnectExec()
        {
            if (_status == null)
            {
                _status = ServerStatus.Instance();
                Log.Warning("_status is null");
            }

            if (!_status.TServer.IsConnected && OnOperated != null)OnOperated(new OperatedEventArgs(OperateType_t.ConnectTServer));
            if (!_status.LogServer.IsConnected && OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.ConnectLogServer));
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
