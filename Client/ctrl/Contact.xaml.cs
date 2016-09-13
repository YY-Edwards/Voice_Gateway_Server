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

    public class contact_str
    {
        public bool issel { set; get; }
        public COrganization contact { set; get; }
    }
    /// <summary>
    /// Contact.xaml 的交互逻辑
    /// </summary>
    /// 
    public partial class Contact : UserControl
    {
        public Contact()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                ControlTemplate baseWindowTemplate = combox.Template;
                ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);
                if (null != lst_ContactList)
                    lst_ContactList.View = (ViewBase)lst_ContactList.FindResource("ContactListView");

                ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);
                if (null != lst_CurrentContact)
                lst_CurrentContact.View = (ViewBase)lst_CurrentContact.FindResource("ContactView");
            };
        }

        public bool IsMultiple
        {
            get { return (bool)GetValue(IsMultipleProperty); }
            set { SetValue(IsMultipleProperty, value); }
        }

        public new static readonly DependencyProperty IsMultipleProperty =
            DependencyProperty.Register("IsMultiple", typeof(bool), typeof(Contact), new UIPropertyMetadata(true));

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
            if (null != lst_ContactList)
            {
               
                foreach (var group in contactlist)
                {
                    if (null != group.Key.target.group)
                    {
                        contact_str con = new contact_str()
                        {
                            issel = false,
                            contact = group.Key,
                        };
                        
                        foreach (var selected in ((Contact)obj).CurrentContact)
                        {
                            if (selected.index == group.Key.index)
                            {
                                con.issel = true;
                                break;
                            }
                        }

                        lst_ContactList.Items.Add(con);
                    }
                }

                foreach (var group in contactlist)
                {
                    foreach(var item in group.Value)
                    {
                        contact_str con = new contact_str()
                        {
                            issel = false,
                            contact = item,
                        };

                        foreach (var selected in ((Contact)obj).CurrentContact)
                        {
                            if (selected.index == item.index)
                            {
                                con.issel = true;
                                break;
                            }
                        }
                        lst_ContactList.Items.Add(con);
                    }                        
                }
            }
        }));

        public List<COrganization> CurrentContact
        {
            get { return (List<COrganization>)GetValue(CurrentContactProperty); }
            set { SetValue(CurrentContactProperty, value); }
        }

        public static readonly DependencyProperty CurrentContactProperty =
            DependencyProperty.Register("CurrentContact", typeof(List<COrganization>), typeof(Contact), new UIPropertyMetadata(new List<COrganization>(), delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e)
            {

                List<COrganization> contactlist = e.NewValue as List<COrganization>;
                ControlTemplate baseWindowTemplate = ((Contact)obj).combox.Template;
                ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", ((Contact)obj).combox);

                if (null != lst_CurrentContact)
                {
                    
                    foreach (var item in contactlist)
                    {
                        lst_CurrentContact.Items.Add(item);
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

        private void CheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (null == CurrentContact) CurrentContact = new List<COrganization>();

            ControlTemplate baseWindowTemplate = combox.Template;
            ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);

            if (true == ((CheckBox)sender).IsChecked)
            {
                CurrentContact.Add((COrganization)((contact_str)((CheckBox)sender).DataContext).contact);
                lst_CurrentContact.Items.Add((COrganization)((contact_str)((CheckBox)sender).DataContext).contact);
            }
            else
            {
                CurrentContact.Remove((COrganization)((contact_str)((CheckBox)sender).DataContext).contact);
                lst_CurrentContact.Items.Remove((COrganization)((contact_str)((CheckBox)sender).DataContext).contact);
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {                           
            ControlTemplate baseWindowTemplate = combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);
            ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);
            if ((null != lst_ContactList) && (null != lst_CurrentContact))
            {           
               int index = -1;
                foreach(contact_str item in lst_ContactList.Items)
               {
                   if(item.contact.index == ((COrganization)((Button)sender).DataContext).index)
                   {
                       index = lst_ContactList.Items.IndexOf(item);
                       break;
                   }
               }

                if(index >= 0)
                {
                    lst_ContactList.Items.RemoveAt(index);
                    lst_ContactList.Items.Insert(index, new contact_str() { issel = false, contact = (COrganization)((Button)sender).DataContext });
                }

                CurrentContact.Remove((COrganization)((Button)sender).DataContext);
                lst_CurrentContact.Items.Remove((COrganization)((Button)sender).DataContext);
            }    
        }

        private void lst_ContactList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            
            ControlTemplate baseWindowTemplate = combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);
            ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);

            if ((lst_ContactList.SelectedIndex < 0) || (lst_ContactList.SelectedIndex >= lst_ContactList.Items.Count)) return;


            CurrentContact.Clear();
            lst_CurrentContact.Items.Clear();

            CurrentContact.Add(((contact_str)lst_ContactList.SelectedItem).contact);
            lst_CurrentContact.Items.Add(((contact_str)lst_ContactList.SelectedItem).contact);
            lst_ContactList.SelectedIndex = -1;          
        }
    }
}
