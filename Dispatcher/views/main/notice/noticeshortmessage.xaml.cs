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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Automation.Peers;
using System.Windows.Threading;
using System.ComponentModel;

using Sigmar.Controls;
using Dispatcher.ViewsModules;

namespace Dispatcher.Views
{
    /// <summary>
    /// notieca_arm.xaml 的交互逻辑
    /// </summary>
    public partial class NoticeShortMessage : UserControl
    {
        public NoticeShortMessage()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                (this.DataContext as VMNotify).PropertyChanged += delegate(object sender, PropertyChangedEventArgs Args)
                {
                    if (Args.PropertyName == "Alarm") NotifyScrollRoEnd();
                };
            };
        }

        public event RoutedEventHandler Close
        {
            add { AddHandler(CloseRoutedEvent, value); }
            remove { RemoveHandler(CloseRoutedEvent, value); }
        }

        public static readonly RoutedEvent CloseRoutedEvent =
           EventManager.RegisterRoutedEvent("Close", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(NoticeShortMessage));

        private void closenotify(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs args = new RoutedEventArgs(CloseRoutedEvent);
            RaiseEvent(args);
        }

        public void NotifyScrollRoEnd()
        {
            if (list == null) return;
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

                    //list.SelectedIndex = list.Items.Count - 1;
                }
            }), DispatcherPriority.ContextIdle);
        }
    }
}
