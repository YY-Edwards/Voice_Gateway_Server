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
using System.Windows.Forms.Integration;

namespace TrboX
{
    /// <summary>
    /// Main.xaml 的交互逻辑
    /// </summary>
    public partial class Main : MyWindow
    {
        MyWebBrowse Map = new MyWebBrowse("http://www.baidu.com");
        public Main()
        {
            InitializeComponent();
            WindowsFormsHost host = new WindowsFormsHost();
        }

        private void MyWindow_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void MyWindow_Loaded(object sender, RoutedEventArgs e)
        {

           

            

            MyWebGrid.Children.Insert(0,Map);
            
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Map.View("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
        }
    }
}
