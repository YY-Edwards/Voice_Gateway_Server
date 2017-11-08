using System;
using System.ComponentModel;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Interop;
using System.Collections.Generic;

using Sigmar;

namespace Manager
{
    public class CVMLayout : INotifyPropertyChanged
    {



        //Command
        #region
        public ICommand Load { get { return new CDelegateCommand(LoadWin); } }
       
        public ICommand Save { get { return new CDelegateCommand(SaveData); } }

        public ICommand ConnectServer { get { return new CDelegateCommand(ReConnectServer); } }

        public ICommand GetConfiguration { get { return new CDelegateCommand(GetFunc); } }
        public ICommand GetResource { get { return new CDelegateCommand(GetFunc); } }

       
        public ICommand TreeSelectedChanged { get { return new CDelegateCommand(OnTreeSelectedChanged); } }
       
        public ICommand ScrollToItem { get { return new CDelegateCommand(ScrollToItemOffset); } }


        #endregion

        //Property
        #region


        private int m_TabIndex = 0;
        public int TabIndex { get { return m_TabIndex; } }


        #endregion
        public event PropertyChangedEventHandler PropertyChanged;


        private bool m_LogServerConnected = false;

        public bool LogServerConnected { get { return m_LogServerConnected; } }

        private bool m_TServerConnected = false;

        public bool TServerConnected { get { return m_TServerConnected; } }

        //windows Method
        #region

        //configuration
        //private CVMBaseSetting m_BaseSettingDataContext = new CVMBaseSetting();
        //public CVMBaseSetting BaseSettingDataContext { get { return m_BaseSettingDataContext; } }

        //private CVMRadioSetting m_RadioSettingDataContext = new CVMRadioSetting();
        //public CVMRadioSetting RadioSettingDataContext { get { return m_RadioSettingDataContext; } }

        //private CVMRepeaterSetting m_RepeaterSettingDataContext = new CVMRepeaterSetting();
        //public CVMRepeaterSetting RepeaterSettingDataContext { get { return m_RepeaterSettingDataContext; } }


        //private CVMRegister m_RegisterDataContext = new CVMRegister();
        //public CVMRegister RegisterDataContext { get { return m_RegisterDataContext; } }

        //private CVMLocationInDoorSetting m_LocationInDoorDataContext = new CVMLocationInDoorSetting();
        //public CVMLocationInDoorSetting LocationInDoorDataContext { get { return m_LocationInDoorDataContext; } }
      
        //resource
        //private CVMUser m_UserDataContext = new CVMUser();
        //public CVMUser UserDataContext { get { return m_UserDataContext; } }

        //private CVMDepartment m_DepartmentDataContext = new CVMDepartment();
        //public CVMDepartment DepartmentDataContext { get { return m_DepartmentDataContext; } }

        //private CVMStaff m_StaffDataContext = new CVMStaff();
        //public CVMStaff StaffDataContext { get { return m_StaffDataContext; } }

        //private CVMRadio m_RadioDataContext = new CVMRadio();
        //public CVMRadio RadioDataContext { get { return m_RadioDataContext; } }

        //private CVMArea m_AreaDataContext = new CVMArea();
        //public CVMArea AreaDataContext { get { return m_AreaDataContext; } }

        //private CVMiBeacon m_iBeaconDataContext = new CVMiBeacon();
        //public CVMiBeacon iBeaconDataContext { get { return m_iBeaconDataContext; } }

        private void LoadWin(object parameter)
        {
            if (PropertyChanged != null)
            {              
                PropertyChanged(this, new PropertyChangedEventArgs("BaseSettingDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RadioSettingDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RepeaterSettingDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RegisterDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("LocationInDoorDataContext"));

                PropertyChanged(this, new PropertyChangedEventArgs("UserDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("DepartmentDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("StaffDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RadioDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("AreaDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconDataContext"));
            }

            //InitializeTServer();
            //InitializeLogServer();

            if (parameter == null) return;

            hs = PresentationSource.FromVisual((Visual)parameter) as HwndSource;
            hs.AddHook(new HwndSourceHook(WndProc));
        }

        private HwndSource hs;

        List<int> log = new List<int>();
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {

            log.Add(msg);
            switch (msg)
            {
                case 0x0202:
                case 0x0205:
                    //
                    break;
                case 0x8023:
                  
                    //m_iBeaconDataContext.UpdateiBeacon();
                    break;
                default: break;
            }
            return (System.IntPtr)0;
        }



        //private void InitializeTServer()
        //{
        //    if (!CTServer.Instance().IsInitialized)
        //    {
        //        CTServer.Instance().OnReceiveRequest += delegate { };
                //CTServer.Instance().OnStatusChanged += OnTServerChanged;
        //        CTServer.Instance().Timeout += OnTimeout;
        //    }
        //    CTServer.Instance().Initialize();  

        //}

        //private void InitializeLogServer()
        //{
        //    if (!CLogServer.Instance().IsInitialized)
        //    {
        //        CLogServer.Instance().OnReceiveRequest += delegate { };
        //        CLogServer.Instance().OnStatusChanged += OnLogServerChanged;
        //        CLogServer.Instance().Timeout += OnTimeout;
        //    }

        //    CLogServer.Instance().Initialize();
        //}

        private bool istimeout = false;
        private void  OnTimeout(object sender, EventArgs e)
        {
            istimeout = true;
        }


        private void SaveData(object parameter)
        {
            try
            {
                //if (parameter == null || !(parameter is Main)) return;
                //Main mainwin = parameter as Main;

                //Notify notify = new Notify();
                //notify.Owner = mainwin;

                new Thread(new ThreadStart(delegate()
                {

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null) 
                    //    {
                    //        cmvnotify.SetEnterEnable(false);
                    //        cmvnotify.AppendNotify("保存基本配置...");
                    //    }
                    //});

                    //save basesetting
                    //m_BaseSettingDataContext.Set();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存Radio调度配置...");
                    //    }
                    //});

                    //save radiosetting
                    //m_RadioSettingDataContext.Set();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存WireLan调度配置...");
                    //    }
                    //});

                    //save repeatersetting
                    //m_RepeaterSettingDataContext.Set();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存室内定位配置...");
                    //    }
                    //});

                    //save locationinddoorsetting
                    //m_LocationInDoorDataContext.Set();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存用户信息...");
                    //    }
                    //});

                    //save user
                    //m_UserDataContext.Set();
                    //Thread.Sleep(100);
                    //m_UserDataContext.Get();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存组信息...");
                    //    }
                    //});

                    //save department
                    //m_DepartmentDataContext.Set();
                    //Thread.Sleep(100);                
                    //m_DepartmentDataContext.Get();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存员工信息...");
                    //    }
                    //});

                    //save staff
                    //m_StaffDataContext.Set();
                    //Thread.Sleep(100);
                    //m_StaffDataContext.AssignDetach(m_DepartmentDataContext.DepartmentList);
                    //m_StaffDataContext.Get();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存终端信息...");
                    //    }
                    //});

                    //save radio
                    //m_RadioDataContext.Set();
                    //Thread.Sleep(100);
                    //m_RadioDataContext.AssignDetach(m_DepartmentDataContext.DepartmentList, m_StaffDataContext.StaffList);
                    //m_RadioDataContext.Get();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存室内定位区域信息...");
                    //    }
                    //});

                    //save area
                    //m_AreaDataContext.Set();
                    //Thread.Sleep(100);
                    //m_AreaDataContext.Get();
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存信标（iBeacons）信息...");
                    //    }
                    //});

                    //save ibeacon
                    //m_iBeaconDataContext.Set();
                    //Thread.Sleep(100);
                    //m_iBeaconDataContext.Get();
                   
                    //System.Threading.Thread.Sleep(100);

                    //mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    //{
                    //    CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                    //    if (cmvnotify != null)
                    //    {
                    //        cmvnotify.AppendNotify("完成");
                    //        cmvnotify.AppendNotify("保存成功.");
                    //        cmvnotify.SetEnterEnable(true);
                    //    }
                    //});
                })).Start();
            }
            catch
            {
            }
        }

        private void ReConnectServer()
        {
            //if(!m_TServerConnected)InitializeTServer();
            //if(!m_LogServerConnected) InitializeLogServer();
        }


        private void GetFunc(object parameter)
        {
            try
            {
                object[] objs = parameter as object[];
                if (objs == null || objs.Length < 1) return;

                for (int i = 1; i < objs.Length; i++)
                {
                    if (objs[i] == null) continue;
                    try
                    {
                        ((CVMManager)objs[i]).Get();
                    }
                    catch
                    {
                    }
                }
            }
            catch
            {
            }
        }


        //tree
        private void OnTreeSelectedChanged(object index)
        {
            if (index == null || !(index is string)) return;
            try
            {
                m_TabIndex = int.Parse(index as string);
            }
            catch
            {
                return;
            }

            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("TabIndex"));
        }

        public static void ScrollToOffset(object parameter)
        {
            try
            {
                object[] objs = parameter as object[];
                if (objs == null || objs.Length < 3) return;
                if (objs[0] == null || objs[1] == null || objs[2] == null) return;
                if (!(objs[0] is Border) || !(objs[1] is ScrollViewer) || !(objs[2] is DockPanel)) return;

                Border bdr = objs[0] as Border;
                ScrollViewer scr = objs[1] as ScrollViewer;
                DockPanel dck = objs[2] as DockPanel;

                scr.ScrollToVerticalOffset(PosInScrView(bdr, scr, dck as FrameworkElement));
            }
            catch
            {
            }
        }

        public static double PosInScrView(Visual visual, ScrollViewer scr, FrameworkElement element)
        {
            GeneralTransform transform = element.TransformToVisual(visual);
            double pos = transform.Transform(new Point(element.Margin.Left, element.Margin.Top)).Y;
            return scr.ContentVerticalOffset + pos - 22.5;
        }

        private void ScrollToItemOffset(object parameter)
        {
            ScrollToOffset(parameter);           
        }
        #endregion

        private void OnTServerChanged(bool isinit)
        {
            m_TServerConnected = isinit;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("TServerConnected"));

            if (isinit)
            {
                //m_BaseSettingDataContext.Get();
                //Thread.Sleep(100);
                //m_RadioSettingDataContext.Get();
                //Thread.Sleep(100);
                //m_RepeaterSettingDataContext.Get();
                //Thread.Sleep(100);
                //m_LocationInDoorDataContext.Get();
                //Thread.Sleep(100);

                //m_RegisterDataContext.Get();
                //Thread.Sleep(100);
            }
        }

        private void OnLogServerChanged(bool isinit)
        {
            m_LogServerConnected = isinit;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("LogServerConnected"));

            if (isinit)
            {
                //m_UserDataContext.Get();
                //Thread.Sleep(100);
                //m_DepartmentDataContext.Get();
                //Thread.Sleep(100);
                //m_StaffDataContext.Get();
                //Thread.Sleep(100);
                //m_RadioDataContext.Get();
                //Thread.Sleep(100);

                //m_AreaDataContext.Get();
                //Thread.Sleep(100);
                //m_iBeaconDataContext.Get();
                //Thread.Sleep(100);
            }
        }
    }
}