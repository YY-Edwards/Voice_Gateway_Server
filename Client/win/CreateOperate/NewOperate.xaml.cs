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

namespace TrboX
{
    /// <summary>
    /// NewOperate.xaml 的交互逻辑
    /// </summary>
    public partial class NewOperate : MyWindow
    {
        Main m_Main;
        public NewOperate()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                m_Main = this.Owner as Main;
                contact_OpTarget.ContactList = TargetMgr.TargetList;

            };
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
                cmb_CycleLst.Items.Add(new ComboBoxItem() { Content = cycle.ToString() + "s",
                            Tag = cycle,
                            Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                            Foreground =new SolidColorBrush(Color.FromArgb(255, 210 ,223, 245)),
                            FontSize = 13,
                            Height = 32}
                            );
            cmb_CycleLst.SelectedIndex = 0;

            if (false == chk_CSBK.IsChecked) chk_Enh.IsChecked = false;
        }

        private void tab_NewType_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {                    
            if (null != contact_OpTarget)

                if (tab_NewType.SelectedIndex == 0)
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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void btn_SysClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void btn_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
    }
}
