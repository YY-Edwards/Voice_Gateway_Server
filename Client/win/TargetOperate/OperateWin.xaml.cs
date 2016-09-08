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
        private OPType Operate = OPType.Dispatch;

        private CHistory m_FirstUnReadMsg = null;

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

                if (null == m_Main) m_Main = (Main)this.Owner;

                m_View = new OpView(this);

                OnChangeOperateType();

                lst_History.View = (ViewBase)FindResource("HistoryView");

                
            };
            this.Activated += delegate {
                if (null == m_Main) m_Main = (Main)this.Owner;
                OnOperateWinActivated(); };    
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
                        if (null == m_FirstUnReadMsg)
                        {
                            m_FirstUnReadMsg = ConvertToHistory(item);
                            bdr_UnRead.Visibility = Visibility.Visible;  
                        }
                    }
                        
                }
            }));
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

        public void SetOperateType(OPType type)
        {
            Operate = type;
            OnChangeOperateType();
        }

        private void OnChangeOperateType()
        {
            switch (Operate)
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
            
            if ((false == this.IsActive) && (null == m_FirstUnReadMsg))
            {
                m_FirstUnReadMsg = msg;
                bdr_UnRead.Visibility = Visibility.Visible;
            }
            
            lst_History.Items.Add(msg);

            ListViewAutomationPeer lvap = new ListViewAutomationPeer(lst_History);
            var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
            var scroll = svap.Owner as ScrollViewer;

            scroll.ScrollToEnd();
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
            if (null != m_FirstUnReadMsg) lst_History.ScrollIntoView(m_FirstUnReadMsg);
            m_FirstUnReadMsg = null;
            bdr_UnRead.Visibility = Visibility.Collapsed;
        }

        private void btn_UnReadClose_Click(object sender, RoutedEventArgs e)
        {
            m_FirstUnReadMsg = null;
            bdr_UnRead.Visibility = Visibility.Collapsed;
        }
    }
}
