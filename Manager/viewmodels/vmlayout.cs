using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;

namespace Manager
{
    public class CVMPayout: INotifyPropertyChanged
    {
        //Command
        #region 
        //public ICommand Load { get { return new CDelegateCommand(LoadWin); } }
            
        public ICommand Move { get { return new CDelegateCommand(MoveWin); } }
        public ICommand Minimize { get { return new CDelegateCommand(MinWin); } }
        public ICommand Close { get { return new CDelegateCommand(CloseWin); } }
        public ICommand Exit { get { return new CDelegateCommand(ExitApp); } }



        public ICommand TreeSelectedChanged { get { return new CDelegateCommand(OnTreeSelectedChanged); } }

        public ICommand ConnectServer { get { return new CDelegateCommand(ReConnectServer); } }
        public ICommand ScrollToItem { get { return new CDelegateCommand(ScrollToItemOffset); } }

        public ICommand Save { get { return new CDelegateCommand(SaveConfiguration); } }
        
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


        //windows Method
        #region
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



        //tree
        private void OnTreeSelectedChanged(object index)
        {
            if (index == null || !(index is string)) return;
            try { 
                m_TabIndex = int.Parse(index as string);
            }
            catch
            {
                return;
            }
           
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("TabIndex"));
        }

        private void ReConnectServer(object parameter)
        {
            try
            {
                object[] objs = parameter as object[];
                if (objs == null || objs.Length < 2) return;
                if (objs[0] == null || objs[1] == null) return;
                if (!(objs[0] is CVMConfiguration)) return;

                CVMConfiguration cvmcfg = objs[0] as CVMConfiguration;
                cvmcfg.ConnectConfiguration();
                
            }
            catch
            {

            }
            
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

                scr.ScrollToVerticalOffset(PosInScrView(bdr,scr, dck as FrameworkElement));

            }
            catch
            {

            }
        }


        private void SaveConfiguration(object parameter)
        {
            try
            {
                object[] objs = parameter as object[];
                if (objs == null || objs.Length < 3) return;
                if (objs[0] == null || objs[1] == null || objs[2] == null) return;
                if (!(objs[0] is Main) ||!(objs[1] is CVMConfiguration)) return;

                Main mainwin = objs[0] as Main;
                CVMConfiguration cvmcfg = objs[1] as CVMConfiguration;

                Notify notify = new Notify();
                notify.Owner = mainwin;

                new Thread(new ThreadStart(delegate()
                {
                    mainwin.Dispatcher.BeginInvoke((Action)delegate()           
                    {
                        ((CVMNotify)notify.win_Main.DataContext).SetEnterEnable(false);
                    });
                  
                    
                    try
                    {
                        mainwin.Dispatcher.BeginInvoke((Action)delegate()
                        {
                            ((CVMNotify)notify.win_Main.DataContext).Notify("保存配置信息...");
                        });

                        cvmcfg.Save();
                    }
                    catch
                    {

                    }

                    System.Threading.Thread.Sleep(500);

                    mainwin.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        ((CVMNotify)notify.win_Main.DataContext).AppendNotify("保存完成.");
                        ((CVMNotify)notify.win_Main.DataContext).SetEnterEnable(true);
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

        #endregion

    }
}
