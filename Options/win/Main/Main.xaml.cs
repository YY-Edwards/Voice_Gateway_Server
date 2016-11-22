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


                TServer.InitializeServer();
                LogServer.InitializeServer();
                SettingComponents = new SettingComponents(this);
                ResourceComponents = new ResourceComponents(this);

                SettingComponents.Set(SettingMgr.Get());


                
               
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
                        case "MNIS":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioMnis as FrameworkElement));
                            break;
                        case "GPS翻转":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioGPS as FrameworkElement));
                            break;
                        case "功能":
                            scrview_Radio.ScrollToVerticalOffset(PosInScrView(scrview_Radio, dck_RadioFunc as FrameworkElement));
                            break;
                    }
                    break;
                case "中继台":
                    switch ((string)item.Header)
                    {
                        case "常规":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanGeneric as FrameworkElement));
                            break;
                        case "网络":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanNetwork as FrameworkElement));
                            break;
                        case "MNIS":
                            scrview_WireLan.ScrollToVerticalOffset(PosInScrView(scrview_WireLan, dck_WireLanMnis as FrameworkElement));
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
           StaffMgr.Save();
           RadioMgr.Save();


           DepartmentMgr.SaveDeptStaff();
           DepartmentMgr.SaveDeptRadio();

           StaffMgr.SaveStaffRadio();
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

    }
}
