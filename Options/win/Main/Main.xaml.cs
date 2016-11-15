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
using System.Windows.Resources;

using Newtonsoft.Json;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

using System.Xml.Linq;


using System.Windows.Threading;

using System.Diagnostics;

using System.Threading;

namespace TrboX
{
    /// <summary>
    /// Login.xaml 的交互逻辑
    /// </summary>
    public partial class Main : MyWindow
    {
        public SettingComponents SettingComponents;
        public ResourceComponents ResourceComponents;

        public Main()
        {
            InitializeComponent();

            this.Loaded += delegate
            {
                DataBase.open("SetttingLog.lg");
                DataBase.InsertLog("---Records Start----------------------------------------------------------");


                TServer.InitializeTServer();
                LogServer.InitializeTServer();
                SettingComponents = new SettingComponents(this);
                ResourceComponents = new ResourceComponents(this);

                SettingComponents.Set(SettingMgr.Get());
               
            };

            lst_User.Loaded += delegate
            { 
                if(lst_User.Items.Count > 0) return;
                List<User> users = UserMgr.List();
                if (users != null)
                {
                    lst_User.Items.Clear();
                    cmb_AuthorityDest.Items.Clear();
                    foreach (User user in users)
                    {
                        cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.username + (user.type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user });
                        lst_User.Items.Add(user);
                    }
                    lst_User.SelectedIndex = 0;
                    cmb_AuthorityDest.SelectedIndex = 0;
                }
            };

            lst_Group.Loaded += delegate
            {
                if (lst_Group.Items.Count > 0) return;
                List<Department> depts = DepartmentMgr.List();
                if(depts!=null)
                {
                    lst_Group.Items.Clear();
                    foreach (Department dept in depts) lst_Group.Items.Add(dept);
                    lst_Group.SelectedIndex = 0;
                }

            };

            list_AllFunc.Loaded += delegate
            {
                list_AllFunc.ItemsSource = User.AddAuth;

            };
        }

       
        private void MyWindow_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void tree_OptionsGroup_Selecteded(object sender, RoutedEventArgs e)
        {
            try
            {
                int page = int.Parse((string)((TreeViewItem)tree_Options.SelectedItem).Tag);
                tab_Options.SelectedIndex = page;
            }
            catch { }
        }

        private void tree_OptionsItem_Selected(object sender, RoutedEventArgs e)
        {
            
        }

        private double PosInScrView(ScrollViewer scr, FrameworkElement element)
        {
            GeneralTransform transform = element.TransformToVisual(bdr_Options);
            double pos = transform.Transform(new Point(element.Margin.Left, element.Margin.Top)).Y;
            return scr.ContentVerticalOffset + pos - 22.5; 
        }

        private void tree_OptionsItem_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {          
            TreeViewItem item = sender as TreeViewItem;
            if (null == item) return;
            TreeViewItem group = item.Parent as TreeViewItem;
            if (null == group) return;

            try
            {
                int page = int.Parse((string)(group.Tag));
                tab_Options.SelectedIndex = page;
            }
            catch
            {
                return;
            }

            tab_Options.UpdateLayout();

            switch ((string)group.Header)
            {
                case "基本设置":
                    switch ((string)item.Header)
                    {
                        case "网络":
                            scrview_Base.ScrollToVerticalOffset(PosInScrView(scrview_Base, dck_BaseNetwork as FrameworkElement));
                            break;
                        case "日志存储":
                            scrview_Base.ScrollToVerticalOffset(PosInScrView(scrview_Base, dck_BaseLog as FrameworkElement));
                            break;
                    }
                    break;
                case "Radio配置":
                    switch ((string)item.Header)
                    {
                        case "常规":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioGeneric as FrameworkElement));
                            break;
                        case "网络":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioNetwork as FrameworkElement));
                            break;
                        case "功能":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioFunc as FrameworkElement));
                            break;
                    }
                    break;
                case "WireLan":
                    switch ((string)item.Header)
                    {
                        case "常规":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanGeneric as FrameworkElement));
                            break;
                        case "网络":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanNetwork as FrameworkElement));
                            break;
                        case "Dongle":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanDongle as FrameworkElement));
                            break;
                        case "功能":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanFunc as FrameworkElement));
                            break;
                    }
                    break;
               case "资源管理":
                    switch ((string)item.Header)
                    {
                        case "供应商":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerCompany as FrameworkElement));
                            break;
                        case "部门":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerPartment as FrameworkElement));
                            break;
                        case "人员":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerEmployee as FrameworkElement));
                            break;
                        case "车辆":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerVehicle as FrameworkElement));
                            break;
                        case "组":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerGroup as FrameworkElement));
                            break;
                        case "终端":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerRadio as FrameworkElement));
                            break;
                        case "绑定/分配":
                            scrview_ResManager.ScrollToVerticalOffset(PosInScrView(scrview_ResManager, dck_ResManagerRelationship as FrameworkElement));
                            break;
                    }
                    break;
               case "账号及权限":
                    switch ((string)item.Header)
                    {
                        case "账户":
                            scrview_UserRoot.ScrollToVerticalOffset(PosInScrView(scrview_UserRoot, dck_User as FrameworkElement));
                            break;
                        case "功能分配":
                            scrview_UserRoot.ScrollToVerticalOffset(PosInScrView(scrview_UserRoot, dck_Root as FrameworkElement));
                            break;
                    }
                    break;
                default: break;

                    
            }

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
        }

        private void btn_Apply_Click(object sender, RoutedEventArgs e)
        {
           SettingMgr.Set(SettingComponents.Get());

           UserMgr.Save();
           DepartmentMgr.Save();
        }

        private void btn_SetDefault_Click(object sender, RoutedEventArgs e)
        {
           // SettingComponents.Set(m_SettingMgr.GetDefalut());
        }

        private void export()
        {
            try
            {
                System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();

                if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    string path = fbd.SelectedPath;
                    //SettingFile.ExportSetting(path, SettingComponents.Get());
                }
            }
            catch
            {

            }
        }

        private void import()
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.Title = "选择文件";
            openFileDialog.Filter = "配置文件|*.cfg|资源文件|*.res|所有文件|*.*";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {

                string fileName = openFileDialog.FileName;
                string extd = fileName.Substring(fileName.LastIndexOf(".") + 1, (fileName.Length - fileName.LastIndexOf(".") - 1));

                if (extd == "cfg")
                {
                    //SettingComponents.Set(SettingFile.ImportSetting(fileName));
                }

            }
        }

        private void cmb_ImExPort_Click(object sender, RoutedEventArgs e)
        {
            export();
        }

        private void cmb_ImExPort_Selected(object sender, RoutedEventArgs e)
        {
            if (cmb_ImExPort.SelectedIndex == 0)//export
            {
                export();
            }
            else if (cmb_ImExPort.SelectedIndex == 1)//import
            {
                import();
            }
        }

        private void btn_SaveUser_Click(object sender, RoutedEventArgs e)
        {
            User user = new User(txt_UserName.Text, psd_UserPassword.Password, cmb_UserType.SelectedIndex == 1 ? UserType.Admin : UserType.Guest);


            user.id = user.Add();
            lst_User.Items.Add(new ListViewItem() { Content = user });
            cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.username + (user.type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user });
        }

        private void btn_UserDel_Click(object sender, RoutedEventArgs e)
        {
            if (lst_User == null || lst_User.SelectedItem == null) return;
            User user = ((ListViewItem)lst_User.SelectedItem).Content as User;
            user.Delete();
            lst_User.Items.RemoveAt(lst_User.SelectedIndex);
            cmb_AuthorityDest.Items.RemoveAt(lst_User.SelectedIndex);
        }

        private void btn_AddFunc_Click(object sender, RoutedEventArgs e)
        {
            if (list_AllFunc == null || lst_CurrentFunc == null || list_AllFunc.SelectedItem == null) return;

            //List<CAuthority> current =  as List<CAuthority>;

           // ((List<CAuthority>)lst_CurrentFunc.ItemsSource).Add(list_AllFunc.SelectedItem as CAuthority);

            if (!lst_CurrentFunc.Items.Contains(list_AllFunc.SelectedItem as CAuthority)) lst_CurrentFunc.Items.Add(list_AllFunc.SelectedItem as CAuthority);

            if(cmb_AuthorityDest != null && cmb_AuthorityDest.SelectedItem != null)
            {
                    User  user =   (User)((ComboBoxItem)cmb_AuthorityDest.SelectedItem).Tag;
                
                    
                    List<CAuthority> lst = new List<CAuthority>();

                    foreach (var it in lst_CurrentFunc.Items)
                    {
                        lst.Add(it as CAuthority);
                    }
                    user.Auth = lst;
                    user.authority = user.parseAuth();

                    ((ComboBoxItem)cmb_AuthorityDest.Items[cmb_AuthorityDest.SelectedIndex]).Tag = user;
                    lst_User.Items[cmb_AuthorityDest.SelectedIndex] = user;

                    user.Modify();
            }

        }

        private void cmb_AuthorityDest_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {   List<CAuthority> lst = ((User)((ComboBoxItem)cmb_AuthorityDest.SelectedItem).Tag).Auth;
                lst_CurrentFunc.Items.Clear();
                foreach(CAuthority it in lst)
                {
                    if (!lst_CurrentFunc.Items.Contains(it))lst_CurrentFunc.Items.Add(it);
                }
            }
            catch { }
        }

        private void btn_SaveDepartment_Click(object sender, RoutedEventArgs e)
        {
            Department dept = null;
            try
            {
                dept = new Department(txt_GroupName.Text, long.Parse(txt_GroupId.Text));
            }
            catch(Exception ex)
            {
                DataBase.InsertLog(ex.Message);
            }
            if (dept == null) return;

            dept.id = dept.Add();
            lst_Group.Items.Add(new ListViewItem() { Content = dept });         
        }

     
    }
}
