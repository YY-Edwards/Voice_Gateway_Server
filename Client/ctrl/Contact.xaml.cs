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
    //public class ContactListVOffset : IValueConverter
    //{
    //    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        if ((null == value) || (0.000001 > (double)value)) return 0;
    //        return ((double)value - 0.000001) / (double)value - 3;

    //        //return 0;
    //    }

    //    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        return value;
    //    }
    //}


    public class contact_str
    {
        public bool IsSel { set; get; }
        public CMember Contact { set; get; }
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
                OnContactListChange("", this);
                OnCurrentContactChange("", this);
            };
        }

        public bool IsMultiple
        {
            get { return (bool)GetValue(IsMultipleProperty); }
            set { SetValue(IsMultipleProperty, value); }
        }

        public static readonly DependencyProperty IsMultipleProperty =
            DependencyProperty.Register("IsMultiple", typeof(bool), typeof(Contact), new UIPropertyMetadata(true));

        public CTargetRes ContactList
        {
            get { return (CTargetRes)GetValue(ContactListProperty); }
            set { SetValue(ContactListProperty, value); }
        }

        public static readonly DependencyProperty ContactListProperty =
            DependencyProperty.Register("ContactList", typeof(CTargetRes), typeof(Contact), new UIPropertyMetadata(new CTargetRes()));

        public static void OnContactListChange(string condition, DependencyObject obj)
        {
            CTargetRes contactlist = ((Contact)obj).ContactList;
            ControlTemplate baseWindowTemplate = ((Contact)obj).combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", ((Contact)obj).combox);
            List<contact_str> conlist = MatchCondition(condition, contactlist, ((Contact)obj).CurrentContact);
            if (null != lst_ContactList)
            {
                lst_ContactList.Items.Clear();
                foreach (contact_str con in conlist)
                    lst_ContactList.Items.Add(new ListViewItem() { Content = con });
            }
        }

        public CMultMember CurrentContact
        {
            get {
                return (CMultMember)GetValue(CurrentContactProperty);
            }
            set { SetValue(CurrentContactProperty, value); }
        }

        public static readonly DependencyProperty CurrentContactProperty =
            DependencyProperty.Register("CurrentContact", typeof(CMultMember), typeof(Contact),
            new PropertyMetadata(new CMultMember()));

        public void UpdateCurrentContact(CMultMember target)
        {
            CurrentContact = (null == target)? null :target.Clone();
            OnCurrentContactChange(m_Condition, this);
            OnContactListChange(m_Condition, this);
        }

        public static void OnCurrentContactChange(string condition, DependencyObject obj)
        {
            ControlTemplate baseWindowTemplate = ((Contact)obj).combox.Template;
            ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", ((Contact)obj).combox);
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", ((Contact)obj).combox);
            CMultMember member = ((Contact)obj).CurrentContact;

            if (null != lst_CurrentContact)
            {
                lst_CurrentContact.Items.Clear();

                if (null == member) return;
                if (null == member.Target) return;

                foreach (var item in member.Target)
                    if (lst_CurrentContact.Items.Count < 2) lst_CurrentContact.Items.Add(new ListViewItem() { Content = item });
                    else if (lst_CurrentContact.Items.Count == 2)
                    {
                        lst_CurrentContact.Items.Add(new ListViewItem() { Content = "..." });
                        break;
                    }
            }
        }



        private static List<contact_str> ConvertToList(CTargetRes targetlist, CMultMember selected)
        {
            List<long> existraido = new List<long>();
            List<contact_str> res = new List<contact_str>();

            if (null != targetlist.Group)
            foreach (var group in targetlist.Group)
            {
                //debar undefine group
                if ((null == group.Value.Group) || (-1 == group.Value.Group.GroupID) || (-1 == group.Value.Group.ID)) continue;

                contact_str con = new contact_str(){IsSel = false,Contact = group.Value,};

                //keep selection status
                if ((null != selected) && (null != selected.Target))              
                    foreach (var item in selected.Target)
                        if (item.IsEqual(con.Contact))
                        {
                            con.IsSel = true;
                            break;
                        }
                 
                res.Add(con);
            }

            if (null != targetlist.Staff)
                foreach (var staff in targetlist.Staff)
            {
                contact_str con = new contact_str() { IsSel = false, Contact = staff.Value, };
                if ((null != selected) && (null != selected.Target))              
                        foreach (var item in selected.Target)
                            if (item.IsEqual(con.Contact))
                            {
                                con.IsSel = true;
                                break;
                            }
                 res.Add(con);

                 if (null != staff.Value.Radio) existraido.Add(staff.Value.Radio.RadioID);
                    
            }

            if (null != targetlist.Radio)
            foreach (var radio in targetlist.Radio)
            {
                if ((null != radio.Value.Radio) && (existraido.Contains(radio.Value.Radio.RadioID))) continue;
                    
                contact_str con = new contact_str() { IsSel = false, Contact = radio.Value, };
                if ((null != selected) && (null != selected.Target))
                
                    foreach (var item in selected.Target)
                        if (item.IsEqual(con.Contact))
                        {
                            con.IsSel = true;
                            break;
                        }
                res.Add(con);
            }

            return res;
        }

        private static List<contact_str> MatchCondition(string condition, CTargetRes targetlist, CMultMember selected)
        {
            List<contact_str> conlist = ConvertToList(targetlist, selected);

            if ("" == condition) return conlist;
            List<contact_str> willdel = new List<contact_str>();
            
            foreach(contact_str con in conlist)
            {
                if (((null != con.Contact.Group) && (con.Contact.Group.GroupID.ToString().ToLower().Contains(condition.ToLower()) || con.Contact.Group.Name.ToLower().Contains(condition.ToLower())))
                  || ((null != con.Contact.Staff) && con.Contact.Staff.Name.ToLower().Contains(condition.ToLower()))
                  || ((null != con.Contact.Radio) && con.Contact.Radio.RadioID.ToString().ToLower().Contains(condition.ToLower())))
                {
                    //match condition
                }
                else
                {
                    //remove
                    willdel.Add(con);
                }
            }

            //remove
            foreach(contact_str con in willdel)
                conlist.Remove(con);

            return conlist;
        }

        private string m_Condition = "";
        private void btn_Search_Click(object sender, RoutedEventArgs e)
        {
           
            ControlTemplate baseWindowTemplate = combox.Template;
            TextBox txt_SearchCon = (TextBox)baseWindowTemplate.FindName("txt_SearchCon", combox);
             m_Condition = (null == txt_SearchCon) ? "" : txt_SearchCon.Text;

             OnContactListChange(m_Condition, this);
        }
        private void  CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            if (null == CurrentContact) CurrentContact = new CMultMember();
            CurrentContact.Type = SelectionType.Multiple;
            if (null == CurrentContact.Target) CurrentContact.Target = new List<CMember>();
            CurrentContact.Target.Add(((contact_str)((CheckBox)sender).DataContext).Contact);

            //ControlTemplate baseWindowTemplate = combox.Template;
            //ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);
            //if (null != lst_CurrentContact)

            //if (lst_CurrentContact.Items.Count < 2) lst_CurrentContact.Items.Add(new ListViewItem() { Content = ((contact_str)((CheckBox)sender).DataContext).Contact });
            //else if (lst_CurrentContact.Items.Count == 2)
            //{
            //    lst_CurrentContact.Items.Add(new ListViewItem() { Content = "..." });
            //}

            OnCurrentContactChange(m_Condition, this);


            //OnCurrentContactChange(m_Condition, this);
        }


        private void CheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            if (null == CurrentContact) return;
            if (null == CurrentContact.Target) return;


            foreach (CMember mem in CurrentContact.Target)
            {
                if (mem.IsEqual(((contact_str)((CheckBox)sender).DataContext).Contact))
                {
                    CurrentContact.Target.Remove(mem);
                    //OnCurrentContactChange(m_Condition, this);
                    break;
                }
            }

            OnCurrentContactChange(m_Condition, this);

            //ControlTemplate baseWindowTemplate = combox.Template;
            //ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);
            ////ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);

            //if (null != lst_CurrentContact)
            //    foreach (var item in lst_CurrentContact.Items)
            //    {
            //        if ((((ListViewItem)item).Content is string) && ((string)((ListViewItem)item).Content == "...")) continue;
            //        if (((CMember)((ListViewItem)item).Content).IsEqual(((contact_str)((CheckBox)sender).DataContext).Contact))
            //        {
            //            lst_CurrentContact.Items.RemoveAt(lst_CurrentContact.Items.Count - 1);
            //            lst_CurrentContact.Items.Remove(item);
            //            break;
            //        }
            //    }


            //if (null != lst_ContactList)
            //    foreach (var item in lst_ContactList.Items)
            //    {
            //        if (((contact_str)((ListViewItem)item).Content).Contact.IsEqual((CMember)((Button)sender).DataContext))
            //        {
            //            DataTemplate baseLstItemTemplate = ((MsgView)lst_ContactList.View).ItemTemplate;
            //            ContentPresenter myContentPresenter = FindVisualChild<ContentPresenter>(item as ListViewItem);
            //            CheckBox chk_CheckBox = (CheckBox)baseLstItemTemplate.FindName("CheckBox", myContentPresenter);

            //            chk_CheckBox.IsChecked = false;
            //            break;
            //        }
            //    }
        } 

        //private void Button_Click(object sender, RoutedEventArgs e)
        //{
        //    foreach (CMember mem in CurrentContact.Target)
        //    {
        //        if (mem.IsEqual((CMember)((Button)sender).DataContext))
        //        {
        //            CurrentContact.Target.Remove(mem);
        //            break;
        //        }
        //    }

        //    ControlTemplate baseWindowTemplate = combox.Template;
        //    ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);
        //    ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);

        //    if (null != lst_CurrentContact)
        //    foreach (var item in lst_CurrentContact.Items)
        //    {
        //        if (((CMember)((ListViewItem)item).Content).IsEqual((CMember)((Button)sender).DataContext))
        //        {
        //            lst_CurrentContact.Items.Remove(item);
        //            break;
        //        }
        //    }

        //    if (null != lst_ContactList)
        //    foreach (var item in lst_ContactList.Items)
        //    {
        //        if (((contact_str)((ListViewItem)item).Content).Contact.IsEqual((CMember)((Button)sender).DataContext))
        //        {
        //            DataTemplate baseLstItemTemplate = ((MsgView)lst_ContactList.View).ItemTemplate;
        //            ContentPresenter myContentPresenter = FindVisualChild<ContentPresenter>(item as ListViewItem);
        //            if (null == myContentPresenter)
        //            {
        //                OnContactListChange("", this);
        //                break;
        //            } 
        //            CheckBox chk_CheckBox = (CheckBox)baseLstItemTemplate.FindName("CheckBox", myContentPresenter);
        //            chk_CheckBox.IsChecked = false;
        //            break;
        //        }
        //    }
        //}

        private childItem FindVisualChild<childItem>(DependencyObject obj)
         where childItem : DependencyObject
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(obj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(obj, i);
                if (child != null && child is childItem)
                    return (childItem)child;
                else
                {
                    childItem childOfChild = FindVisualChild<childItem>(child);
                    if (childOfChild != null)
                        return childOfChild;
                }
            }
            return null;
        }

        private void lst_ContactList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }

        private void lst_ContactList_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            ControlTemplate baseWindowTemplate = combox.Template;
            ListView lst_ContactList = (ListView)baseWindowTemplate.FindName("lst_ContactList", combox);
            ListView lst_CurrentContact = (ListView)baseWindowTemplate.FindName("lst_CurrentContact", combox);


            if (null == lst_ContactList) return;

            if ((lst_ContactList.SelectedIndex < 0) || (lst_ContactList.SelectedIndex >= lst_ContactList.Items.Count)) return;



            if (IsMultiple)
            {
                DataTemplate baseLstItemTemplate = ((MsgView)lst_ContactList.View).ItemTemplate;
                ContentPresenter myContentPresenter = FindVisualChild<ContentPresenter>(lst_ContactList.SelectedItem as ListViewItem);
                CheckBox chk_CheckBox = (CheckBox)baseLstItemTemplate.FindName("CheckBox", myContentPresenter);

                chk_CheckBox.IsChecked = !chk_CheckBox.IsChecked;
            }
            else
            {
                if (null == CurrentContact) CurrentContact = new CMultMember();
                CurrentContact.Type = SelectionType.Single;
                if (null == CurrentContact.Target) CurrentContact.Target = new List<CMember>();

                CurrentContact.Target.Clear();
                //lst_CurrentContact.Items.Clear();

                CurrentContact.Target.Add(((contact_str)((ListViewItem)lst_ContactList.SelectedItem).Content).Contact);
                //lst_CurrentContact.Items.Add(new ListViewItem() { Content = ((contact_str)((ListViewItem)lst_ContactList.SelectedItem).Content).Contact });

                combox.IsDropDownOpen = false;
            }
            lst_ContactList.SelectedIndex = -1;
            OnCurrentContactChange(m_Condition, this);
        }
    }
}
