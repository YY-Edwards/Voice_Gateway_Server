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

using System.Windows.Controls.Primitives; 

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
        
        public MainView m_View;



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

        public new static readonly DependencyProperty IsNeedSaveWorkSpace =
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
            //FastOperate it = (FastOperate)((FastPanel)sender).DataContext;
            //m_FastOperateWin.Remove(it);
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

            //m_FastOperateWin.Add(op);

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


        private void FastPanel_TiltePressed(object sender, RoutedEventArgs e)
         {
             FastOperate it = (FastOperate)((FastPanel)sender).DataContext;

             Point point = ((FastPanel)sender).TranslatePoint(new Point(0, 0), lst_dispatch);

             object data = GetDataFromListBox(lst_dispatch, point);
             if (data != null)
             {
                 DragDrop.DoDragDrop(this, data, DragDropEffects.Move);
             }
         }

        private static object GetDataFromListBox(ListBox source, Point point)
        {
            UIElement element = source.InputHitTest(point) as UIElement;
            if (element != null)
            {
                object data = DependencyProperty.UnsetValue;
                while (data == DependencyProperty.UnsetValue)
                {
                    data = source.ItemContainerGenerator.ItemFromContainer(element);
                    if (data == DependencyProperty.UnsetValue)
                    {
                        element = VisualTreeHelper.GetParent(element) as UIElement;
                    }
                    if (element == source)
                    {
                        return null;
                    }
                }
                if (data != DependencyProperty.UnsetValue)
                {
                    return data;
                }
            }
            return null;
        }


        //drage item
        int SourceIndex = -1;
        delegate Point GetPositionDelegate(IInputElement element);  


        private void lst_dispatch_Drop(object sender, DragEventArgs e)
        {
            int TargetIndex = GetCurrentIndex(new GetPositionDelegate(e.GetPosition));

            //m_FastOperateWin.ChangePosition(SourceIndex, TargetIndex);

            //if ((SourceIndex > -1) && (SourceIndex < lst_dispatch.Items.Count))
            //{
            //    FastOperate Source = lst_dispatch.Items[SourceIndex] as FastOperate;
            //    if (null != Source)
            //    {
            //        //m_FastOperateWin_
            //        m_FastOperateWin.Remove(Source);
            //    }

            //    if (null != Source) m_FastOperateWin.Remove(Source);
            //}

            //if ((TargetIndex > -1) && (TargetIndex < lst_dispatch.Items.Count))
            //{
                
            //}

        }

        ListViewItem GetListViewItem(int index)
        {
            if (lst_dispatch.ItemContainerGenerator.Status != GeneratorStatus.ContainersGenerated)
                return null;

            return lst_dispatch.ItemContainerGenerator.ContainerFromIndex(index) as ListViewItem;
        }  

        private int GetCurrentIndex(GetPositionDelegate getPosition)
        {
            int index = -1;
            for (int i = 0; i < lst_dispatch.Items.Count; ++i)
            {
                ListViewItem item = GetListViewItem(i);
                if (item != null && this.IsMouseOverTarget(item, getPosition))
                {
                    index = i;
                    break;
                }
            }
            return index;
        }

        bool IsMouseOverTarget(Visual target, GetPositionDelegate getPosition)
        {
            Rect bounds = VisualTreeHelper.GetDescendantBounds(target);
            Point mousePos = getPosition((IInputElement)target);
            return bounds.Contains(mousePos);
        } 



        private void lst_dispatch_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
           SourceIndex = this.GetCurrentIndex(e.GetPosition);

           if (SourceIndex < 0) return;

           lst_dispatch.SelectedIndex = SourceIndex;
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
            //m_FastOperateWin.Save();
            m_View.m_NotifyView.m_Notify.Save();
            g_IsNeedSaveWorkSpace = false;
        }

    }
}
