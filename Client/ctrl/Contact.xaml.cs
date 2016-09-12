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

namespace TrboX
{
    /// <summary>
    /// Contact.xaml 的交互逻辑
    /// </summary>
    public partial class Contact : UserControl
    {
        public Contact()
        {
            InitializeComponent();
        }

        public new string Content
        {
            get { return (string)GetValue(ContentProperty); }
            set { SetValue(ContentProperty, value); }
        }

        public new static readonly DependencyProperty ContentProperty =
            DependencyProperty.Register("Content", typeof(string), typeof(Contact));

        public Dictionary<COrganization, List<COrganization>> ContactSrc
        {
            get { return (Dictionary<COrganization, List<COrganization>>)GetValue(ContactSrcProperty); }
            set { SetValue(ContactSrcProperty, value); }
        }

        public static readonly DependencyProperty ContactSrcProperty =
            DependencyProperty.Register("ContactSrc", typeof(Dictionary<COrganization, List<COrganization>>), typeof(Contact), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {

            Dictionary<COrganization, List<COrganization>> contactlist = e.NewValue as Dictionary<COrganization, List<COrganization>>;
            ControlTemplate baseWindowTemplate = ((Contact)obj).combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", ((Contact)obj).combox);

            lst_ContactList.View = (ViewBase)lst_ContactList.FindResource("ContactListView");

            if (null != lst_ContactList)
            {
                foreach (var group in contactlist)
                {
                    lst_ContactList.Items.Add(group.Key);
                }

                foreach (var group in contactlist)
                {
                    foreach(var item in group.Value)
                        lst_ContactList.Items.Add(item);
                }
            }
        }));

        

        private void SearchAndDisp(string str)
        {
            ControlTemplate baseWindowTemplate = combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);

            if (null != lst_ContactList)
            {

                lst_ContactList.Items.Clear();
                foreach (var group in ContactSrc)
                {
                    if (null != group.Key.target.group)
                    if((group.Key.target.group.group_id.ToString().Contains(str)) ||(group.Key.target.group.name.Contains(str)))
                        lst_ContactList.Items.Add(group.Key);
                }

                foreach (var group in ContactSrc)
                {
                    foreach (var item in group.Value)
                    {
                        if(null != item.target.group)
                            if ((item.target.group.group_id.ToString().Contains(str)) || (item.target.group.name.Contains(str)))
                            {
                                lst_ContactList.Items.Add(item);
                                continue;
                            }

                        if (null != item.target.employee)
                            if (item.target.employee.name.Contains(str))
                            {
                                lst_ContactList.Items.Add(item);
                                continue;
                            }

                        if (null != item.target.vehicle)
                            if (item.target.vehicle.number.Contains(str))
                            {
                                lst_ContactList.Items.Add(item);
                                continue;
                            }

                        if (null != item.target.radio)
                            if (item.target.radio.radio_id.ToString().Contains(str))
                            {
                                lst_ContactList.Items.Add(item);
                                continue;
                            }                       
                    }                       
                }
            }
        }

        private void btn_Search_Click(object sender, RoutedEventArgs e)
        {
            ControlTemplate baseWindowTemplate = combox.Template;
            TextBox txt_SearchCon = (TextBox)baseWindowTemplate.FindName("txt_SearchCon", combox);
            if (null == txt_SearchCon) SearchAndDisp("");
            else SearchAndDisp(txt_SearchCon.Text);
        }
    }
}
