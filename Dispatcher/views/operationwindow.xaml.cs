using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;

using System.Windows.Automation.Peers;
using System.Windows.Threading;

using Sigmar.Windows;
using Sigmar.Extension;
using Dispatcher.ViewsModules;
using Dispatcher.Service;
namespace Dispatcher.Views
{
    /// <summary>
    /// operationwindow.xaml 的交互逻辑
    /// </summary>
    public partial class OperationWindow : BaseWindow
    {
        public OperationWindow()
        {
            InitializeComponent();
            this.Closing += new CancelEventHandler(OnClosing);
            tab.Loaded += delegate { 
                tab.SelectedIndex = (int)Type; 
            };
        }

        public VMTarget Target
        {
            get { return (VMTarget)GetValue(TargetProperty); }
            set { SetValue(TargetProperty, value); }
        }
        public static readonly DependencyProperty TargetProperty =
            DependencyProperty.Register("Target", typeof(VMTarget), typeof(OperationWindow), new PropertyMetadata(null));

        public TaskType_t Type
        {
            get { return (TaskType_t)GetValue(TypeProperty); }
            set { SetValue(TypeProperty, value); }
        }
        public static readonly DependencyProperty TypeProperty =
            DependencyProperty.Register("Type", typeof(TaskType_t), typeof(OperationWindow), new PropertyMetadata(TaskType_t.Schedule, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
            {
                OperationWindow window = source as OperationWindow;
                if (window != null && window.tab != null)
                {
                    window.tab.SelectedIndex = (int)e.NewValue;

                    switch((int)e.NewValue)
                    {
                        case 0: window.rad_call.IsChecked = true; break;
                        case 1: window.rad_shortmessage.IsChecked = true; break;
                        case 2: window.rad_location.IsChecked = true; break;
                        case 3: window.rad_locationindoor.IsChecked = true; break;
                        case 4: window.rad_jobyickets.IsChecked = true; break;
                    }
                }
            }));


        private void window_close(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void Border_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void OnClosing(object sender, CancelEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        public void NotifyScrollRoEnd()
        {
            if(list == null)return;
            list.Dispatcher.BeginInvoke(new Action(() =>
            {
                //if (null != m_CurrentMsg)
                //{
                //    lst_History.SelectedIndex = FindItemInListView(ConvertToHistory(m_CurrentMsg));
                //    lst_History.UpdateLayout();
                //    lst_History.ScrollIntoView(lst_History.SelectedItem);
                //    m_CurrentMsg = null;
                //}
                //else
                {
                    ListViewAutomationPeer lvap = new ListViewAutomationPeer(list);
                    var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                    ((ScrollViewer)svap.Owner).ScrollToEnd();

                    list.SelectedIndex = list.Items.Count - 1;
                }
            }), DispatcherPriority.ContextIdle);
        }

        private void tabchange(object sender, RoutedEventArgs e)
        {
            tab.SelectedIndex = ((sender as RadioButton).Tag as string).ToInt();
        }
    }
}
