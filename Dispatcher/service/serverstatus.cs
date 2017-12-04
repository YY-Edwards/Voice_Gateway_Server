using Newtonsoft.Json;
using System;
using System.ComponentModel;

namespace Dispatcher.Service
{
    public class GlobalStatus
    {
        public GlobalStatus()
        {
            WorkMode = FunctionConfigure.Mode_t.UnKnown;

            ServerStatus = 0;
            DeviceStatus = 0;
            DatabaseStatus = 0;
            MnisStatus = 0;
            MicphoneStatus = 0;
            SpeakerStatus = 0;
            LEStatus = 0;
            WireLanStatus = 0;
            DeviceInfoStatus = 0;
        }

        public FunctionConfigure.Mode_t WorkMode { get; set; }

        public int ServerStatus;
        public int DeviceStatus;
        public int DatabaseStatus;
        public int MnisStatus;
        public int MicphoneStatus;
        public int SpeakerStatus;
        public int LEStatus;
        public int WireLanStatus;
        public int DeviceInfoStatus;

        [JsonIgnore]
        public bool IsServerConnected { get { return ServerStatus == 0; } }

        [JsonIgnore]
        public bool IsDeviceConnected { get { return DeviceStatus == 0; } }

        [JsonIgnore]
        public bool IsDatabaseConnected { get { return DatabaseStatus == 0; } }

        [JsonIgnore]
        public bool IsMnisConnected { get { return MnisStatus == 0; } }

        public int DongleCount { get; set; }

        [JsonIgnore]
        public bool IsMicphoneConnected { get { return MicphoneStatus == 0; } }

        [JsonIgnore]
        public bool IsSpeakerConnected { get { return SpeakerStatus == 0; } }

        [JsonIgnore]
        public bool IsLEConnected { get { return LEStatus == 0; } }

        [JsonIgnore]
        public bool IsWireLanConnected { get { return WireLanStatus == 0; } }

        [JsonIgnore]
        public bool IsDeviceInfoUpdated { get { return DeviceInfoStatus == 0; } }

        public new string ToString()
        {
            if (WorkMode == FunctionConfigure.Mode_t.UnKnown) return "离线模式";
            else if (WorkMode == FunctionConfigure.Mode_t.Debug) return "调试模式";

            string serverDept = IsServerConnected ? "" : "服务未连接\t";
            string deviceDept = string.Format("{0}{1}连接{2}\t",
                 WorkMode == FunctionConfigure.Mode_t.VehicleStation || WorkMode == FunctionConfigure.Mode_t.VehicleStationWithMnis ? "车载台" : "中继台",
                 IsDeviceConnected ? "已" : "未",
                 WorkMode == FunctionConfigure.Mode_t.VehicleStation || WorkMode == FunctionConfigure.Mode_t.VehicleStationWithMnis
                        ?
                        (FunctionConfigure.RadioSetting != null ? string.Format("({0})", FunctionConfigure.RadioSetting.Ride.Host) : "")
                        : (FunctionConfigure.RepeaterSetting != null ? string.Format("({0})", FunctionConfigure.RepeaterSetting.Master.Ip) : "")
                );
            string mnisDept = WorkMode != FunctionConfigure.Mode_t.RepeaterWithMnis && WorkMode != FunctionConfigure.Mode_t.VehicleStationWithMnis ? "" :
                string.Format("MNIS{0}连接{1}\t",
                IsMnisConnected ? "已" : "未",
                 (FunctionConfigure.MnisSetting != null ? string.Format("({0})", FunctionConfigure.MnisSetting.Host) : "")
                );

            if (WorkMode == FunctionConfigure.Mode_t.VehicleStation || WorkMode == FunctionConfigure.Mode_t.VehicleStationWithMnis)
            {
                return string.Format("{0}{1}{2}{3}{4}"
                    , serverDept
                    , deviceDept
                    , mnisDept
                    , IsDatabaseConnected ? "" : "数据库未连接\t"
                    , IsDeviceInfoUpdated ? "" : "正在读取设备信息..."
                    );
            }
            else
            {
                return string.Format("{0}{1}{2}{3}{4}{5}{6}{7}{8}{9}"
                    , serverDept
                    , deviceDept
                    , mnisDept
                    , IsDatabaseConnected ? "" : "数据库未连接\t"
                    , DongleCount > 0 ? "" : "Dongle连接异常\t"
                    , IsMicphoneConnected ? "" : "麦克风连接异常\t"
                    , IsSpeakerConnected ? "" : "喇叭连接异常\t"
                    , IsLEConnected ? "" : "LE连接异常\t"
                    , IsWireLanConnected ? "" : "Wirelan连接异常\t"
                    , IsDeviceInfoUpdated ? "" : "正在读取设备信息..."
                    );
            }
        }
    }

    public class ServerStatus
    {
        private volatile static ServerStatus _instance = null;
        private static readonly object lockHelper = new object();

        public event EventHandler StatusChanged;

        public event Action<object, bool> WaitStatusChanged;

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

        public GlobalStatus SystemStatus { get; set; }

        public void SetGlobalStatus(GlobalStatus globalStatus)
        {
            int DatabaseStatus = SystemStatus == null ? 0 : SystemStatus.DatabaseStatus;
            SystemStatus = globalStatus;
            SystemStatus.DatabaseStatus = DatabaseStatus;
            if (StatusChanged != null) StatusChanged(this, null);
        }

        public void SetDatabaseStatus(GlobalStatus globalStatus)
        {
            if (SystemStatus != null)
            {
                SystemStatus.DatabaseStatus = globalStatus.DatabaseStatus;
            }
            else
            {
                SystemStatus = globalStatus;
            }

            if (StatusChanged != null) StatusChanged(this, null);
        }

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
            if (WaitStatusChanged != null) WaitStatusChanged(this, iswait);
        }

        public ServerStatus_t VoiceBusiness
        {
            get
            {
                switch (FunctionConfigure.WorkMode)
                {
                    case FunctionConfigure.Mode_t.Repeater:
                    case FunctionConfigure.Mode_t.RepeaterWithMnis:
                        if (!TServer.IsConnected) return new ServerStatus_t(Repeater.Host, Repeater.Port, false);
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

            public bool IsConnected { get { return _isconnected; } private set { _isconnected = value; } }
            public string Host { get { return _host; } private set { _host = value; } }
            public int Port { get { return _port; } private set { _port = value; } }

            public ServerStatus_t()
            {
            }

            public ServerStatus_t(string host, int port, bool isconnected)
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

        public class VMServerStatus : INotifyPropertyChanged
        {
            private ServerStatus_t _state;

            public VMServerStatus(ServerStatus_t state)
            {
                _state = state;
            }

            public string HostAndPort { get { return _state == null ? "" : string.Format("{0}:{1}", _state.Host, _state.Port); } }
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