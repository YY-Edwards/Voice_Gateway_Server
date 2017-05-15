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
using System.Windows.Shapes;
using System.Globalization;

namespace TrboX
{
    public class OpTypeSelectToContactType : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if((null != value) && ((int)value == 0) ) return false;

            return true;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
    /// <summary>
    /// newFast.xaml 的交互逻辑
    /// </summary>
    public partial class newFast : MyWindow
    {
        Main m_Main;
        public FastType Type;
        public newFast()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                m_Main = this.Owner as Main;
                contact_OpTarget.ContactList = TargetMgr.TargetList;

                 updatecyclelist(null, new RoutedEventArgs());
                 FillContact();
                 FillFastOp();
                 SelectionType();

                if(m_Main.StatusBar.Get().type == RunMode.Repeater)
                {
                    cmbit_Control.Visibility = Visibility.Collapsed;
                }
                else
                {
                    cmbit_Control.Visibility = Visibility.Visible;
                }

            };

            this.Activated += delegate {
                //m_Main = this.Owner as Main;              
            };    
        }

        private void SelectionType()
        {
            switch (Type)
            { 
                case FastType.FastType_Contact:
                    rad_CreateContact.IsChecked = true;
                    break;
                case FastType.FastType_Operate:
                    rad_CreateFastOperate.IsChecked = true;
                    break;
                default: break;
            }
        }

        private void FillContact()
        {
            //lst_ContactList.View = (ViewBase)lst_ContactList.FindResource("ContactListView");

            CTargetRes TargetRes = TargetMgr.TargetList;

            List<long> existraido = new List<long>();

            lst_ContactList.Items.Clear();

            if (null != TargetRes.Group)
            foreach (var group in TargetRes.Group)
            {
                //debar undefine group
                if ((null == group.Value.Group) || (-1 == group.Value.Group.GroupID) || (-1 == group.Value.Group.ID)) continue;
                lst_ContactList.Items.Add(new ListViewItem() { Content = group.Value });
            }

            if (null != TargetRes.Staff)
                foreach (var staff in TargetRes.Staff)
            {
                lst_ContactList.Items.Add(new ListViewItem() { Content = staff.Value });
                if (null != staff.Value.Radio) existraido.Add(staff.Value.Radio.RadioID);
            }

            if (null != TargetRes.Radio)
                foreach (var radio in TargetRes.Radio)
            {
                if ((null != radio.Value.Radio) && (existraido.Contains(radio.Value.Radio.RadioID))) continue;
                lst_ContactList.Items.Add(new ListViewItem() { Content = radio.Value });
            }
        }

        private void FillFastOp()
        {
            //lst_SelectContact.View = (ViewBase)lst_ContactList.FindResource("SelectContactView");
            //lst_SelectOperate.View = (ViewBase)lst_ContactList.FindResource("SelectOperateView");           
            
            List<FastOperate> FastOperateList = m_Main.WorkArea.FastPanel.Get();
            lst_SelectContact.Items.Clear();
            lst_SelectOperate.Items.Clear();
            foreach(var item in FastOperateList)
            {
                if (FastType.FastType_Contact == item.Type)
                {
                    lst_SelectContact.Items.Add(new ListViewItem() { Content = item });
                }
                else if (FastType.FastType_Operate == item.Type)
                {
                    lst_SelectOperate.Items.Add(new ListViewItem() { Content = item });
                }

            }
        }

        public override void OnMyWindow_Btn_Close_Click()
        {
            this.Close();
        }

        private void updatecyclelist(object sender, RoutedEventArgs e)
        {
            List<double> cyclelist = CPosition.UpdateCycleList((bool)chk_CSBK.IsChecked, (bool)chk_Enh.IsChecked);
            cmb_CycleLst.Items.Clear();
            foreach (double cycle in cyclelist)
                cmb_CycleLst.Items.Add(new ComboBoxItem()
                {
                    Content = cycle.ToString() + "s",
                    Tag = cycle,
                    Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                    Foreground = new SolidColorBrush(Color.FromArgb(255, 210, 223, 245)),
                    FontSize = 13,
                    Height = 32,
                });
            cmb_CycleLst.SelectedIndex = 0;

            if (false == chk_CSBK.IsChecked) chk_Enh.IsChecked = false;
        }

        List<FastOperate> addlist = new List<FastOperate>();
        List<FastOperate> dellist = new List<FastOperate>();

        private void AddFastOp(FastOperate operate)
        {
            if (null == operate) return;
            
            foreach(var item in dellist)
           {
               if (operate.IsEqual(item))
               {
                   dellist.Remove(item);
                   return;
               }
           }

           addlist.Add(operate);
        }

        private void DelFastOp(FastOperate operate)
        {
            if (null == operate) return;

            foreach (var item in addlist)
            {
                if (operate.IsEqual(item))
                {
                    addlist.Remove(item);
                    return;
                }
            }

            dellist.Add(operate);
        }

        private void btn_AddContact_Click(object sender, RoutedEventArgs e)
        {
            if (null == lst_ContactList.SelectedItem) return;
            CMultMember selItem = ((CMember)((ListViewItem)lst_ContactList.SelectedItem).Content).SingleToMult();

            foreach (var item in lst_SelectContact.Items)
            {
                FastOperate op = (FastOperate)((ListViewItem)item).Content;

                if ((null != op) && (selItem.IsEqual(op.Contact)))
                    return;
            }

            FastOperate operate = new FastOperate()
            {
                Type = FastType.FastType_Contact,
                Contact = selItem,
                Operate = null
            };

            lst_SelectContact.Items.Add(new ListViewItem() { Content = operate });
            AddFastOp(operate);
        }

        private void btn_RemoveContact_Click(object sender, RoutedEventArgs e)
        {
           if (null == lst_SelectContact.SelectedItem) return;


           DelFastOp((FastOperate)((ListViewItem)lst_SelectContact.SelectedItem).Content);
           lst_SelectContact.Items.Remove(lst_SelectContact.SelectedItem);

        }

        private void btn_AddOperate_Click(object sender, RoutedEventArgs e)
        {
            COperate operate = null;

            ControlType[] control = new ControlType[6]{ControlType.Check, ControlType.Monitor,ControlType.ShutDown, ControlType.StartUp, ControlType.Sleep, ControlType.Week};

            switch (cmb_OpType.SelectedIndex)
            {
                case 0://dispatch
                    operate = new COperate() { Type = OPType.Dispatch, Target = contact_OpTarget.CurrentContact.Clone(), Operate = new CDispatch() { } };                   
                    break;
                case 1:
                    operate = new COperate() { Type = OPType.ShortMessage, Target = contact_OpTarget.CurrentContact.Clone(), Operate = new CShortMessage() { Message = txt_Message.Text} }; 
                    break;
                case 2:
                    operate = new COperate() { Type = OPType.Position, Target = contact_OpTarget.CurrentContact.Clone(), Operate = new CPosition() { IsCSBK = (bool)chk_CSBK.IsChecked, IsEnh = (bool)chk_Enh.IsChecked, IsCycle = (bool)chk_Cycle.IsChecked, Cycle = (double)((ComboBoxItem)cmb_CycleLst.SelectedItem).Tag } }; 
                    break;
                case 3:
                    operate = new COperate() { Type = OPType.Control, Target = contact_OpTarget.CurrentContact.Clone(), Operate = new CControl() { 
                        Type = ((cmb_ControlList.SelectedIndex < 0) || (cmb_ControlList.SelectedIndex >=6)) ? ControlType.Check : control[cmb_ControlList.SelectedIndex]
                    }}; 
                    break;
                case 4:
                    operate = new COperate() { Type = OPType.JobTicker, Target = contact_OpTarget.CurrentContact.Clone(), Operate = new CJobTicket() { } }; 
                    break;
                default:
                    return;
            }

            if (null == operate) return;

            FastOperate fast = new FastOperate() { Type = FastType.FastType_Operate , Contact = null, Operate = operate};

            foreach (var item in lst_SelectOperate.Items)
            {
                FastOperate op = (FastOperate)((ListViewItem)item).Content;
                if ((null != op) && (fast.IsEqual(op))) return;
            }

            lst_SelectOperate.Items.Add(new ListViewItem() { Content = fast });
            AddFastOp(fast);
        }

        private void btn_RemoveOperate_Click(object sender, RoutedEventArgs e)
        {
            if (null == lst_SelectOperate.SelectedItem) return;


            DelFastOp((FastOperate)((ListViewItem)lst_SelectOperate.SelectedItem).Content);
            lst_SelectOperate.Items.Remove(lst_SelectOperate.SelectedItem);
        }

        private void btn_Canel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void btn_Enter_Click(object sender, RoutedEventArgs e)
        {
            foreach (var item in addlist)
                m_Main.WorkArea.FastPanel.Add(item);

            foreach (var item in dellist)
                m_Main.WorkArea.FastPanel.Remove(item);

            this.Close();
        }

        private void cmb_OpType_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (null != contact_OpTarget)

                if (cmb_OpType.SelectedIndex == 0)
                {
                    if ((null != contact_OpTarget.CurrentContact)
                    && (null != contact_OpTarget.CurrentContact.Target)
                    && (contact_OpTarget.CurrentContact.Target.Count >= 1))
                    {
                        contact_OpTarget.UpdateCurrentContact(contact_OpTarget.CurrentContact.Target[0].SingleToMult());
                    }
                }
                else
                {
                    contact_OpTarget.UpdateCurrentContact(contact_OpTarget.CurrentContact);
                }
        }

        private void rad_CreateFastOperate_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void btn_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void btn_SysClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
