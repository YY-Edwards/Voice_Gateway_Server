using Dispatcher.Service;
using Dispatcher.Views;
using Dispatcher.Modules;
using Sigmar.Extension;
using Sigmar.Logger;
using System;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Collections.Generic;

using Sigmar;

namespace Dispatcher.ViewsModules
{
    public class VMMain : INotifyPropertyChanged
    {
        public event OperatedEventHandler OnViewModulesOperated;

        private VMHeader _headerviewmodule;

        public VMHeader HeaderViewModule {
            get{
                return _headerviewmodule;
            } 
            private set{
                _headerviewmodule = value; 
                NotifyPropertyChanged("HeaderViewModule");
            }
        }

        private VMToolBar _toolbarviewmodule;
        public VMToolBar ToolBarViewModule
        {
            get
            {
                return _toolbarviewmodule;
            }
            private set
            {
                _toolbarviewmodule = value;
                NotifyPropertyChanged("ToolBarViewModule");
            }
        }

        private VMStatus _statusviewmodule;
        public VMStatus StatusViewModule
        {
            get
            {
                return _statusviewmodule;
            }
            private set
            {
                _statusviewmodule = value;
                NotifyPropertyChanged("StatusViewModule");
            }
        }

        private VMNavigation _navigationviewmodule;
        public VMNavigation NavigationViewModule
        {
            get
            {
                return _navigationviewmodule;
            }
            private set
            {
                _navigationviewmodule = value;
                NotifyPropertyChanged("NavigationViewModule");
            }
        }

        private VMResources _resourcesviewmodule;
        public VMResources ResourcesViewModule
        {
            get
            {
                return _resourcesviewmodule;
            }
            private set
            {
                _resourcesviewmodule = value;
                NotifyPropertyChanged("ResourcesViewModule");
            }
        }

        private VMQuickList _quicklistviewmodule;
        public VMQuickList QuickListViewModule
        {
            get
            {
                return _quicklistviewmodule;
            }
            private set
            {
                _quicklistviewmodule = value;
                NotifyPropertyChanged("QuickListViewModule");
            }
        }

        private VMAmap _amapviewmodule;
        public VMAmap AmapViewModule
        {
            get
            {
                return _amapviewmodule;
            }
            private set
            {
                _amapviewmodule = value;
                NotifyPropertyChanged("AmapViewModule");
            }
        }

        public VMMapInDoor _mapindoorviewmodule;
        public VMMapInDoor MapInDoorViewModule
        {
            get
            {
                return _mapindoorviewmodule;
            }
            private set
            {
                _mapindoorviewmodule = value;
                NotifyPropertyChanged("MapInDoorViewModule");
            }
        }

        private VMNotify _notifyviewmodule;
        public VMNotify NotifyViewModule
        {
            get
            {
                return _notifyviewmodule;
            }
            private set
            {
                _notifyviewmodule = value;
                NotifyPropertyChanged("NotifyViewModule");
            }
        }

        public ICommand Loaded { get { return _compositeload; } }
        private CompositeCommand _compositeload;

        public ICommand SizeChanged { get { return _compositesizechanged; } }
        private CompositeCommand _compositesizechanged;
        public VMMain()
        {
            HeaderViewModule = new VMHeader();
            HeaderViewModule.OnOperated += OnOperated;

            ToolBarViewModule = new VMToolBar();
            ToolBarViewModule.OnOperated += OnOperated;

            StatusViewModule = new VMStatus();
            StatusViewModule.OnOperated += OnOperated;

            NavigationViewModule = new VMNavigation();
            NavigationViewModule.OnOperated += OnOperated;

            ResourcesViewModule = new VMResources();
            ResourcesViewModule.OnOperated += OnOperated;

            AmapViewModule = new VMAmap();
            MapInDoorViewModule = new VMMapInDoor();

            QuickListViewModule = new VMQuickList();
            QuickListViewModule.OnOperated += OnOperated;

            NotifyViewModule = new VMNotify();
            NotifyViewModule.OnOperated += OnOperated;

            _compositeload = new CompositeCommand();
            _compositeload.RegisterCommand(HeaderViewModule.WindowLoaded);
            _compositeload.RegisterCommand(WindowLoaded);
            
            _compositesizechanged = new CompositeCommand();
            _compositesizechanged.RegisterCommand(HeaderViewModule.WindowSizeChanged);


            SystemStatus.OnOperated += OnOperated;
            SystemStatus.PropertyChanged += new PropertyChangedEventHandler(OnSystemStatusChagned);
            ResourcesMgr.Instance().OnResourcesLoaded += new EventHandler(OnResourcesLoaded);

            if(_dispatcher == null)
            {
                if(RunAccess.Mode == RunAccess.Mode_t.CPC || RunAccess.Mode == RunAccess.Mode_t.IPSC || RunAccess.Mode == RunAccess.Mode_t.LCP)
                {
                    _dispatcher = RepeaterDispatcher.Instance();
                }
                else if (RunAccess.Mode == RunAccess.Mode_t.VehicleStation || RunAccess.Mode == RunAccess.Mode_t.VehicleStationWithMnis)
                {
                    _dispatcher = VehicleStationDispatcher.Instance();
                }
                else
                {
                    //for test
                    _dispatcher = VehicleStationDispatcher.Instance();
                }
            }
        }

        private void OnSystemStatusChagned(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "CallStatus")
            {
                NotifyPropertyChanged("RX");
            }
        }

        private void OnResourcesLoaded(object sender, EventArgs e)
        {
            if (m_TServerConnected && _dispatcher != null)
            {
                _dispatcher.GetStatus();
                _dispatcher.GetOnlineList();
            }
        }

        
        public ICollectionView RX { get { return new ListCollectionView(SystemStatus.RX); } }
        public ICommand WindowLoaded { get { return new Command(WindowLoadedExec); } }
        private void WindowLoadedExec(object parameter)
        {
            CTServer.Instance().OnStatusChanged += OnTServerChanged;
            CLogServer.Instance().OnStatusChanged += OnLogServerChanged;

            InitializeTServer();
            InitializeLogServer();
        }

        private CDispatcher _dispatcher;


        private bool m_LogServerConnected = false;
        public bool LogServerConnected { get { return m_LogServerConnected; } }

        private bool m_TServerConnected = false;

        public bool TServerConnected { get { return m_TServerConnected; } }
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
            ServerStatus.Instance().TServer.SetStatus(m_TServerConnected);         
        }

        private void OnLogServerChanged(bool isinit)
        {
            Log.Info(String.Format("LogServer Status:{0}", isinit ? "Connected" : "Disconnected"));
            m_LogServerConnected = isinit;            
            ServerStatus.Instance().LogServer.SetStatus(m_LogServerConnected);
        }
        private void OnOperated(OperatedEventArgs e)
        {
            if(e.Operate >= OperateType_t.WindowMove && e.Operate <= OperateType_t.WindowClose)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenNewOperateWindow)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.NavigationWork)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenOperateWindow)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenNewFastWindow)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenEvent || e.Operate == OperateType_t.CloseEvent)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenNotify || e.Operate == OperateType_t.CloseNotify)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
                _notifyviewmodule.UpdateView(e);
                _headerviewmodule.MenuViewModule.UpdateNotifyOption.Execute(e);
            }
            else if (e.Operate == OperateType_t.CloseAllNotify)
            {
                if (OnViewModulesOperated != null) OnViewModulesOperated(e);
            }
            else if (e.Operate == OperateType_t.OpenTools || e.Operate == OperateType_t.CloseTools)
            {
                _toolbarviewmodule.UpdateView(e);
            }
            else if (e.Operate == OperateType_t.OpenNavigation || e.Operate == OperateType_t.CloseNavigation)
            {
                _navigationviewmodule.UpdateView(e);
            }          
            else if (e.Operate == OperateType_t.SetCurrentTarget)
            {
                SetCurrentTarget(e.parameter as VMTarget);
            }
            else if (e.Operate == OperateType_t.NewFast)
            {
                _quicklistviewmodule.AddQuick.Execute(e.parameter);
            }
            else if(e.Operate == OperateType_t.AddNotify)
            {
                _notifyviewmodule.Add.Execute(e.parameter);
            }
            else if(e.Operate == OperateType_t.DrawLocationPoint)
            {
                _amapviewmodule.DrawPoint.Execute(e.parameter);
            }
            else if(e.Operate == OperateType_t.SaveWorkspace)
            {
                SaveWork.Instance().Save(this);
            }           
            else
            {
                Log.Warning(string.Format("Unexpected Operation Command:{0}({1})", e.Operate.ToString(), e.parameter != null ?  e.parameter.ToString()  : "null"));
            }
        }


        private void SetCurrentTarget(VMTarget target)
        {
            _headerviewmodule.MenuViewModule.SetTarget.Execute(target);
            _toolbarviewmodule.SetTarget.Execute(target);
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