using Dispatcher.Service;
using Dispatcher.ViewsModules;
using System.Collections.Generic;
using Sigmar.Logger;
using Sigmar.Windows;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Linq;
using Dispatcher.Modules;
using Dispatcher.Controls;
using System.ComponentModel;

using System.Windows.Automation.Peers;

using Dispatcher.Service;

namespace Dispatcher.Views
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class Main : BaseWindow
    {
        private VMMain MainViewModule;

        public Main()
        {
            //RunAccess.Mode = RunAccess.Mode_t.All;

            InitializeComponent();

#if false
            MainViewModule = new VMMain();
            this.DataContext = MainViewModule;
#else
            MainViewModule = this.DataContext as VMMain;

#endif

            this.Loaded += new RoutedEventHandler(window_Load);
            this.Closing+= new CancelEventHandler(window_closing);
            this.Closed += new EventHandler(window_Closed);
        }

        private void window_Load(object sender, RoutedEventArgs e)
        {
            Runtime.Instance().OnNotify += AddEvent;
            Log.BindingMessage(OnLogNotify);

            updatelayout();

            Runtime.Notify("TrboX3.1 登陆成功");
        }

        private void OnLogNotify(LogContent log)
        {
            LogMode_t logMode = log.GetMode();

            if (logMode == LogMode_t.MESSAGE) AddEvent(new Runtime(log.Message));
        }

        private void window_closing(object sender, CancelEventArgs e)
        {
            if(SaveWork.Instance().EnableSaveWorkSpace)
            {
                Message _messagewindow = new Message();
                _messagewindow.Owner = this;
                mask.Visibility = System.Windows.Visibility.Visible;
                _messagewindow.ShowDialog();
                mask.Visibility = System.Windows.Visibility.Hidden;

                if (_messagewindow.Result == Message.ResultType.Cancel) e.Cancel = true;
                if(_messagewindow.Result == Message.ResultType.Enter)
                {
                    SaveWork.Instance().Save(MainViewModule);
                }
            }
        }
        private void window_Closed(object sender, EventArgs e)
        {
            Log.Message("Exit Application.");
            Environment.Exit(0);
        }


        private void AddEvent(Runtime runtion)
        {
            this.Dispatcher.BeginInvoke((Action)delegate()
            {
                while (lst_Event.Items.Count > 100)
                {
                    lst_Event.Items.RemoveAt(0);
                }

                lst_Event.Items.Add(runtion);

                ListViewAutomationPeer lvap = new ListViewAutomationPeer(lst_Event);
                var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                ((ScrollViewer)svap.Owner).ScrollToEnd();
            });          
        }
        private void updatelayout()
        {
            left.MaxWidth = grid.ActualWidth - right.ActualWidth - work.MinWidth - 10;
            right.MaxWidth = grid.ActualWidth - left.ActualWidth - work.MinWidth - 10;

            leftbottom.MinHeight = FunctionConfigure.NavigationHeight > 0 ? 41 : 0;
            leftbottom.MaxHeight = FunctionConfigure.NavigationHeight;
            leftbottom.Height = new GridLength(FunctionConfigure.NavigationHeight);

        }
 
        private double? _leftWidth, _leftMinWidth, _rightWidth, _rightMinWidth;
        private double? _lefttopHeight, _lefttopMinHeight, _workbottomHeight, _workbottomMinHeight;
        private void displapanel(string target, bool onoff)
        {
            if(target == "left")
            {
                if (onoff)
                {
                    if (left.ActualWidth <= 0)
                    {
                        left.MinWidth = _leftMinWidth ?? 100;
                        left.Width = new GridLength(_leftWidth ?? 200);
                    }
                }
                else
                {
                    _leftMinWidth = left.MinWidth;
                    _leftWidth = left.ActualWidth ;
                    left.MinWidth = 0;
                    left.Width = new GridLength(0);
                }
            }
            else if (target == "right")
            {
                if (onoff)
                {
                    if (right.ActualWidth <= 0)
                    {
                        right.MinWidth = _rightMinWidth ?? 100;
                        right.Width = new GridLength(_rightWidth ?? 200);
                    }
                }
                else
                {
                    _rightMinWidth = right.MinWidth;
                    _rightWidth = right.ActualWidth;
                    right.MinWidth = 0;
                    right.Width = new GridLength(0);
                }
            }
            else if (target == "lefttop")
            {
                if (onoff)
                {
                    if (lefttop.ActualHeight <= 0)
                    {
                        lefttop.MinHeight = _lefttopMinHeight ?? 100;
                        lefttop.Height = new GridLength(_lefttopHeight ?? 400);                       
                    }
                }
                else
                {
                    _lefttopMinHeight= lefttop.MinHeight;
                    _lefttopHeight = lefttop.ActualHeight;
                    lefttop.MinHeight = 0;
                    lefttop.Height = new GridLength(0);
                    leftbottom.Height = new GridLength(1, GridUnitType.Star);
                }
            }
            else if(target == "workbottom")
            {
                if (onoff)
                {
                    if (workbottom.ActualHeight <= 0)
                    {
                        workbottom.MinHeight = _workbottomMinHeight ?? 100;
                        workbottom.Height = new GridLength(_workbottomHeight ?? 160);
                    }
                }
                else
                {
                    _workbottomMinHeight = workbottom.MinHeight;
                    _workbottomHeight = workbottom.ActualHeight;
                    workbottom.MinHeight = 0;
                    workbottom.Height = new GridLength(0);
                    worktop.Height = new GridLength(1, GridUnitType.Star);
                }
            }
        }

        private void chk_MaxSize_Click(object sender, RoutedEventArgs e)
        {
            if((bool)((CheckBox)sender).IsChecked)
            {
                displapanel("left", false);
                displapanel("right", false);
                displapanel("workbottom", false);
            }
            else
            {
                displapanel("left", true);
                displapanel("right", true);
                displapanel("workbottom", true);
            }           
        }

        private void chk_View_Click(object sender, RoutedEventArgs e)
        {            
            quick_list.View = (bool)((CheckBox)sender).IsChecked ? ViewType_t.IconView : ViewType_t.PanelView;
        }

        private void quick_list_OnViewChanged(object sender, RoutedEventArgs e)
        {
            chk_View.IsChecked =  ((QuickList)sender).View ==ViewType_t.IconView ?  true:false;
        }
   
        private void VMMain_OnOperated(OperatedEventArgs e)
        {
            if (e.Operate >= OperateType_t.WindowMove && e.Operate <= OperateType_t.WindowClose)
            {
                WindowOperate(e);
            }         
            else if (e.Operate == OperateType_t.OpenNewOperateWindow)
            {
                OpenNewOperateWindow(e);
                Log.Debug(String.Format("New Operate:{0}", e.parameter.ToString()));
            }
            else if(e.Operate == OperateType_t.OpenNewFastWindow)
            {
                OpenNewFastWindow(e);
                Log.Debug(String.Format("New Quick:{0}", e.parameter.ToString()));
            }
            else if (e.Operate == OperateType_t.OpenOperateWindow)
            {
                OpenOperateWindow(e);
                Log.Debug(String.Format("Open Operate:{0}", e.parameter.ToString()));
            }
            else if (e.Operate == OperateType_t.NavigationWork)
            {
                NavigationWork(e);
                Log.Debug(String.Format("Navigation:{0}", e.parameter.ToString()));
            }
            else if (e.Operate == OperateType_t.OpenNotify || e.Operate == OperateType_t.CloseNotify)
            {
                if (e.Operate == OperateType_t.OpenNotify) displapanel("right", true);               
            }
            else if (e.Operate == OperateType_t.CloseAllNotify)
            {
                displapanel("right", false);       
            }
            else if (e.Operate == OperateType_t.OpenEvent)
            {
                displapanel("workbottom", true);
            }
            else if (e.Operate == OperateType_t.CloseEvent)
            {
                displapanel("workbottom", false);
            } 
        }

        private void WindowOperate(OperatedEventArgs e)
        {
            switch (e.Operate)
            {
                case OperateType_t.WindowMove:
                    this.DragMove();
                    break;

                case OperateType_t.WindowMinimize:
                    this.WindowState = System.Windows.WindowState.Minimized;
                    break;

                case OperateType_t.WindowMaximize:
                    this.WindowState = System.Windows.WindowState.Maximized;
                    break;

                case OperateType_t.WindowRestore:
                    this.WindowState = System.Windows.WindowState.Normal;
                    break;

                case OperateType_t.WindowClose:
                    this.Close();
                    break;
            }
        }
        private List<OperationWindow> _operationWindows = new List<OperationWindow>();

        private void OpenNewOperateWindow(OperatedEventArgs e)
        {
            if (!(e.parameter is TaskType_t)) return;

            NewOperationWindow _newoperationwindow = new NewOperationWindow();
            _newoperationwindow.Type = (TaskType_t)e.parameter;
            _newoperationwindow.Owner = this;
            _newoperationwindow.DataContext = MainViewModule.QuickListViewModule;
            mask.Visibility = System.Windows.Visibility.Visible;
            _newoperationwindow.ShowDialog();
            mask.Visibility = System.Windows.Visibility.Hidden;
        }

        private void OpenNewFastWindow(OperatedEventArgs e)
        {
            if (!(e.parameter is QuickPanelType_t)) return;

            NewQuick _newquickwindow = new NewQuick();
            _newquickwindow.Type = (QuickPanelType_t)e.parameter;
            _newquickwindow.Owner = this;
            _newquickwindow.DataContext = MainViewModule.QuickListViewModule;
            mask.Visibility = System.Windows.Visibility.Visible;
            _newquickwindow.ShowDialog();
            mask.Visibility = System.Windows.Visibility.Hidden;
        }
        private void NavigationWork(OperatedEventArgs e)
        { 
            if (!(e.parameter is NavigationKey_t)) return;
            try
            {
                tab_work.SelectedIndex = (int)e.parameter;
            }
            catch(Exception ex)
            {
                Log.Warning("Navigation Target Is Invalid", ex);
            }          
        }


        private Dictionary<Point, bool> _operationwindPostion;
        private void OpenOperateWindow(OperatedEventArgs e)
        {
            if (_operationwindPostion == null)
            {
                _operationwindPostion = new Dictionary<Point, bool>();
                double fisrttop =  SystemParameters.WorkArea.Height / 2 -305 - 100;
                double fisrtleft = SystemParameters.WorkArea.Width / 2 - 460 - 300;
                    
                for(int i = 0; i < 100; i++)
                {
                    double top = (fisrttop + i * 40) % (SystemParameters.WorkArea.Height - 200 - 610) + 100;
                    double left = (fisrtleft + i * 40) % (SystemParameters.WorkArea.Height - 600 - 920) + 300;
                    Point pos = new Point(left, top);
                    if (_operationwindPostion.ContainsKey(pos)) break;
                    _operationwindPostion.Add(pos, false);
                }
            }


            if (e == null || e.parameter == null || !(e.parameter is TargetOperateArgs)) return;
            TargetOperateArgs args = (TargetOperateArgs)e.parameter;

            OperationWindow window = _operationWindows.Find(p => p.Target == args.Target as VMTarget);
            if (window == null)
            {
                window = new OperationWindow();
                window.Target = args.Target as VMTarget;
                (args.Target as VMTarget).PropertyChanged += delegate(object sender, PropertyChangedEventArgs Args)
                {
                    if (Args.PropertyName == "INotices") window.NotifyScrollRoEnd();
                };
                window.IsVisibleChanged += new DependencyPropertyChangedEventHandler(OnVisibleChanged);
                _operationWindows.Add(window);
            }

            window.Type = (TaskType_t)args.Key;

           

            if (window.Visibility == System.Windows.Visibility.Visible) window.Activate();
            else
            {
                foreach (var item in _operationwindPostion)
                {
                    if (!item.Value)
                    {
                        window.Left = item.Key.X;
                        window.Top = item.Key.Y;
                        break;
                    }
                }
                window.Show();
            }
        }

        private void OnVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            OperationWindow win = sender as OperationWindow;
            Point pos = new Point(win.Left, win.Top);
            if (_operationwindPostion.ContainsKey(pos)) _operationwindPostion[pos] = win.Visibility == System.Windows.Visibility.Visible;
        }

    }
}