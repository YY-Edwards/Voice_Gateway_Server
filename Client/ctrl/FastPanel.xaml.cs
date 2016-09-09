using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Globalization;

namespace TrboX
{
    /// <summary>
    /// FastPanel.xaml 的交互逻辑
    /// </summary>
    public partial class FastPanel : UserControl
    {
        public FastPanel()
        {
            InitializeComponent();
            updatecyclelist(null, null);
        }
        public FastOperate PanelItem 
        {
            get { return (FastOperate)GetValue(PanelItemProperty); }
            set { SetValue(PanelItemProperty, value); }  
        }

        public static readonly DependencyProperty PanelItemProperty =
            DependencyProperty.Register("PanelItem", typeof(FastOperate), typeof(FastPanel), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e){

                if (OPType.Position == ((FastOperate)e.NewValue).m_Operate.type)
                {
                    var element = obj as FastPanel;
                    List<double> cyclelist = CPosition.UpdateCycleList(((FastOperate)e.NewValue).m_Operate.position.iscsbk, ((FastOperate)e.NewValue).m_Operate.position.isenh);
                    element.cmb_CycleLst.Items.Clear();
                    foreach (double cycle in cyclelist)
                        element.cmb_CycleLst.Items.Add(new ComboBoxItem() {Content = cycle.ToString() + "s", Tag = cycle });
                }
            }));

        public static readonly RoutedEvent ClosingRoutedEvent =
            EventManager.RegisterRoutedEvent("Closing", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void btn_Close_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClosingRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void updatecyclelist(object sender, RoutedEventArgs e)
        {
            List<double> cyclelist = CPosition.UpdateCycleList((bool)chk_CSBK.IsChecked, (bool)chk_Enh.IsChecked);
            cmb_CycleLst.Items.Clear();
            foreach (double cycle in cyclelist)
                cmb_CycleLst.Items.Add(new ComboBoxItem() { Content = cycle.ToString() + "s", Tag = cycle });
        }

        public static readonly RoutedEvent TitlePressedRoutedEvent =
           EventManager.RegisterRoutedEvent("TitlePressed", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void bdr_PanelTitle_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(TitlePressedRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public event RoutedEventHandler TitlePressed
        {
            add { AddHandler(TitlePressedRoutedEvent, value); }
            remove { RemoveHandler(TitlePressedRoutedEvent, value); }
        }

        public event RoutedEventHandler Closing
        {
            add { AddHandler(ClosingRoutedEvent, value); }
            remove { RemoveHandler(ClosingRoutedEvent, value); }
        }

        public event RoutedEventHandler OnlineCheck
        {
            add { AddHandler(OnlineCheckRoutedEvent, value); }
            remove { RemoveHandler(OnlineCheckRoutedEvent, value); }
        }

         public static readonly RoutedEvent OnlineCheckRoutedEvent =
         EventManager.RegisterRoutedEvent("OnlineCheck", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));


        private void btn_Check_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(OnlineCheckRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent MonitorRoutedEvent =
EventManager.RegisterRoutedEvent("Monitor", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Monitor_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MonitorRoutedEvent);
            RaiseEvent(newEventArgs);
        }
        public static readonly RoutedEvent DispatchRoutedEvent =
EventManager.RegisterRoutedEvent(" Dispatch", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Call_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(DispatchRoutedEvent);
            RaiseEvent(newEventArgs);
        }
        public static readonly RoutedEvent MessageRoutedEvent =
EventManager.RegisterRoutedEvent("Message", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Message_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }
        public static readonly RoutedEvent PositionRoutedEvent =
EventManager.RegisterRoutedEvent("Position", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Position_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(PositionRoutedEvent);
            RaiseEvent(newEventArgs);

        }
        public static readonly RoutedEvent JobTicketRoutedEvent =
EventManager.RegisterRoutedEvent("JobTicket", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Job_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(JobTicketRoutedEvent);
            RaiseEvent(newEventArgs);
        }



        public static readonly RoutedEvent ShutDownRoutedEvent =
EventManager.RegisterRoutedEvent("ShutDown", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Control_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ShutDownRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent StartUpRoutedEvent =
EventManager.RegisterRoutedEvent("StartUp", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        public static readonly RoutedEvent SleepRoutedEvent =
EventManager.RegisterRoutedEvent("Sleep", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        public static readonly RoutedEvent WeekRoutedEvent =
EventManager.RegisterRoutedEvent("Week", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void btn_Control_Selected(object sender, RoutedEventArgs e)
        {
            switch(btn_Control.SelectedIndex)
            {
                case 0:

            RaiseEvent(new RoutedEventArgs(StartUpRoutedEvent));
            break;
                case 1:

            RaiseEvent(new RoutedEventArgs(SleepRoutedEvent));
            break;
                case 2:

            RaiseEvent(new RoutedEventArgs(WeekRoutedEvent));
            break;
                default:
                    break;
            }
        }

        public event RoutedEventHandler Monitor
        {
            add { AddHandler(MonitorRoutedEvent, value); }
            remove { RemoveHandler(MonitorRoutedEvent, value); }
        }

        public event RoutedEventHandler Dispatch
        {
            add { AddHandler(DispatchRoutedEvent, value); }
            remove { RemoveHandler(DispatchRoutedEvent, value); }
        }

        public event RoutedEventHandler Message
        {
            add { AddHandler(MessageRoutedEvent, value); }
            remove { RemoveHandler(MessageRoutedEvent, value); }
        }

        public event RoutedEventHandler Position
        {
            add { AddHandler(PositionRoutedEvent, value); }
            remove { RemoveHandler(PositionRoutedEvent, value); }
        }

        public event RoutedEventHandler ShutDown
        {
            add { AddHandler(ShutDownRoutedEvent, value); }
            remove { RemoveHandler(ShutDownRoutedEvent, value); }
        }
        public event RoutedEventHandler StartUp
        {
            add { AddHandler(StartUpRoutedEvent, value); }
            remove { RemoveHandler(StartUpRoutedEvent, value); }
        }
        public event RoutedEventHandler Sleep
        {
            add { AddHandler(SleepRoutedEvent, value); }
            remove { RemoveHandler(SleepRoutedEvent, value); }
        }
        public event RoutedEventHandler Week
        {
            add { AddHandler(WeekRoutedEvent, value); }
            remove { RemoveHandler(WeekRoutedEvent, value); }
        }

        public event RoutedEventHandler Jobticket
        {
            add { AddHandler(JobTicketRoutedEvent, value); }
            remove { RemoveHandler(JobTicketRoutedEvent, value); }
        }



    }
}
