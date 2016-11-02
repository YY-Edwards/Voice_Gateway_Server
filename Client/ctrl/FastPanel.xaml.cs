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
    public class OpTypeToContactType : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return true;
            if (OPType.Dispatch == ((COperate)value).Type) return false;
            else return true;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }
   
    
    /// <summary>
    /// FastPanel.xaml 的交互逻辑
    /// </summary>
    public partial class FastPanel : UserControl
    {
        Main main = null;
        CMultMember SelectContact = new CMultMember();
        public FastPanel()
        {
            InitializeComponent();
            updatecyclelist(null, null);
            this.Loaded+=delegate
            {
                main = Main.GetWindow(this) as Main;
                contact_box.ContactList = TargetMgr.TargetList;
                contact_box.CurrentContact = (null == PanelItem.Operate ) ? null:PanelItem.Operate.Target;

                CurrentContact = PanelItem.Contact;
            };

            this.Closing += delegate
            {
                //SelectContact = contact_box.CurrentContact;
            };
        }

        public FastOperate PanelItem 
        {
            get { return (FastOperate)GetValue(PanelItemProperty); }
            set { SetValue(PanelItemProperty, value); }  
        }

        public static readonly DependencyProperty PanelItemProperty =
            DependencyProperty.Register("PanelItem", typeof(FastOperate), typeof(FastPanel), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e){
                if (null == e.NewValue) return;
                if ((null != ((FastOperate)e.NewValue).Operate) && (OPType.Position == ((FastOperate)e.NewValue).Operate.Type))
                {
                    var element = obj as FastPanel;
                    List<double> cyclelist = CPosition.UpdateCycleList(((CPosition)((FastOperate)e.NewValue).Operate.Operate).IsCSBK, ((CPosition)((FastOperate)e.NewValue).Operate.Operate).IsEnh);
                    element.cmb_CycleLst.Items.Clear();
                    foreach (double cycle in cyclelist)
                        element.cmb_CycleLst.Items.Add(new ComboBoxItem() { Content = cycle.ToString() + "s", Tag = cycle });
                    element.cmb_CycleLst.SelectedIndex = 0;
                }
            }));

        public CMultMember CurrentContact
        {
            get { return (CMultMember)GetValue(CurrentContactProperty); }
            set { SetValue(CurrentContactProperty, value); }
        }

        public static readonly DependencyProperty CurrentContactProperty =
    DependencyProperty.Register("CurrentContact", typeof(CMultMember), typeof(FastPanel), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {

        }));

    //    public static readonly DependencyProperty ShortMessageProperty =
    //DependencyProperty.Register("PanelItem", typeof(string), typeof(FastPanel), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e)
    //{
        //if ((null != ((FastOperate)e.NewValue).Operate) && (OPType.Position == ((FastOperate)e.NewValue).Operate.Type))
        //{
        //    var element = obj as FastPanel;
        //    List<double> cyclelist = CPosition.UpdateCycleList(((CPosition)((FastOperate)e.NewValue).Operate.Operate).IsCSBK, ((CPosition)((FastOperate)e.NewValue).Operate.Operate).IsEnh);
        //    element.cmb_CycleLst.Items.Clear();
        //    foreach (double cycle in cyclelist)
        //        element.cmb_CycleLst.Items.Add(new ComboBoxItem() { Content = cycle.ToString() + "s", Tag = cycle });
        //}
    //}));


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
            cmb_CycleLst.SelectedIndex = 0;

            if (false == chk_CSBK.IsChecked) chk_Enh.IsChecked = false;
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
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Control, CurrentContact, new CControl() { Type = ControlType.Check });
            RoutedEventArgs newEventArgs = new RoutedEventArgs(OnlineCheckRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent MonitorRoutedEvent =
EventManager.RegisterRoutedEvent("Monitor", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Monitor_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Control, CurrentContact, new CControl() { Type = ControlType.Monitor });
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MonitorRoutedEvent);
            RaiseEvent(newEventArgs);
        }
        public static readonly RoutedEvent DispatchRoutedEvent =
EventManager.RegisterRoutedEvent(" Dispatch", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        public static readonly RoutedEvent MessageRoutedEvent =
EventManager.RegisterRoutedEvent("Message", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_Message_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.ShortMessage, CurrentContact, new CShortMessage() { Message = txt_ShortMessage.Text});

            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent ShutDownRoutedEvent =
EventManager.RegisterRoutedEvent("ShutDown", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_ShutDown_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Control, CurrentContact, new CControl() { Type = ControlType.ShutDown });
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ShutDownRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent StartUpRoutedEvent =
EventManager.RegisterRoutedEvent("StartUp", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void btn_StartUp_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Control, CurrentContact, new CControl() { Type = ControlType.StartUp });
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ShutDownRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public static readonly RoutedEvent MoreRoutedEvent =
EventManager.RegisterRoutedEvent("More", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));
        private void btn_More_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MoreRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        public event RoutedEventHandler Monitor
        {
            add { AddHandler(MonitorRoutedEvent, value); }
            remove { RemoveHandler(MonitorRoutedEvent, value); }
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
        public event RoutedEventHandler More
        {
            add { AddHandler(MoreRoutedEvent, value); }
            remove { RemoveHandler(MoreRoutedEvent, value); }
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

        private void btn_Op_PTT_Checked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Dispatch, contact_box.CurrentContact, new CDispatch() { Exec = ExecType.Start});

            RoutedEventArgs newEventArgs = new RoutedEventArgs(DispatchRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void btn_Op_PTT_Unchecked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Dispatch, contact_box.CurrentContact, new CDispatch() { Exec = ExecType.Stop });

            RoutedEventArgs newEventArgs = new RoutedEventArgs(DispatchRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void chk_Contact_PTT_Checked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Dispatch, CurrentContact, new CDispatch() { Exec = ExecType.Start });

            RoutedEventArgs newEventArgs = new RoutedEventArgs(DispatchRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void chk_Contact_PTT_Unchecked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Contact == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Dispatch, CurrentContact, new CDispatch() { Exec = ExecType.Stop });

            RoutedEventArgs newEventArgs = new RoutedEventArgs(DispatchRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void btn_Op_SendMsg_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.ShortMessage, contact_box.CurrentContact, new CShortMessage() { Message = txt_ShortMessage.Text });

            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void btn_Op_Query_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
            {
                PanelItem.Operate = new COperate(OPType.ShortMessage,(bool)chk_All.IsChecked ? new CMultMember() { Type = SelectionType.All} :contact_box.CurrentContact , new CPosition() {
                Type = ExecType.Start,
                IsCycle = (bool)chk_Cycle.IsChecked,                   
                IsCSBK = (bool)chk_CSBK.IsChecked,
                IsEnh = (bool)chk_Enh.IsChecked,
                Cycle = (double)((ComboBoxItem)cmb_CycleLst.SelectedItem).Tag});               
            }
               
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void chk_Op_StartCycle_Checked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
            {
                PanelItem.Operate = new COperate(OPType.ShortMessage, (bool)chk_All.IsChecked ? new CMultMember() { Type = SelectionType.All } : contact_box.CurrentContact, new CPosition()
                {
                    Type = ExecType.Start,
                    IsCycle = (bool)chk_Cycle.IsChecked,
                    IsCSBK = (bool)chk_CSBK.IsChecked,
                    IsEnh = (bool)chk_Enh.IsChecked,
                    Cycle = (double)((ComboBoxItem)cmb_CycleLst.SelectedItem).Tag
                });
            }

            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void chk_Op_StartCycle_Unchecked(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
            {
                PanelItem.Operate = new COperate(OPType.ShortMessage, (bool)chk_All.IsChecked ? new CMultMember() { Type = SelectionType.All } : contact_box.CurrentContact, new CPosition()
                {
                    Type = ExecType.Stop,
                    IsCycle = (bool)chk_Cycle.IsChecked,
                    IsCSBK = (bool)chk_CSBK.IsChecked,
                    IsEnh = (bool)chk_Enh.IsChecked,
                    Cycle = (double)((ComboBoxItem)cmb_CycleLst.SelectedItem).Tag
                });
            }
            RoutedEventArgs newEventArgs = new RoutedEventArgs(MessageRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void btn_Op_Control_Click(object sender, RoutedEventArgs e)
        {
            if (FastType.FastType_Operate == PanelItem.Type)
                PanelItem.Operate = new COperate(OPType.Control, CurrentContact, new CControl() { Type = (ControlType)cmb_Op_Control.SelectedIndex });
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ShutDownRoutedEvent);
            RaiseEvent(newEventArgs);
        }

    }
}
