using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sigmar.Logger;
using System.ComponentModel;

namespace Dispatcher.Service
{
    public class ServerStatus
    {
        private volatile static ServerStatus _instance = null;
        private static readonly object lockHelper = new object();

        public event EventHandler StatusChanged;
        public event Action<object ,bool> WaitStatusChanged;

        public static ServerStatus Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new ServerStatus();
                }
            }
            return _instance;
        }

        public ServerStatus_t TServer { get; set; }
        public ServerStatus_t LogServer { get; set; }
        public ServerStatus_t Repeater { get; set; }
        public ServerStatus_t VehicleStation { get; set; }
        public ServerStatus_t Mnis { get; set; }

        private ServerStatus()
        {
            TServer = new ServerStatus_t();
            LogServer = new ServerStatus_t();
            Repeater = new ServerStatus_t();
            VehicleStation = new ServerStatus_t();
            Mnis = new ServerStatus_t();          
        }


        public void SetWaitStatus(bool iswait)
        {
            if(WaitStatusChanged !=null) WaitStatusChanged(this, iswait);           
        }

        public ServerStatus_t VoiceBusiness
        {
            get
            {
                switch (FunctionConfigure.WorkMode)
                {
                    case FunctionConfigure.Mode_t.Repeater:
                    case FunctionConfigure.Mode_t.RepeaterWithMnis:
                        if (!TServer.IsConnected) return new ServerStatus_t(Repeater.Host, Repeater.Port,false);
                        else return Repeater;
                    case FunctionConfigure.Mode_t.VehicleStation:
                    case FunctionConfigure.Mode_t.VehicleStationWithMnis:
                        if (!TServer.IsConnected) return new ServerStatus_t(VehicleStation.Host, VehicleStation.Port, false);
                        else return VehicleStation;   
                    default:
                        return new ServerStatus_t("", 0, false);
                }
            }
        }

        public ServerStatus_t DataBusiness
        {
            get
            {
                switch (FunctionConfigure.WorkMode)
                {

                    case FunctionConfigure.Mode_t.RepeaterWithMnis:
                    case FunctionConfigure.Mode_t.VehicleStationWithMnis:
                        if (!TServer.IsConnected) return new ServerStatus_t(Mnis.Host, Mnis.Port, false);
                        else return Mnis;
                    case FunctionConfigure.Mode_t.VehicleStation:
                        if (!TServer.IsConnected) return new ServerStatus_t(VehicleStation.Host, VehicleStation.Port, false);
                        else return VehicleStation;
                    case FunctionConfigure.Mode_t.Repeater:
                    default:
                        return new ServerStatus_t("", 0, false);
                }
            }
        }


        //public void Update(ServerStatusChangedEventArgs e)
        //{
        //    switch (e.Key)
        //    {
        //        case ServerStatusType_t.TServer:
        //            TServer = e.NewValue;
        //            break;
        //        case ServerStatusType_t.LogServer:
        //            LogServer = e.NewValue;
        //            break;
        //        case ServerStatusType_t.Repeater:
        //            TServer = e.NewValue;
        //            break;
        //        case ServerStatusType_t.VehicleStation:
        //            Repeater = e.NewValue;
        //            break;
        //        case ServerStatusType_t.Mnis:
        //            Mnis = e.NewValue;
        //            break;
        //        default:
        //            break;
        //    }
        //}

        public enum ServerStatusType_t
        {
            TServer,
            LogServer,
            Repeater,
            VehicleStation,
            Mnis,
        }

        public class ServerStatus_t
        {
            private bool _isconnected = false;
            private string _host;
            private int _port;

            public bool IsConnected { get { return _isconnected; } private set { _isconnected = value;  } }
            public string Host { get { return _host; } private set { _host = value; } }
            public int Port { get { return _port; } private set { _port = value;  } }


            public ServerStatus_t()
            {
            }

            public ServerStatus_t( string host, int port, bool isconnected)
            {
                IsConnected = isconnected;
                Host = host;
                Port = port;
            }


            public ServerStatus_t SetHostAndPort(string host, int port)
            {
                Host = host;
                Port = port;                               
                StatusChanged();
                return this;
            }

            public ServerStatus_t SetStatus(bool isconnected)
            {
                IsConnected = isconnected;
                StatusChanged();
                return this;
            }

            private void StatusChanged()
            {
                if (ServerStatus.Instance().StatusChanged != null) ServerStatus.Instance().StatusChanged(this, null);
            }
        }


        public class VMServerStatus:INotifyPropertyChanged
        {
            private ServerStatus_t _state;
            public VMServerStatus(ServerStatus_t state)
            {
                _state = state;
            }

            public string HostAndPort { get { return _state == null ? "" : string.Format("{0}:{1}",_state.Host,_state.Port); } }
            public bool IsConnected { get { return _state == null ? false : _state.IsConnected; } }

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
}
