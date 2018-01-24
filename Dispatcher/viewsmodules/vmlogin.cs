using Dispatcher.Modules;
using Dispatcher.Service;
using Sigmar;
using Dispatcher;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Controls;
using System.Windows.Input;

namespace Dispatcher.ViewsModules
{
    public class VMLogin : INotifyPropertyChanged
    {
        public event EventHandler OnLoginOK;

        public event EventHandler InitializeCompleted;

        private string _info;
        public string Info { get { return _info; } set { _info = value; NotifyPropertyChanged("Info"); } }

        private string initilizeContents = "";
        public string InitilizeContents { get { return initilizeContents; } set { initilizeContents = value; NotifyPropertyChanged("InitilizeContents"); } }

        private bool _initilizeCanClose = false;
        public bool InitilizeCanClose { get { return _initilizeCanClose; } private set { _initilizeCanClose = value; NotifyPropertyChanged("InitilizeCanClose"); } }

        public ICommand CloseInitilize
        {
            get
            {
                return new Command(() =>
                {
                    Environment.Exit(0);
                });
            }
        }

        private void AddInitilizeContents(string content)
        {
            if (this.InitilizeContents == "") InitilizeContents = content;
            else InitilizeContents += "\r\n" + content;
        }

        public string UserName { get; set; }
        private Semaphore _waitlogin;

        public ICommand Loaded { get { return new Command(LoadedExec); } }

        private CUserMgr _user;
        private CRegister _register;

        private CBaseSetting _base;
        private CRadioSetting _vehiclestation;
        private CRepeaterSetting _repeater;

        private CMnisSetting _mnis;
        private CLocationSetting _location;
        private CLocationInDoorSetting _locationInDoor;

        private void LoadedExec(object parameter)
        {
            InitilizeCanClose = false;

            if (_user == null)
            {
                _user = CUserMgr.Instance();
                _user.OnLoginResult += new LoginResultHandler(OnLoginResult);
                _user.Timeout += new EventHandler(OnTimeout);
            }
            if (_register == null)
            {
                _register = new CRegister();
                _register.OnQuery += new CRegister.OnRegiserHandle(OnQuery);
                _register.Timeout += new EventHandler(OnTimeout);
            }

            if (_base == null)
            {
                _base = new CBaseSetting();
                _base.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _base.Timeout += new EventHandler(OnTimeout);
            }

            if (_vehiclestation == null)
            {
                _vehiclestation = new CRadioSetting();
                _vehiclestation.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _vehiclestation.Timeout += new EventHandler(OnTimeout);
            }

            if (_repeater == null)
            {
                _repeater = new CRepeaterSetting();
                _repeater.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _repeater.Timeout += new EventHandler(OnTimeout);
            }

            if (_mnis == null)
            {
                _mnis = new CMnisSetting();
                _mnis.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _mnis.Timeout += new EventHandler(OnTimeout);
            }

            if (_location == null)
            {
                _location = new CLocationSetting();
                _location.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _location.Timeout += new EventHandler(OnTimeout);
            }

            if (_locationInDoor == null)
            {
                _locationInDoor = new CLocationInDoorSetting();
                _locationInDoor.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _locationInDoor.Timeout += new EventHandler(OnTimeout);
            }

            StartMonitorServer();
            Log.Debug("StartUp Monitor.");

            new System.Threading.Tasks.Task(ConnectServer).Start();
        }

        private void ConnectServer()
        {
            Log.Debug("Initialize TServer and LogServer.");
            while (true)
            {
                if (!m_TServerConnected)
                {
                    InitializeTServer();
                }
                if (!m_LogServerConnected)
                {
                    InitializeLogServer();
                }

                Thread.Sleep(1000);
            }
        }

        private void OnTimeout(object sender, EventArgs e)
        {
            _issuccess = false;
            Info = "指令超时";
            AddInitilizeContents("指令超时...");
            Log.Error("Communication Timeout");
            try
            {
                _waitlogin.Release();
            }
            catch (Exception ex)
            {
                Log.Warning("Wait Login Release Semaphore Failure", ex);
            }
        }

        public ICommand Login { get { return new Command(LoginExec); } }

        private void LoginExec(object parameter)
        {
            Info = "";

            if (UserName == null || UserName == "")
            {
                Info = "请输入用户名";
                Log.Debug("Must Input Username Begin Login.");
                return;
            }

            if (parameter == null || !(parameter is PasswordBox)) return;

            PasswordBox psdbox = parameter as PasswordBox;
            string psd = psdbox.Password;
            psdbox.Password = "";
            LoginAPP(UserName, psd);
        }

        private bool _issuccess = true;

        private void OnLoginResult(LoginResultArgs e)
        {
            _issuccess = e.IsSuccess;
            if (!_issuccess)
            {
                Info = "登陆失败，账号或密码错误";
                Log.Error("Login Failure, UID or PSD Error");
            }
            _waitlogin.Release();
        }

        private void OnQuery(bool success, CRegister res)
        {
            _issuccess = success;
            if (!_issuccess)
            {
                Info = "软件验证失败";
                AddInitilizeContents("软件验证失败...");
                Log.Error("Login Failure, Unregister");
            }
            _waitlogin.Release();
        }

        private void OnConfigurationChanged(SettingType type, object config)
        {
            if (type == SettingType.Base)
            {
                CBaseSetting setting = config as CBaseSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取基本配置信息完成...");
                    FunctionConfigure.SetBaseSetting(setting);
                }
            }
            else if (type == SettingType.Radio)
            {
                CRadioSetting setting = config as CRadioSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取车载台配置信息完成...");
                    FunctionConfigure.SetRadioSetting(setting);
                    ServerStatus.Instance().VehicleStation = new ServerStatus.ServerStatus_t(setting.Ride.Host, setting.Ride.MessagePort, false);
                }
            }
            else if (type == SettingType.Repeater)
            {
                CRepeaterSetting setting = config as CRepeaterSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取中继台配置信息完成...");
                    FunctionConfigure.SetRepeaterSetting(setting);
                    ServerStatus.Instance().Repeater = new ServerStatus.ServerStatus_t(setting.Master.Ip, setting.Master.Port, false);
                }
            }
            else if (type == SettingType.Mnis)
            {
                CMnisSetting setting = config as CMnisSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取MNIS配置信息完成...");
                    FunctionConfigure.SetMnisSetting(setting);
                    ServerStatus.Instance().Mnis = new ServerStatus.ServerStatus_t(setting.Host, setting.MessagePort, false);
                }
            }
            else if (type == SettingType.Location)
            {
                CLocationSetting setting = config as CLocationSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取GPS位置查询配置信息完成...");
                    FunctionConfigure.SetLocationSetting(setting);
                }
            }
            else if (type == SettingType.LocationInDoor)
            {
                CLocationInDoorSetting setting = config as CLocationInDoorSetting;
                if (setting != null)
                {
                    AddInitilizeContents("读取室内位置查询配置信息完成...");
                    FunctionConfigure.SetLocationInDoorSetting(setting);
                }
            }

            _waitlogin.Release();
        }

        private void LoginAPP(string uid, string psd)
        {
            if (!m_LogServerConnected)
            {
                Info = "未连接到服务";
                Log.Fatal("Login Failure.Should Connect TServer and LogServer Begin Login.");
                return;
            }
            FunctionConfigure.CurrentUser = uid;

            new Thread(new ThreadStart(delegate()
            {
                _waitlogin = new Semaphore(0, 1);

                _user.Auth(uid, psd);
                _waitlogin.WaitOne(10 * 1000);
                if (!_issuccess)
                {
                    return;
                }

                if (OnLoginOK != null) OnLoginOK(this, new EventArgs());

                if (!m_TServerConnected)
                {
                    AddInitilizeContents("初始化失败，未连接服务");
                    InitilizeCanClose = true;
                    return;
                }

                AddInitilizeContents("账号验证成功...");
                AddInitilizeContents("获取注册信息...");
                _register.Get();
                _waitlogin.WaitOne(10 * 1000);
                if (!_issuccess)
                {
                    AddInitilizeContents("获取注册信息失败...");
                    InitilizeCanClose = true;
                    return;
                }

                if (!ReadSetting())
                {
                    AddInitilizeContents("读取配置信息失败...");
                    InitilizeCanClose = true;
                    return;
                }

                FunctionConfigure.InitilizeSystemConfiguration();

                Log.Info("Login Success.");
                Log.Info(String.Format("Work Mode:{0}.", FunctionConfigure.WorkMode.ToString()));

                CTServer.Instance().OnStatusChanged -= OnTServerChanged;
                CLogServer.Instance().OnStatusChanged -= OnLogServerChanged;

                AddInitilizeContents("初始化完成...");

                if (InitializeCompleted != null) InitializeCompleted(this, new EventArgs());
            })).Start();
        }

        private bool ReadSetting()
        {
            AddInitilizeContents("获取基本配置信息...");
            _base.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;

            AddInitilizeContents("获取车载台配置信息...");
            _vehiclestation.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;

            AddInitilizeContents("获取中继台配置信息...");
            _repeater.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;

            AddInitilizeContents("获取MNIS配置信息...");
            _mnis.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;

            AddInitilizeContents("获取GPS位置查询配置信息...");
            _location.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;
            AddInitilizeContents("获取室内位置查询配置信息...");
            _locationInDoor.Get();
            _waitlogin.WaitOne(10 * 1000);
            if (!_issuccess) return false;

            return true;
        }

        private bool m_LogServerConnected = false;
        private bool m_TServerConnected = false;

        private void InitializeTServer()
        {
            if (!CTServer.Instance().IsInitialized)
            {
                CTServer.Instance().OnReceiveRequest += delegate { };
                CTServer.Instance().OnStatusChanged += OnTServerChanged;
            }
            CTServer.Instance().Initialize();
        }

        private void InitializeLogServer()
        {
            if (!CLogServer.Instance().IsInitialized)
            {
                CLogServer.Instance().OnReceiveRequest += delegate { };
                CLogServer.Instance().OnStatusChanged += OnLogServerChanged;
            }

            CLogServer.Instance().Initialize();
        }

        private void OnTServerChanged(bool isinit)
        {
            Log.Info(String.Format("TServer Status:{0}", isinit ? "Connected" : "Disconnected"));
            m_TServerConnected = isinit;
            ServerStatus.Instance().TServer.SetHostAndPort(CTServer.Instance().Host, CTServer.Instance().Port);
            ServerStatus.Instance().TServer.SetStatus(m_TServerConnected);
        }

        private void OnLogServerChanged(bool isinit)
        {
            Log.Info(String.Format("LogServer Status:{0}", isinit ? "Connected" : "Disconnected"));
            m_LogServerConnected = isinit;
            ServerStatus.Instance().LogServer.SetHostAndPort(CLogServer.Instance().Host, CLogServer.Instance().Port);
            ServerStatus.Instance().LogServer.SetStatus(m_LogServerConnected);
        }

        private void StartMonitorServer()
        {
            Process[] processes;
            processes = Process.GetProcessesByName("Monitor");

            if (processes.Count() > 0) return;
            else
            {
                if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + "../Svr/Monitor.exe"))
                {
                    System.Diagnostics.Process.Start(AppDomain.CurrentDomain.BaseDirectory + "../Svr/Monitor.exe");
                }
                else
                {
                    Log.Error(String.Format("Can not found: {0}../Svr/Monitor.exe", AppDomain.CurrentDomain.BaseDirectory));
                }
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

        #endregion INotifyPropertyChanged Members
    }
}