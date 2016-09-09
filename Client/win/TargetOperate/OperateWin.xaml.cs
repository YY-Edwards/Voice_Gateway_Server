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

using System.Windows.Automation.Peers;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Windows.Threading;

namespace TrboX
{
    public class CHistory
    {
        public bool istx { set; get; }
        public NotifyType type { set; get; }
        public DateTime time { set; get; }
        public object content { set; get; }
    }
    
    /// <summary>
    /// operate.xaml 的交互逻辑
    /// </summary>
    public partial class OperateWin : MyWindow
    {
        Main m_Main;
        OpView m_View;
        private CRelationShipObj m_Target = null;
        private COperate Operate = new COperate (OPType.Dispatch, null);

        private int m_FirstUnReadMsgIndex = -1;
        private CNotification m_CurrentMsg = null;

        public Main OwnerWin { set { m_Main = value; } get { return m_Main; } }

        public OperateWin(CRelationShipObj target)
            : base()
        {
            InitializeComponent();
            

            if(null == target)return;
            m_Target = target;

            this.Loaded += delegate
            {
                
                Title = m_Target.KeyName;
                SubTitle = m_Target.HeaderWithoutKey;

                m_View = new OpView(this);

                OnChangeOperateType();

                lst_History.View = (ViewBase)FindResource("HistoryView");
            };
            this.Activated += delegate {OnOperateWinActivated(); };    
        }

        private void OnOperateWinActivated()
        {
            UpdateNotify();
            m_Main.MsgWin.ClearTarget(m_Target);
        }


        private void UpdateNotify()
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                List<CNotification> notify = m_Main.MsgWin.GetNotify(true);

                foreach (CNotification item in notify)
                {
                    if (true == CRelationShipObj.Compare(item.Source, Target))
                    {
                        RxMessage(item);

                        if (-1 == m_FirstUnReadMsgIndex)
                        {
                            m_FirstUnReadMsgIndex = lst_History.Items.Count - 1;
                            bdr_UnRead.Visibility = Visibility.Visible;
                        }
                    }                       
                }

                lst_History.Dispatcher.BeginInvoke(new Action(() =>
                {
                    if (null != m_CurrentMsg)
                    {
                        lst_History.SelectedIndex = FindItemInListView(ConvertToHistory(m_CurrentMsg));
                        lst_History.UpdateLayout();
                        lst_History.ScrollIntoView(lst_History.SelectedItem);
                        m_CurrentMsg = null;
                    }
                    else
                    {
                        ListViewAutomationPeer lvap = new ListViewAutomationPeer(lst_History);
                        var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                        ((ScrollViewer)svap.Owner).ScrollToEnd();
                    }
                }), DispatcherPriority.ContextIdle);
            }));


        }

        public CNotification CurrentNotify
        {
            set {
                m_CurrentMsg = value;
            }
        }

        public CRelationShipObj Target
        {
            get
            {
                return m_Target;
            }
            set
            {
                m_Target = Target;
            }
        }

        public override void OnMyWindow_Btn_Close_Click()
        {
            this.Hide();
        }

        public void SetOperateType(COperate op)
        {
            Operate = op;
            OnChangeOperateType();
        }

        private void OnChangeOperateType()
        {
            switch (Operate.type)
            {
                case OPType.Dispatch:
                case OPType.ShortMessage:
                case OPType.JobTicker:
                case OPType.Position:
                case OPType.Tracker:
                default:
                    break;

            };
        }

        public void AddMessage(CHistory msg)
        {
            if (null == msg) return;
            
            lst_History.Items.Add(msg);

            if ((false == this.IsActive) && (-1 == m_FirstUnReadMsgIndex))
            {
                m_FirstUnReadMsgIndex = lst_History.Items.Count - 1;
                bdr_UnRead.Visibility = Visibility.Visible;
            }
            ListViewAutomationPeer lvap = new ListViewAutomationPeer(lst_History);
            var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
            ((ScrollViewer)svap.Owner).ScrollToEnd();
        }

        private int FindItemInListView(CHistory item)
        {
            for(int i  = 0; i< lst_History.Items.Count; i++)
            {
                if (JsonConvert.SerializeObject(lst_History.Items[i] as CHistory).Equals(JsonConvert.SerializeObject(item)))
                {
                    return i;
                }
            }
            return -1;
        }

        private CHistory ConvertToHistory(CNotification notify)
        {
            string content = "";
            
            switch (notify.Type)
            {
                case NotifyType.Alarm:
                    content = ((CAlarmNotification)notify.Content).Content;
                    break;
                case NotifyType.Call:
                    content = "语音呼叫";
                    break;
                case NotifyType.Message:
                    content = ((CMsgNotification)notify.Content).Content;
                    break;
                case NotifyType.JobTicker:
                    content = "工单";
                    break;
                case NotifyType.Tracker:
                    content = "巡更";
                    break;
            }
            return new CHistory()
            {
                istx = false,
                type = notify.Type,
                time = notify.Time,
                content = content,
            };
        }

        public void RxMessage(CNotification notify)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                AddMessage(ConvertToHistory(notify));

                if (true == this.IsActive)
                {
                    m_Main.MsgWin.ClearTarget(m_Target);
                }
            }));               
        }


        private void btn_PTT_Click(object sender, RoutedEventArgs e)
        {
            AddMessage(new CHistory()
            { 
            istx = true,
            type = NotifyType.Call,
            time = DateTime.Now,
             content = "语音呼叫"
            });
        }

        private void btn_SendMsg_Click(object sender, RoutedEventArgs e)
        {

            AddMessage(new CHistory()
            { 
            istx = true,
            type = NotifyType.Message,
            time = DateTime.Now,
            content = txt_Message.Text
            });
        }

        private void bdr_UnRead_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (-1 != m_FirstUnReadMsgIndex)
            {
                lst_History.SelectedIndex = m_FirstUnReadMsgIndex;
            }
            
            m_FirstUnReadMsgIndex = -1;
            bdr_UnRead.Visibility = Visibility.Collapsed;
        }

        private void btn_UnReadClose_Click(object sender, RoutedEventArgs e)
        {
            m_FirstUnReadMsgIndex = -1;
            bdr_UnRead.Visibility = Visibility.Collapsed;
        }


        private void lst_History_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            lst_History.Dispatcher.BeginInvoke(new Action(() =>
            {
                lst_History.UpdateLayout();
                if (null != lst_History.SelectedItem)
                {
                    lst_History.ScrollIntoView(lst_History.SelectedItem);
                }
                else
                {
                    ListViewAutomationPeer lvap = new ListViewAutomationPeer(lst_History);
                    var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                    ((ScrollViewer)svap.Owner).ScrollToEnd();
                }
            }), DispatcherPriority.ContextIdle);
        }

        private void btn_AddFastPanel_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
