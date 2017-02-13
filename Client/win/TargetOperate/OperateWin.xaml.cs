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
        private CMultMember m_Target = new CMultMember();
        private COperate Operate = new COperate(OPType.Dispatch, new CMultMember() { Type = SelectionType.Single, Target = new List<CMember>() }, null);

        private int m_FirstUnReadMsgIndex = -1;
        private CNotification m_CurrentMsg = null;

        public Main OwnerWin { set { m_Main = value; } get { return m_Main; } }

        public OperateWin(CMultMember target)
            : base()
        {
            InitializeComponent();
            
            if(null == target)return;
            m_Target = target;

            this.Loaded += delegate
            {
                if (m_Target == null) return;
                if (SelectionType.All == m_Target.Type)
                {
                    txt_Title.Text = "全部设备";
                    txt_SubTitle.Text = "";
                }
                else if (SelectionType.Single == m_Target.Type)
                {
                    txt_Title.Text = m_Target.Target[0].Name;
                    txt_SubTitle.Text = m_Target.Target[0].Information;
                }
                else if (SelectionType.Multiple == m_Target.Type)
                {
                    txt_Title.Text = m_Target.Target[0].Name + "、" + m_Target.Target[1].Name + " 等共" + m_Target.Target.Count.ToString() + "人";
                }
                else
                {
                    return;
                }

                if (m_Target.Type == SelectionType.All)
                {
                    if (TargetMgr.IsTx)
                    {
                        chk_PTT.IsChecked = true;
                        WindowBackground = MyWindow.TargetInCallBrush;
                    }
                    if (TargetMgr.IsRx)
                    {
                        WindowBackground = MyWindow.TargetInCallBrush;
                    }
                }
                else if (m_Target.Target != null && m_Target.Target.Count > 0)
                {
                    foreach (CMember mem in m_Target.Target)
                    {
                        if (mem.Type == MemberType.Group)
                        {
                            if (mem.Group.IsTx)
                            {
                                chk_PTT.IsChecked = true;
                                WindowBackground = MyWindow.TargetInCallBrush;

                            }
                            if (mem.Group.IsRx)
                            {
                                WindowBackground = MyWindow.TargetInCallBrush;
                            }

                        }
                        else
                        {
                            if (mem.Radio.IsTx)
                            {
                                chk_PTT.IsChecked = true;
                                WindowBackground = MyWindow.TargetInCallBrush;
                            }

                            if (mem.Radio.IsRx)
                            {
                                WindowBackground = MyWindow.TargetInCallBrush;
                            }

                            if (mem.Radio.IsGPS) chk_QueryCyclePosition.IsChecked = true;
                        }
                    }
                }


                m_Target.DisableFunc(
                    delegate{
                        chk_PTT.IsEnabled = false;
                        //rad_Call.IsEnabled = false;
                        //rad_Message.IsEnabled = false;
                        //rad_Position.IsEnabled = false;
                        //rad_Jobticket.IsEnabled = false;  
                        btn_Control.IsEnabled = false;
                        btn_Check.IsEnabled = false;
                        btn_Minitor.IsEnabled = false;
                    },
                     null, null, null, null, 
                    delegate{
                        rad_Position.Visibility = Visibility.Collapsed;
                    },
                    delegate{
                        rad_Message.Visibility = Visibility.Collapsed;
                    },
                    null);

                m_View = new OpView(this);

                updatecyclelist(null, null);

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


        public void UpdateSta(int mask, bool sta)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                if ((mask & 1) != 0)//olnline
                {
                    txt_SubTitle.Text = "(在线)" + txt_SubTitle.Text;
                }
                else if ((mask & 2) != 0)//ingps
                {
                    chk_QueryCyclePosition.IsChecked = sta;
                }
                else if ((mask & 4) != 0)//isTx
                {
                    chk_PTT.IsChecked = sta;

                    if (sta) WindowBackground = MyWindow.TargetInCallBrush;
                    else WindowBackground = new SolidColorBrush(Color.FromArgb(255, 151, 197, 247));
                }
                else if ((mask & 8) != 0)//isRx
                {
                    if (sta) WindowBackground = MyWindow.TargetInCallBrush;
                    else WindowBackground = new SolidColorBrush(Color.FromArgb(255, 151, 197, 247));
                }
            })); 
        }

        private void UpdateNotify()
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                List<CNotification> notify = m_Main.MsgWin.GetNotify(true);

                foreach (CNotification item in notify)
                {
                    if(item.Source.IsLike(m_Target))
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

        public CMultMember Target
        {
            get
            {
                if (null == m_Target) m_Target = new CMultMember();
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
            switch (Operate.Type)
            {
                case OPType.Dispatch:
                    rad_Call.IsChecked = true;break;
                case OPType.ShortMessage:
                    rad_Message.IsChecked = true;break;
                case OPType.JobTicker:
                    rad_Jobticket.IsChecked = true;break;
                case OPType.Position:
                    rad_Position.IsChecked = true;break;

                case OPType.Tracker:
                default:
                    break;
            };
        }

        public void AddMessage(CHistory msg)
        {
              this.Dispatcher.Invoke(new Action(() =>
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
            }));    
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
                if (true == this.IsActive) m_Main.MsgWin.ClearTarget(m_Target);                       
            }));               
        }

        private void btn_SendMsg_Click(object sender, RoutedEventArgs e)
        {

            new COperate()
            {
                Type = OPType.ShortMessage,
                Target = m_Target,
                Operate = new CShortMessage() { Message = txt_Message.Text },
            }.Exec();

            AddMessage(new CHistory()
            { 
            istx = true,
            type = NotifyType.Message,
            time = DateTime.Now,
            content = txt_Message.Text
            });
        }

        private bool clickincloseunread = false;
        private void bdr_UnRead_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!clickincloseunread)            
            if (-1 != m_FirstUnReadMsgIndex)
            {
                lst_History.SelectedIndex = m_FirstUnReadMsgIndex;
            }
            
            m_FirstUnReadMsgIndex = -1;
            bdr_UnRead.Visibility = Visibility.Collapsed;

            clickincloseunread = false;
        }

        private void btn_UnReadClose_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            clickincloseunread = true;
        }

        private void btn_UnReadClose_Click(object sender, RoutedEventArgs e)
        {
            //m_FirstUnReadMsgIndex = -1;
            //bdr_UnRead.Visibility = Visibility.Collapsed;
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

        private void btn_QueryPosition_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                new COperate()
                {
                    Type = OPType.Position,
                    Target = m_Target,
                    Operate = new CPosition() { Type = ExecType.Start, IsCycle = (bool)chk_IsCycle.IsChecked, IsCSBK = (bool)chk_IsCSBK.IsChecked, IsEnh = (bool)chk_IsEnh.IsChecked }
                }.Exec();
            }
            catch { }

            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Position,
                time = DateTime.Now,
                content = "位置查询"
            });
        }


        private void updatecyclelist(object sender, RoutedEventArgs e)
        {
            if (cmb_Cycle == null || chk_IsCSBK == null || chk_IsEnh == null) return;
            
            List<double> cyclelist = CPosition.UpdateCycleList((bool)chk_IsCSBK.IsChecked, (bool)chk_IsEnh.IsChecked);
            cmb_Cycle.Items.Clear();
            foreach (double cycle in cyclelist)
                cmb_Cycle.Items.Add(new ComboBoxItem()
                {
                    Content = cycle.ToString() + "s",
                    Tag = cycle,
                    Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                    Foreground = new SolidColorBrush(Color.FromArgb(255, 210, 223, 245)),
                    FontSize = 13,
                    Height = 32
                });
            cmb_Cycle.SelectedIndex = 0;

            if (false == chk_IsCSBK.IsChecked) chk_IsEnh.IsChecked = false;
        }


        private void btn_Control_Click(object sender, RoutedEventArgs e)
        {
            new COperate()
            {
                Type = OPType.Control,
                Target = m_Target,
                Operate = new CControl() { Type  = ControlType.ShutDown }
            }.Exec();

            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Control,
                time = DateTime.Now,
                content = "遥毙"
            });
        }

        private void btn_Control_Selected(object sender, RoutedEventArgs e)
        {
            if (btn_Control.SelectedIndex == 0)
            {
                new COperate()
                {
                    Type = OPType.Control,
                    Target = m_Target,
                    Operate = new CControl() { Type = ControlType.ShutDown }
                }.Exec();

                AddMessage(new CHistory()
                {
                    istx = true,
                    type = NotifyType.Control,
                    time = DateTime.Now,
                    content = "遥毙"
                });
            }
            else if (btn_Control.SelectedIndex == 1)
            {
                new COperate()
                {
                    Type = OPType.Control,
                    Target = m_Target,
                    Operate = new CControl() { Type = ControlType.StartUp }
                }.Exec();

                AddMessage(new CHistory()
                {
                    istx = true,
                    type = NotifyType.Control,
                    time = DateTime.Now,
                    content = "遥开"
                });
            } 
        }

        private void btn_Check_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                new COperate()
                {
                    Type = OPType.Control,
                    Target = m_Target,
                    Operate = new CControl() { Type = ControlType.Check }
                }.Exec();

                AddMessage(new CHistory()
                {
                    istx = true,
                    type = NotifyType.Control,
                    time = DateTime.Now,
                    content = "在线检测"
                });
            }
            catch { }
        }

        private void btn_Minitor_Click(object sender, RoutedEventArgs e)
        {
            new COperate()
            {
                Type = OPType.Control,
                Target = m_Target,
                Operate = new CControl() { Type = ControlType.Monitor }
            }.Exec();

            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Control,
                time = DateTime.Now,
                content = "远程监听"
            });
        }

        private void chk_PTT_Click(object sender, RoutedEventArgs e)
        {
            chk_PTT.IsChecked = !chk_PTT.IsChecked;


            if(!(bool)new COperate()
            {
                Type = OPType.Dispatch,
                Target = m_Target,
                Operate = new CDispatch() { Exec = (bool)chk_PTT.IsChecked ?  ExecType.Stop : ExecType.Start}
            }.Exec())
            {              
               return;
            }

            if((bool)chk_PTT.IsChecked)
            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Call,
                time = DateTime.Now,
                content = "结束呼叫"
            });
        }

        private void chk_QueryCyclePosition_Click(object sender, RoutedEventArgs e)
        {
            chk_QueryCyclePosition.IsChecked = !(bool)chk_QueryCyclePosition.IsChecked;

            COperate operate =  new COperate()
            {
                Type = OPType.Position,
                Target = m_Target,
                Operate = new CPosition()
                {
                    Type = (bool)chk_QueryCyclePosition.IsChecked ? ExecType.Stop : ExecType.Start,                
                    IsCycle = (bool)chk_IsCycle.IsChecked, 
                    IsCSBK = (bool)chk_IsCSBK.IsChecked, 
                    IsEnh = (bool)chk_IsEnh.IsChecked,
                    Cycle = (double)((ComboBoxItem)cmb_Cycle.SelectedItem).Tag
                }
            };

            operate.Exec();


            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Position,
                time = DateTime.Now,
                content = (bool)chk_QueryCyclePosition.IsChecked ? "停止周期查询" : "开始周期查询",
            });

            try
            {
                if (((CPosition)operate.Operate).IsCycle || ((CPosition)operate.Operate).Type == ExecType.Stop)
                {
                    if (operate.Target.Type == SelectionType.All)
                    {
                        var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                        foreach (var item in radio) m_Main.ResrcMgr.SetGpsOnline(item.Value.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start);

                    }
                    else if (operate.Target.Type != SelectionType.Null)
                    {
                        foreach (CMember trgt in operate.Target.Target)
                        {
                            if (trgt.Type == MemberType.Group)
                            {
                                if (trgt.Group == null || trgt.Group.ID <= 0) continue;
                                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == trgt.Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                                foreach (var item in radio) m_Main.ResrcMgr.SetGpsOnline(item.Value.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start);
                            }
                            else
                            {
                                if (trgt.Radio == null || trgt.Radio.ID <= 0) continue;
                                m_Main.ResrcMgr.SetGpsOnline(trgt.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start);
                            }
                        }
                    }
                }              
            }
            catch { }

        }

        private void btn_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void btn_SysClose_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void btn_Control_Click_1(object sender, RoutedEventArgs e)
        {
            if (!(bool)new COperate(
           
                OPType.Control,
                m_Target,
                new CControl() { Type = ControlType.ShutDown }) .Exec())
            {
                return;
            }

            AddMessage(new CHistory()
            {
                istx = true,
                type = NotifyType.Call,
                time = DateTime.Now,
                content = "遥毙"
            });
        }

        private void btn_Control_Selected_1(object sender, RoutedEventArgs e)
        {
            switch(btn_Control.SelectedIndex)
            {
                case 0:
                     if (!(bool)new COperate(
           
                        OPType.Control,
                        m_Target,
                        new CControl() { Type = ControlType.ShutDown }) .Exec())
                    {
                        return;
                    }

                    AddMessage(new CHistory()
                    {
                        istx = true,
                        type = NotifyType.Call,
                        time = DateTime.Now,
                        content = "遥毙"
                    });
                    break;
                case 1:
                   if (!(bool)new COperate(
           
                        OPType.Control,
                        m_Target,
                        new CControl() { Type = ControlType.StartUp }) .Exec())
                    {
                        return;
                    }

                    AddMessage(new CHistory()
                    {
                        istx = true,
                        type = NotifyType.Call,
                        time = DateTime.Now,
                        content = "遥开"
                    });
                    break;
                    

            }

        }
    }
}
