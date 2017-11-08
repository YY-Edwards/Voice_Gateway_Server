using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using Manager.Models;

namespace Manager.ViewModels
{
    public class MainViewModel : INotifyPropertyChanged
    {

        private bool _logServerConnected;
        private bool _tServerConnected;

        public ManageListViewModel ManageListViewModel { get; private set;}

        public MainViewModel()
        {
            _logServerConnected = false;
            _tServerConnected = false;

            TServer.Instance().StatusChanged += new Action<object, bool>(OnTServerStatusChanged);
            LogServer.Instance().StatusChanged += new Action<object, bool>(OnLogServerStatusChanged);

            if (ManageListViewModel == null) ManageListViewModel = new ManageListViewModel();

        }

        public string ServerStatus { get { return !_logServerConnected || !_tServerConnected ? "服务未连接" : "已连接服务"; } }
        public Visibility ReconnectBtnVisable { get { return !_logServerConnected || !_tServerConnected ? Visibility.Visible : Visibility.Collapsed; } }

        public ICommand ConnectServer
        {
            get
            {
                return new Command(() =>
                {
                    if (!_tServerConnected) InitializeTServer();
                    if (!_logServerConnected) InitializeLogServer();
                });
            }
        }

        public ICommand SaveConfigure
        {
            get
            {
                return new Command(() =>
                {
                    if (_tServerConnected && _logServerConnected) ManageListViewModel.SaveManagement.Execute(null);                    
                });
            }
        }

       
        private void InitializeTServer()
        {
            _tServerConnected = false;
            TServer.Instance().InitializeServer();
        }

        private readonly object _serverStatusChangedLocakHelper = new object();
        private void OnTServerStatusChanged(object sender, bool isconnected)
        {
            lock (_serverStatusChangedLocakHelper)
            { 
                _tServerConnected = isconnected;
                NotifyPropertyChanged(new string[] { "ServerStatus", "ReconnectBtnVisable" });
                if (_tServerConnected && _logServerConnected)
                {
                    ManageListViewModel.ReadManagement.Execute(null);
                }
            }  
        }

        private void InitializeLogServer()
        {
            _logServerConnected = false;
            LogServer.Instance().InitializeServer();
        }

        private void OnLogServerStatusChanged(object sender, bool isconnected)
        {
            lock (_serverStatusChangedLocakHelper)
            {
                _logServerConnected = isconnected;
                NotifyPropertyChanged(new string[] { "ServerStatus", "ReconnectBtnVisable" });

                if (_tServerConnected && _logServerConnected)
                {
                    ManageListViewModel.ReadManagement.Execute(null);
                }
            }  
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;
        protected void NotifyPropertyChanged(string propertyName)
        {
            if (propertyName != null && this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        protected void NotifyPropertyChanged(string[] propertyNames)
        {
            if (propertyNames != null && this.PropertyChanged != null)
            {
                foreach (string name in propertyNames) if (name != null) this.PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }

        #endregion
    }
}
