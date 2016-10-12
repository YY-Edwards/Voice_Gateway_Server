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

using System.Linq;
using System.Xml.Linq;

namespace TrboX
{

   
    /// <summary>
    /// Login.xaml 的交互逻辑
    /// </summary>
    public partial class Main : MyWindow
    {
        public XmlDocument xmlDoc = new XmlDocument();
        public Main()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                //check conf files
                string path = AppDomain.CurrentDomain.BaseDirectory + "conf.xml";
                if (!File.Exists(path))
                {
                    Uri uri = new Uri("/DefaultConfiguration.xml", UriKind.Relative);
                    StreamResourceInfo info = Application.GetResourceStream(uri);
                    StreamWriter sw = new StreamWriter(path);
                    info.Stream.CopyTo(sw.BaseStream);
                    sw.Flush();
                    sw.Close();
                }

                try
                {
                    xmlDoc.Load(path);
                   // FileStream ConfFile = new FileStream(path, FileMode.Open);
                }
                catch
                {
                    Console.Write("找不到配置文件 conf.xml");
                }

                InitializeComponentContent();
               
            };
        }

        private void InitializeComponentContent()
        {

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
                
               // element.TransformToVisual(scr);
            return transform.Transform(new Point(element.Margin.Left, element.Margin.Top)).Y - 10; 
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
                default: break;
            }

        }
    }
}
