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
        public MainView m_View;
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
                m_View.m_NotifyView.m_Notify.Save();
                Environment.Exit(0);
            };
        }

        public override void OnMouseL_R_Prssed()
        {
           // m_View.On_Mouse_Pressed();
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

            //msglist
           
        }



        public void OnOrganizationMenu_Click(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = ((ContextMenu)((MenuItem)sender).Parent).PlacementTarget as TreeViewItem;

            switch ((string)((MenuItem)sender).Tag)
            {
                case "fast":
                    m_FastOperateWin.Add(new FastOperate()
                     {
                         m_Type = FastType.FastType_Contact,
                         m_Contact = ((COrganization)item.Tag).target
                     });
                    break;
            };


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

       
        private void btn_Tool_NewContact_Click(object sender, RoutedEventArgs e)
        {

        }

        private void FastPanel_Closing(object sender, RoutedEventArgs e)
        {
            FastOperate it = (FastOperate)((FastPanel)sender).DataContext;
            m_FastOperateWin.Remove(it);
        }

        private void tree_OrgView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {

        }

        private void tree_OrgView_PreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

        }

        private int index = 0;

        private void btn_Tool_NewFastOperate_Click(object sender, RoutedEventArgs e)
        {
            FastOperate op = new FastOperate()
            {
                m_Type = FastType.FastType_Operate,
                //m_Contact = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                //    , new CEmployee() { id = 2, name = "崔二胯子" }
                //    , new CVehicle() { id = 12, number = "牛B74110" }
                //    , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),

                //m_Operate = new COperate(OPType.Dispatch, new CDispatch() {target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                //    , new CEmployee() { id = 2, name = "崔二胯子" }
                //    , new CVehicle() { id = 12, number = "牛B74110" }
                //    , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }), targettype = TargetType.Radio})

                //m_Operate = new COperate(OPType.ShortMessage, new CShortMessage()
                //{
                //    target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                //        , new CEmployee() { id = 2, name = "崔二胯子" }
                //        , new CVehicle() { id = 12, number = "牛B74110" }
                //        , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),
                //    targettype = TargetType.Radio,
                //    message = "hello,test msg"
                //}

                
                m_Operate = new COperate(OPType.Position, new CPosition()
                {
                    target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                        , new CEmployee() { id = 2, name = "崔二胯子" }
                        , new CVehicle() { id = 12, number = "牛B74110" }
                        , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),
                    targettype = TargetType.Radio,
                    iscsbk = true,
                    isenh = false,
                    iscycle = false,
                }
                
                )
            };

            m_FastOperateWin.Add(op);

            index++;
        }

        private void btn_AlarmNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            CAlarmNotification it = (CAlarmNotification)((Button)sender).DataContext;
            m_View.m_NotifyView.m_Notify.Remove(it);
        }

        private void btn_MsgNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            CMsgNotification it = (CMsgNotification)((Button)sender).DataContext;
            m_View.m_NotifyView.m_Notify.Remove(it);
        }

        private void btn_RxNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            CRxNotification it = (CRxNotification)((Button)sender).DataContext;
            m_View.m_NotifyView.m_Notify.Remove(it);
        }

        private void btn_JobNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            CJobTicketNotification it = (CJobTicketNotification)((Button)sender).DataContext;
            m_View.m_NotifyView.m_Notify.Remove(it);
        }
        private void btn_TrackerNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            CTackerNotification it = (CTackerNotification)((Button)sender).DataContext;
            m_View.m_NotifyView.m_Notify.Remove(it);
        }
    }
}
