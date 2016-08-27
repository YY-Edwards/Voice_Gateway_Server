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
        MainView m_View;


        DataTst json = new DataTst();
        
        MyWebBrowse Map = new MyWebBrowse("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
        public Main()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                this.WindowState = WindowState.Maximized;
                m_View = new MainView(this);

                MyWebGrid.Children.Insert(0, Map);
            };
            this.Closed += delegate
            {
                Environment.Exit(0);
            };
        }

        public override void OnMouseL_R_Prssed()
        {
            m_View.On_Mouse_Pressed();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Map.View("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            TesTitle win = new TesTitle();
            win.Show();
        }

        //About
        private void menu_Help_About_Click(object sender, RoutedEventArgs e)
        {
            About aboutwin = new About();
            aboutwin.Show();
        }

        private void btn_About_Click(object sender, RoutedEventArgs e)
        {
            About aboutwin = new About();
            aboutwin.Show();
        }

        private void btn_Tool_PTT_Click(object sender, RoutedEventArgs e)
        {
            OperateWin Operate = new OperateWin();
            Operate.Show();
        }

        private void btn_Tool_Msg_Click(object sender, RoutedEventArgs e)
        {
            OperateWin Operate = new OperateWin();
            Operate.Show();
        }

        private void btn_Tool_Position_Click(object sender, RoutedEventArgs e)
        {
            OperateWin Operate = new OperateWin();
            Operate.Show();
        }

        private void btn_Tool_Job_Click(object sender, RoutedEventArgs e)
        {
            OperateWin Operate = new OperateWin();
            Operate.Show();
        }
    }
}
