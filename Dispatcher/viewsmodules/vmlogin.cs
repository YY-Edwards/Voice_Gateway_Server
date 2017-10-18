using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Controls;

using System.Diagnostics;
using System.IO;

using Sigmar;
using Sigmar.Extension;
using Dispatcher.Views;
using Dispatcher.Service;
using Dispatcher.Modules;


using System.Threading;

using Sigmar.Logger;


namespace Dispatcher.ViewsModules
{
    public class VMLogin : INotifyPropertyChanged
    {

        public event EventHandler OnLoginOK;

        private string _info;
        public string Info { get { return _info; } set { _info = value; NotifyPropertyChanged("Info"); } }

        public string UserName { get; set;}
        private Semaphore _waitlogin;

        public ICommand Loaded { get { return new Command(LoadedExec); } }


        private CUserMgr _user ;
        private CRegister _register;

        private CRepeaterSetting _repeater;
        private CRadioSetting _vehiclestation;
       


        private void LoadedExec(object parameter)
        {
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

            if (_repeater == null)
            {
                _repeater = new CRepeaterSetting();
                _repeater.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _repeater.Timeout += new EventHandler(OnTimeout);
            }

            if (_vehiclestation == null)
            {
                _vehiclestation = new CRadioSetting();
                _vehiclestation.OnConfigurationChanged += new CConfiguration.ConfigurationChangedHandle(OnConfigurationChanged);
                _repeater.Timeout += new EventHandler(OnTimeout);
            }
           
            StartMonitorServer();
            Log.Debug("StartUp Monitor.");

            
            InitializeTServer();
            InitializeLogServer();
            Log.Debug("Initialize TServer and LogServer.");
        }

        private void OnTimeout(object sender, EventArgs e)
        {
            _issuccess = false;
            Info = "指令超时";
            Log.Error("Communication Timeout");
            try
            {
                _waitlogin.Release();
            }
            catch(Exception ex)
            {
                Log.Warning("Release Semaphore Failure", ex);
            }
        }

        public ICommand Login { get { return new Command(LoginExec); } }
        private void LoginExec(object parameter)
        {
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
             _issuccess =success;
             if (!_issuccess)
             {
                 Info = "软件验证失败";
                 Log.Error("Login Failure, Unregister");
             }
            _waitlogin.Release();
        }

        private void OnConfigurationChanged(SettingType type, object config)
        {
            if(type == SettingType.Repeater)
            {
                CRepeaterSetting repeatercfg = config as CRepeaterSetting;
                if (repeatercfg != null && repeatercfg.IsEnable)
                {
                    switch( repeatercfg.Type)
                    {
                        case WireLanType.IPSC:RunAccess.Mode = RunAccess.Mode_t.IPSC;break;
                        case WireLanType.CPC: RunAccess.Mode = RunAccess.Mode_t.CPC; break;
                        case WireLanType.LCP: RunAccess.Mode = RunAccess.Mode_t.LCP; break;
                    }

                    ServerStatus.Instance().Repeater = new ServerStatus.ServerStatus_t(repeatercfg.Master.Ip, repeatercfg.Master.Port,false);
                    ServerStatus.Instance().Mnis = new ServerStatus.ServerStatus_t(repeatercfg.Mnis.Host, repeatercfg.Mnis.MessagePort,false);
                }
            }
            else if(type == SettingType.Radio)
            {
                CRadioSetting radiocfg = config as CRadioSetting;
                if (radiocfg != null && radiocfg.IsEnable)
                {
                    if (radiocfg.IsOnlyRide) RunAccess.Mode = RunAccess.Mode_t.VehicleStation;
                    else RunAccess.Mode = RunAccess.Mode_t.VehicleStationWithMnis;

                    ServerStatus.Instance().VehicleStation = new ServerStatus.ServerStatus_t(radiocfg.Ride.Host, radiocfg.Ride.MessagePort,false);
                    ServerStatus.Instance().Mnis = new ServerStatus.ServerStatus_t(radiocfg.Mnis.Host, radiocfg.Mnis.MessagePort,false);                  
                }
            }



            _waitlogin.Release();
        }

  

        private void LoginAPP(string uid, string psd)
        {
            if (!m_LogServerConnected || !m_TServerConnected)
            {
                Info = "未连接到服务";
                Log.Fatal("Login Failure.Should Connect TServer and LogServer Begin Login.");
                return;
            }

            new Thread(new ThreadStart(delegate()
            {

                _waitlogin = new Semaphore(0, 1);

                //_user.Auth(uid, psd);
                //_waitlogin.WaitOne();
                //if (!_issuccess) return;

                //_register.Get();
                //_waitlogin.WaitOne();
                //if (!_issuccess) return;


                RunAccess.Mode = RunAccess.Mode_t.None;
                _repeater.Get();
                _waitlogin.WaitOne();
                if (!_issuccess) return;
                _vehiclestation.Get();
                _waitlogin.WaitOne();
                if (!_issuccess) return;

                Log.Info("Login Success.");
                Log.Info(String.Format("Work Mode:{0}.", RunAccess.Mode.ToString()));

                CTServer.Instance().OnStatusChanged -= OnTServerChanged;
                CLogServer.Instance().OnStatusChanged -= OnLogServerChanged;


                if (OnLoginOK != null) OnLoginOK(this, new EventArgs());
            })).Start();
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
            Log.Info(String.Format("TServer Status:{0}", isinit ? "Connected": "Disconnected"));
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

        #endregion
    }
}
