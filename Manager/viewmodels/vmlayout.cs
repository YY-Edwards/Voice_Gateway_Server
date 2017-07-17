﻿using System;
using System.ComponentModel;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Manager
{
    public class CVMLayout : INotifyPropertyChanged
    {
        //Command
        #region
        public ICommand Load { get { return new CDelegateCommand(LoadWin); } }
        public ICommand Move { get { return new CDelegateCommand(MoveWin); } }
        public ICommand Minimize { get { return new CDelegateCommand(MinWin); } }
        public ICommand Close { get { return new CDelegateCommand(CloseWin); } }
        public ICommand Exit { get { return new CDelegateCommand(ExitApp); } }
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


        private Visibility m_MaskVisible = Visibility.Hidden;
        public Visibility MaskVisible { get { return m_MaskVisible; } }

        private string m_MaskText = string.Empty;
        public string MaskText { get { return m_MaskText; } }

        #endregion
        public event PropertyChangedEventHandler PropertyChanged;


        private bool m_LogServerConnected = false;

        public bool LogServerConnected { get { return m_LogServerConnected; } }

        private bool m_TServerConnected = false;

        public bool TServerConnected { get { return m_TServerConnected; } }

        //windows Method
        #region

        //configuration
        private CVMBaseSetting m_BaseSettingDataContext = new CVMBaseSetting();
        public CVMBaseSetting BaseSettingDataContext { get { return m_BaseSettingDataContext; } }

        private CVMRadioSetting m_RadioSettingDataContext = new CVMRadioSetting();
        public CVMRadioSetting RadioSettingDataContext { get { return m_RadioSettingDataContext; } }

        private CVMRepeaterSetting m_RepeaterSettingDataContext = new CVMRepeaterSetting();
        public CVMRepeaterSetting RepeaterSettingDataContext { get { return m_RepeaterSettingDataContext; } }


        //resource
        private CVMUser m_UserDataContext = new CVMUser();
        public CVMUser UserDataContext { get { return m_UserDataContext; } }

        private CVMDepartment m_DepartmentDataContext = new CVMDepartment();
        public CVMDepartment DepartmentDataContext { get { return m_DepartmentDataContext; } }

        private CVMStaff m_StaffDataContext = new CVMStaff();
        public CVMStaff StaffDataContext { get { return m_StaffDataContext; } }

        private CVMRadio m_RadioDataContext = new CVMRadio();
        public CVMRadio RadioDataContext { get { return m_RadioDataContext; } }

        private void LoadWin(object parameter)
        {
            if (PropertyChanged != null)
            {              
                PropertyChanged(this, new PropertyChangedEventArgs("BaseSettingDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RadioSettingDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RepeaterSettingDataContext"));

                PropertyChanged(this, new PropertyChangedEventArgs("UserDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("DepartmentDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("StaffDataContext"));
                PropertyChanged(this, new PropertyChangedEventArgs("RadioDataContext"));

            }

            InitializeTServer();
            InitializeLogServer();
        }

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

        private void MoveWin(object parameter)
        {
            if (parameter == null || !(parameter is Main)) return;
            Main mainWindows = parameter as Main;
            mainWindows.DragMove();
        }

        private void MinWin(object parameter)
        {
            if (parameter == null || !(parameter is Main)) return;
            Main mainWindows = parameter as Main;
            mainWindows.WindowState = System.Windows.WindowState.Minimized;
        }

        private void CloseWin(object parameter)
        {
            if (parameter == null || !(parameter is Main)) return;
            Main mainWindows = parameter as Main;
            mainWindows.Close();
        }

        private void ExitApp()
        {
            Environment.Exit(0);
        }

        private void SaveData(object parameter)
        {
            try
            {
                if (parameter == null || !(parameter is Main)) return;
                Main mainwin = parameter as Main;

                Notify notify = new Notify();
                notify.Owner = mainwin;

                new Thread(new ThreadStart(delegate()
                {

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null) 
                        {
                            cmvnotify.SetEnterEnable(false);
                            cmvnotify.AppendNotify("保存基本配置...");
                        }
                    });

                    //save basesetting
                    m_BaseSettingDataContext.Set();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存Radio调度配置...");
                        }
                    });

                    //save radiosetting
                    m_RadioSettingDataContext.Set();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存WireLan调度配置息...");
                        }
                    });

                    //save radiosetting
                    m_RepeaterSettingDataContext.Set();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存用户信息...");
                        }
                    });

                    //save user
                    m_UserDataContext.Set();
                    m_UserDataContext.Get();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存组信息...");
                        }
                    });

                    //save department
                    m_DepartmentDataContext.Set();                                     
                    m_DepartmentDataContext.Get();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存员工信息...");
                        }
                    });

                    //save staff
                    m_StaffDataContext.Set();
                    m_StaffDataContext.AssignDetach(m_DepartmentDataContext.DepartmentList);
                    m_StaffDataContext.Get();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存终端信息...");
                        }
                    });

                    //save radio
                    m_RadioDataContext.Set();
                    m_RadioDataContext.AssignDetach(m_DepartmentDataContext.DepartmentList, m_StaffDataContext.StaffList);
                    m_RadioDataContext.Get();
                    System.Threading.Thread.Sleep(100);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        CVMNotify cmvnotify = (CVMNotify)notify.win_Main.DataContext;
                        if (cmvnotify != null)
                        {
                            cmvnotify.AppendNotify("完成");
                            cmvnotify.AppendNotify("保存成功.");
                            cmvnotify.SetEnterEnable(true);
                        }
                    });
                })).Start();

                m_MaskVisible = Visibility.Visible;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("MaskVisible"));
                notify.ShowDialog();
                m_MaskVisible = Visibility.Hidden;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("MaskVisible"));
            }
            catch
            {
            }
        }

        private void ReConnectServer()
        {
            if(!m_TServerConnected)InitializeTServer();
            if(!m_LogServerConnected) InitializeLogServer();
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

        private double PosInScrView(Visual visual, ScrollViewer scr, FrameworkElement element)
        {
            GeneralTransform transform = element.TransformToVisual(visual);
            double pos = transform.Transform(new Point(element.Margin.Left, element.Margin.Top)).Y;
            return scr.ContentVerticalOffset + pos - 22.5;
        }

        private void ScrollToItemOffset(object parameter)
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
        #endregion

        private void OnTServerChanged(bool isinit)
        {
            m_TServerConnected = isinit;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("TServerConnected"));

            if (isinit)
            {
                m_BaseSettingDataContext.Get();
                m_RadioSettingDataContext.Get();
                m_RepeaterSettingDataContext.Get();
            }
        }

        private void OnLogServerChanged(bool isinit)
        {
            m_LogServerConnected = isinit;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("LogServerConnected"));

            if (isinit)
            {
                m_UserDataContext.Get();
                m_DepartmentDataContext.Get();
                m_StaffDataContext.Get();
                m_RadioDataContext.Get();
            }
        }
    }
}