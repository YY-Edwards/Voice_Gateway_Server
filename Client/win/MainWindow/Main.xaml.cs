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

    public class itemstc
    {
        public string name { set; get; }
        public bool sex { set; get; }
        public int age { set; get; }
    }
    
    /// <summary>
    /// Main.xaml 的交互逻辑
    /// </summary>
    public partial class Main : MyWindow
    {
        MainView m_View;
        FastOperateWindow m_FastOperateWin;

        
        public TargetMgr m_Target = new TargetMgr();

        
        MyWebBrowse Map = new MyWebBrowse("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
        public Main()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                this.WindowState = WindowState.Maximized;
                m_View = new MainView(this);


                

                OnWindowLoaded();
            };
            this.Closed += delegate
            {
                m_FastOperateWin.Save();
                Environment.Exit(0);
            };
        }

        public override void OnMouseL_R_Prssed()
        {
            m_View.On_Mouse_Pressed();
        }


        private void OnWindowLoaded()
        {
           // organzation tree
            m_Target.UpdateTragetList();
            m_View.FillDataToOrgTreeView();

            //dispatch
            m_FastOperateWin = new FastOperateWindow(this);
            lst_dispatch.View = (ViewBase)this.FindResource("ImageView");

            //map
            MyWebGrid.Children.Insert(0, Map);

           
            
        }






        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Map.View("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
        }

        //About
        private void menu_Help_About_Click(object sender, RoutedEventArgs e)
        {
            About aboutwin = new About();
            aboutwin.ShowDialog();
        }

        private void btn_About_Click(object sender, RoutedEventArgs e)
        {
            About aboutwin = new About();
            aboutwin.ShowDialog();
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

        private void lst_dispatch_Loaded(object sender, RoutedEventArgs e)
        {
            //itemstc it = new itemstc
            //{
            //    name = "AddItem"
            //};

            //btn_add.Content = it;
            
        }

        private int index = 0;
        private void btn_Tool_NewContact_Click(object sender, RoutedEventArgs e)
        {

            FastOperate op = new FastOperate()
            {
                m_Type = (index % 2 == 0) ? FastType.FastType_Contact : FastType.FastType_Operate,
                m_Contact = new CRelationShipObj(OrgItemType.Type_Group, new CGroup() {id = 1, name = "地勤" }, null,null, null),
            };

            m_FastOperateWin.Add(op);
            
            index++;
        }

        private void FastPanel_Closing(object sender, RoutedEventArgs e)
        {
            FastOperate it = (FastOperate)((FastPanel)sender).DataContext;
            m_FastOperateWin.Remove(it);
        }
    }
}
