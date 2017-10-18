using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using System.Threading;

using Sigmar;
using Sigmar.Logger;

using Dispatcher.Service;

namespace Dispatcher.ViewsModules
{
    public class VMStatus : INotifyPropertyChanged
    {
        private ServerStatus _status;
        public event OperatedEventHandler OnOperated;

        public DateTime SystemTime { get { return DateTime.Now; } }

        public string StatusContent
        {
            get 
            {
                if (_status == null || _status.TServer == null || !_status.TServer.IsConnected || _status.LogServer == null || !_status.LogServer.IsConnected) return "服务未连接";
                else
                {
                    string status = "";
                    switch (RunAccess.Mode)
                    {

                        case RunAccess.Mode_t.All:
                            if (!_status.Repeater.IsConnected || !_status.VehicleStation.IsConnected || !_status.Mnis.IsConnected) return "设备未连接";
                            else return "设备已连接";
                        case RunAccess.Mode_t.LCP:
                        case RunAccess.Mode_t.CPC:
                        case RunAccess.Mode_t.IPSC:
                            status = "";
                            if(_status.Repeater.IsConnected)
                            {
                                status += "中继台已连接："+_status.Repeater.Host + ":" + _status.Repeater.Port.ToString();
                            }
                            else
                            {
                                status += "中继台未连接";
                            }

                            if(_status.Mnis.IsConnected)
                            {
                                status += "\tMNIS已连接："+_status.Mnis.Host;
                            }
                            else
                            {
                                status += "\tMNIS未连接";
                            }

                            return status;
                        case RunAccess.Mode_t.VehicleStation:
                            status = "";
                            if(_status.VehicleStation.IsConnected)
                            {
                                status += "车载台已连接：" + _status.VehicleStation.Host;
                            }
                            else
                            {
                                status += "车载台未连接";
                            }
                            return status;

                        case RunAccess.Mode_t.VehicleStationWithMnis:
                            status = "";
                            if (_status.VehicleStation.IsConnected)
                            {
                                status += "车载台已连接：" + _status.VehicleStation.Host;
                            }
                            else
                            {
                                status += "车载台未连接";
                            }

                            if(_status.Mnis.IsConnected)
                            {
                                status += "\tMNIS已连接："+_status.Mnis.Host;
                            }
                            else
                            {
                                status += "\tMNIS未连接";
                            }

                            return status;
                        default:
                            return "不支持的工作模式";
                    }
                }
            }
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
