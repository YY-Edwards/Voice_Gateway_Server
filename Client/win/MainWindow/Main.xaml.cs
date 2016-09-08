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
        public MainMenu MenuBar;
        public MainTool ToolBar;

        public MainView View;

        public MainResourceMgr ResrcMgr;
        public MainArea WorkArea;

        public EventWin EventList;
        public MainMsgWin MsgWin;

        public NewWinMgr SubWindow;        


        public COrganization CurrentTraget = null;

        public bool g_IsNeedSaveWorkSpace
        {
            get {
                return (bool)GetValue(IsNeedSaveWorkSpace); 
            }
            set
            {
                SetValue(IsNeedSaveWorkSpace, value);
            }
        }

        public static readonly DependencyProperty IsNeedSaveWorkSpace =
            DependencyProperty.Register("NeedSave", typeof(bool), typeof(Main), new PropertyMetadata(false));

        public Main()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                WindowState = WindowState.Maximized;

                MenuBar = new MainMenu(this);
                ToolBar = new MainTool(this);

                View = new MainView(this);

                ResrcMgr = new MainResourceMgr(this);
                WorkArea = new MainArea(this);
                EventList = new EventWin(this);
                MsgWin = new MainMsgWin(this);
                MsgWin.Initialize();

                SubWindow = new NewWinMgr(this);

                OnWindowLoaded();
            };
            this.Closed += delegate
            {
                Environment.Exit(0);
            };
        }

        private void OnWindowLoaded()
        {
           // organzation tree
            //m_Target.UpdateTragetList();
            //m_View.FillDataToOrgTreeView();

            ////dispatch
            //lst_dispatch.View = (ViewBase)this.FindResource("ImageView");

            ////map
            //MyWebGrid.Children.Insert(0, Map);
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

            //m_FastOperateWin.Add(op);

            index++;
        }




        private void btn_AlarmNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }

        private void btn_MsgNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }

        private void btn_RxNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }

        private void btn_JobNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }
        private void btn_TrackerNotify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }


        private void FastPanel_TiltePressed(object sender, RoutedEventArgs e)
        {
             WorkArea.PastPanelPressed(sender, e);
        }

        private void FastPanel_Closing(object sender, RoutedEventArgs e)
        {
            WorkArea.FastPanel.Remove((FastOperate)((FastPanel)sender).DataContext);
        }

        private void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (true == g_IsNeedSaveWorkSpace)
            {

                WarnningWindow warn = new WarnningWindow();
                warn.ShowDialog();

                switch (warn.Reslut)
                {
                    case ResultType.Enter:
                        SaveWorkSpace();
                        break;
                    case ResultType.Exit:
                        break;
                    case ResultType.Cancle:
                        e.Cancel = true;
                        break;

                }
            }           
        }

        public void SaveWorkSpace()
        {
            WorkArea.FastPanel.Save();
            MsgWin.SaveNotify();
            g_IsNeedSaveWorkSpace = false;
        }

        private void tree_OrgView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {

        }
    }
}
