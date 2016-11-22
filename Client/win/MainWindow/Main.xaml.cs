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

using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Windows.Threading;

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

        public Status StatusBar;

        public CMultMember CurrentTraget = null;

        DispatcherTimer myTimer = new DispatcherTimer();//定时周期2秒

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
                RegRxHandler();
                TServer.InitializeServer();
                LogServer.InitializeServer();  
     
                WindowState = WindowState.Maximized;

                MenuBar = new MainMenu(this);
                ToolBar = new MainTool(this);
                StatusBar = new Status(this);

                View = new MainView(this);

                ResrcMgr = new MainResourceMgr(this);
                WorkArea = new MainArea(this);
                EventList = new EventWin(this);
                MsgWin = new MainMsgWin(this);
                MsgWin.Initialize();

                       

                SubWindow = new NewWinMgr(this);
                //Rpc = new RpcInterface();

                OnWindowLoaded();

               

                //new Thread(new ThreadStart(delegate() {
                //    ReceiveMsghread();                            
                //})).Start();

                //ResrcMgr.SetRadioOnline(9, true);
                //ResrcMgr.SetRadioOnline(10, true);
                //ResrcMgr.SetRadioOnline(9, false);

                //RadioOperate.GetStatus(1);

                myTimer.Interval = TimeSpan.FromSeconds(60);
                myTimer.Tick += new EventHandler(OnTimer60s);

                DataBase.InsertLog("登陆软件");
            };
            this.Closed += delegate
            {
                Environment.Exit(0);
            };
        }


        private void OnTimer60s(object sender, EventArgs e)
        {
            EventList.AddEvent("提示：呼叫结束，呼叫超时");
            TServer.IsInCalled = false;
            if(RadioOperate.LastCall !=null)
            {
                ResrcMgr.SetTx(RadioOperate.LastCall.Type, RadioOperate.LastCall.Target, false);  
            }

            myTimer.Stop();
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

        private void RegRxHandler()
        {
            TServer.RegRxHanddler(RequestType.status,  OnRadioStatus);
            TServer.RegRxHanddler(RequestType.callStatus, OnRadioCallStatus);
            TServer.RegRxHanddler(RequestType.message, OnRadioMessage);
            TServer.RegRxHanddler(RequestType.messageStatus, OnRadioMessageStatus);

            TServer.RegRxHanddler(RequestType.sendArs, OnRadioArs);
            TServer.RegRxHanddler(RequestType.sendGps, OnRadioGps);
            TServer.RegRxHanddler(RequestType.queryGpsStatus, OnRadioGpsStatus);
            

            TServer.RegRxHanddler(RequestType.controlStatus, OnControlStatus);

            TServer.RegRxHanddler(RequestType.wlInfo, OnRadioStatus);
            TServer.RegRxHanddler(RequestType.wlCall,  OnWirelanCall);
            TServer.RegRxHanddler(RequestType.wlCallStatus,  OnWirelanCallStatus);

            TServer.RegRxHanddler(RequestType.wlPlayStatus, OnWirelanPlayStatus);
        }

        public override void OnCustomMsg(CustomMessage dest)
        {
            switch(dest.Type)
            {
                case DestType.AddEvent:
                    EventList.AddEvent(dest.Contents);
                    break;
                case DestType.OnConnectTServer:
                    StatusBar.GetRunMode();

                    if(StatusBar.Get().type == RunMode.Radio)
                    {
                        RadioOperate.GetStatus(1);
                        RadioOperate.GetStatus(2);
                    }
                    else if (StatusBar.Get().type == RunMode.Repeater)
                    {
                        WirelanOperate .GetStatus(1);
                    }

                    break;
            }
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

        private void btn_Notify_Close_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.RemoveNotify(((Button)sender).DataContext);
        }

        private void btn_Notify_Explaner_Click(object sender, RoutedEventArgs e)
        {
            MsgWin.ExplanerNotify(((Button)sender).DataContext);
        }


        private void FastPanel_TiltePressed(object sender, RoutedEventArgs e)
        {
            WorkArea.PastPanelPressed(sender, e);
        }

        private void FastPanel_Closing(object sender, RoutedEventArgs e)
        {
            WorkArea.FastPanel.Remove((FastOperate)((FastPanel)sender).DataContext);
        }


        private void dis_Operate(object sender, RoutedEventArgs e)
        {
            COperate operate = ((FastOperate)((FastPanel)sender).DataContext).Operate;
            if (null != operate)
            {
                operate.Exec();
                try
                {
                    if (operate.Type == OPType.Position)
                    {
                        if (((CPosition)operate.Operate).IsCycle || ((CPosition)operate.Operate).Type == ExecType.Stop) 
                        {
                            if (operate.Target.Type == SelectionType.All)
                            {
                                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                                foreach (var item in radio)ResrcMgr.SetGpsOnline(item.Value.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start);              
                               
                            }
                            else if (operate.Target.Type != SelectionType.Null)
                            {
                                foreach (CMember trgt in operate.Target.Target)
                                {
                                    if (trgt.Type == MemberType.Group)
                                    {
                                        if (trgt.Group == null || trgt.Group.ID <= 0) continue;
                                        var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == trgt.Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                                        foreach (var item in radio) ResrcMgr.SetGpsOnline(item.Value.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start); 
                                    }
                                    else
                                    {
                                        if (trgt.Radio == null || trgt.Radio.ID <= 0) continue;
                                        ResrcMgr.SetGpsOnline(trgt.Radio.RadioID, ((CPosition)operate.Operate).Type == ExecType.Start);
                                    }
                                }
                            }
                        }
                    }
                }
                catch { }
            }



        }

        private void dis_More(object sender, RoutedEventArgs e)
        {
            SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Dispatch, ((FastOperate)((FastPanel)sender).DataContext).Contact, null));  
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

        //TcpInterface tcp = new TcpInterface();
        private void btn_Tool_Check_Click(object sender, RoutedEventArgs e)
        {
            //TcpInterface tcp = new TcpInterface();   
            if (StatusBar.Get().type == RunMode.Radio)
            {
                RadioOperate.GetStatus(1);
                RadioOperate.GetStatus(2);
            }
            else if (StatusBar.Get().type == RunMode.Repeater)
            {
                WirelanOperate.GetStatus(1);
            }  
        }

        private void btn_Tool_Monitor_Click(object sender, RoutedEventArgs e)
        {

        }

        private void lst_dispatch_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

        }

        private void btn_Tool_Position_Click(object sender, RoutedEventArgs e)
        {
            WorkArea.ClearPoint();
        }

        private void btn_Tool_Job_Click(object sender, RoutedEventArgs e)
        {
            
        }
        private void OnRadioStatus(string param)
        {
            RadioStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<RadioStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioStatusParam Error");
                return;
            }

            if (sta == null) return;
            if((sta.getType & (long)StatusType.ContectStatus) != 0)
            {
                try
                {
                    StatusBar.SetConectSta(int.Parse(sta.info.ToString()));
                }
                catch { }              
            }
            if ((sta.getType & (long)StatusType.RadioStatus) != 0)
            {
                try
                {
                    List<RadioStatus> infolist = JsonConvert.DeserializeObject<List<RadioStatus>>(JsonConvert.SerializeObject(sta.info));
                    foreach(RadioStatus rad in infolist)
                    {
                        if (rad.IsOnline) ResrcMgr.SetRadioOnline(rad.radioId, true);
                        if (rad.IsInGps) ResrcMgr.SetGpsOnline(rad.radioId, true);
                    }

                }
                catch { DataBase.InsertLog("Parse  RadioStatusParam Error"); }
            }
        }

        private void OnRadioCallStatus(string param)
        {
            RadioCallStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<RadioCallStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioStatusParam Error");
                return;
            }

            if (sta == null) return;
            if (sta.Operate == ExecType.Start)
            {
                CMember targt = new TargetSimple() { Type = sta.Type, ID = sta.Target }.ToMember();
                if (targt == null)
                {
                    TServer.IsInCalled = false;
                    return;
                }

                if (sta.Status == 0)
                {
                    //TODO:success
                    EventList.AddEvent("提示：开始呼叫" + targt.SimpleName + "(ID:" + sta.Target.ToString() + ")");
                    TServer.IsInCalled = true;
                }
                else if (sta.Status == 1)
                {
                    //TODO:connect failure
                    EventList.AddEvent("提示：呼叫" + targt.SimpleName + "(ID:" + sta.Target.ToString() + ")失败");


                    SubWindow.AddMessage(                     
                       RadioOperate.LastCall.Type == TargetType.All ? null : new TargetSimple() {Type= RadioOperate.LastCall.Type, ID = RadioOperate.LastCall.Target }.ToMember(), 
                       new CHistory()
                    {
                        istx = true,
                        type = NotifyType.Call,
                        time = DateTime.Now,
                        content = "呼叫失败"
                    });
                    
                    TServer.IsInCalled = false;

                }                
            }
            else if(sta.Operate == ExecType.Stop)
            {
                TServer.IsInCalled = false;
                EventList.AddEvent("提示：呼叫结束");

                SubWindow.AddMessage(
                    RadioOperate.LastCall.Type == TargetType.All ? null : new TargetSimple() { Type = RadioOperate.LastCall.Type, ID = RadioOperate.LastCall.Target }.ToMember(), 
                new CHistory()
                {
                    istx = true,
                    type = NotifyType.Call,
                    time = DateTime.Now,
                    content = "呼叫结束"
                });
            }

            if (TServer.IsInCalled) myTimer.Start();
            else { myTimer.Stop(); }

            try
            {
                ResrcMgr.SetTx(RadioOperate.LastCall.Type, RadioOperate.LastCall.Target, TServer.IsInCalled);
            }
            catch { }
        }


        private void OnRadioMessage(string param)
        {
            RadioSmsParam msg = null;
            try
            {
                msg = JsonConvert.DeserializeObject<RadioSmsParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioSmsParam Error");
                return;
            }

            if (msg == null) return;

            MsgWin.AddNotify(new CNotification()
            {
                Type = NotifyType.Message,
                Time = DateTime.Now,
                Source = ResrcMgr.Target.SimpleToMember(new TargetSimple() { Type = msg.Type, ID = msg.Source }).SingleToMult(),
                Content = new CMsgNotification() { Content = msg.Contents }
            });
        }

        private void OnRadioMessageStatus(string param)
        {
            RadioSmsStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<RadioSmsStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioSmsStatusParam Error");
                return;
            }

            if (sta == null) return;
            CMember targt = new TargetSimple() { Type = sta.Type, ID = sta.Target }.ToMember();
            if (targt == null)
            {
                return;
            }
            if(sta.Status == 0)
            {
                EventList.AddEvent("提示：发送短信成功（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")");
            }
            else if (sta.Status == 1)
            {
                EventList.AddEvent("提示：发送短信失败（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")");

                SubWindow.AddMessage(targt,
                new CHistory()
                {
                    istx = true,
                    type = NotifyType.Message,
                    time = DateTime.Now,
                    content = "短信发送失败"
                });
            }
        }

        private void OnControlStatus(string param)
        {
            RadioControlsStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<RadioControlsStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioControlsStatusParam Error");
                return;
            }

            if (sta == null) return;
            CMember targt = new TargetSimple() { Type = TargetType.Private, ID = sta.Target }.ToMember();
            if (targt == null)
            {
                return;
            }

            switch(sta.Type)
            {
                case ControlType.Check:
                    EventList.AddEvent("提示：在线检测（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")：" 
                        +(sta.Status == 2? "失败":(sta.Status == 0 ? "在线":"离线")));
                    if (sta.Status == 0 || sta.Status == 1)
                    {
                        ResrcMgr.SetRadioOnline(sta.Target, sta.Status == 0);
                        SubWindow.AddMessage(targt, new CHistory()
                       {
                           istx = false,
                           type = NotifyType.Control,
                           time = DateTime.Now,
                           content = "在线检测：" + (sta.Status == 0 ? "在线":"离线")
                      });
                    } 
                   else
                   {
                       SubWindow.AddMessage(targt, new CHistory()
                       {
                           istx = true,
                           type = NotifyType.Control,
                           time = DateTime.Now,
                           content = "在线检测失败"
                       });
                   }


                    break;
                case ControlType.Monitor:
                    EventList.AddEvent("提示：远程监听（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")" +(sta.Status == 0? "成功":"失败"));
                    if (sta.Status != 0)
                    SubWindow.AddMessage(targt, new CHistory()
                    {
                        istx = true,
                        type = NotifyType.Control,
                        time = DateTime.Now,
                        content = "远程监听失败"
                    });

                    break;
                case ControlType.StartUp:
                    EventList.AddEvent("提示：遥开（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")" +(sta.Status == 0? "成功":"失败"));
                    if (sta.Status != 0)
                        SubWindow.AddMessage(targt, new CHistory()
                        {
                            istx = true,
                            type = NotifyType.Control,
                            time = DateTime.Now,
                            content = "遥开失败"
                        });
                    break;
                case ControlType.ShutDown:
                    EventList.AddEvent("提示：遥毙（" + targt.SimpleName + "，ID:" + sta.Target.ToString() + ")" +(sta.Status == 0? "成功":"失败"));
                    if (sta.Status != 0)
                        SubWindow.AddMessage(targt, new CHistory()
                        {
                            istx = true,
                            type = NotifyType.Control,
                            time = DateTime.Now,
                            content = "遥毙失败"
                        });
                    break;
                case ControlType.Sleep:
                case ControlType.Week:
                default:
                    break;
            }
        }
       
        private void OnRadioArs(string param)
        {
            RadioArsParam ars = null;
            try
            {
                ars = JsonConvert.DeserializeObject<RadioArsParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioArsParam Error");
                return;
            }

            if (ars == null) return;
            CMember targt = new TargetSimple() { Type = TargetType.Private, ID = ars.Target }.ToMember();
            if (targt == null) return;

            ResrcMgr.SetRadioOnline(ars.Target, ars.ISOnline);
            EventList.AddEvent("ARS：" + targt.SimpleName + "（ID:" + ars.Target .ToString() + "）" + (ars.ISOnline ? "上线" : "下线"));

            SubWindow.AddMessage(targt, new CHistory()
            {
                istx = true,
                type = NotifyType.JobTicker,
                time = DateTime.Now,
                content = "ARS：" + (ars.ISOnline ? "上线" : "下线")
            });
        }

        private void OnRadioGpsStatus(string param)
        {
            RadioGpsStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<RadioGpsStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  RadioGpsStatusParam Error");
                return;
            }

            if (sta == null) return;

            CMember targt = new TargetSimple() { Type = TargetType.Private, ID = sta.Target }.ToMember();
            EventList.AddEvent("提示：申请查询对讲机(ID：" + targt.NameInfo+ ")位置信息" + (sta.Status == 0 ? "成功" : "失败"));

            if (sta.Status != 0)
                SubWindow.AddMessage(targt, new CHistory()
                {
                    istx = true,
                    type = NotifyType.Position,
                    time = DateTime.Now,
                    content = "申请查询对讲机位置信息失败"
                });
            
        }

        private void OnRadioGps(string param)
        {
            GPSParam gps = null;
            try
            {
                gps = JsonConvert.DeserializeObject<GPSParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  GPSParam Error");
                return;
            }

            if (gps == null) return;

            WorkArea.AddPoint(gps);
        }

        private List<WirelanCallParam> m_WirelanCallList = new List<WirelanCallParam>();

        private void OnWirelanCall(string param)
        {
            WirelanCallParam call = null;
            try
            {
                call = JsonConvert.DeserializeObject<WirelanCallParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  WirelanCallParam Error");
                return;
            }

            if (call == null) return;

            

            foreach(WirelanCallParam p in m_WirelanCallList)
            {
                if (p.type == call.type && p.source == call.source)
                {
                    if (call.operate == ExecType.Stop)
                    {
                        if (p.isCurrent) ResrcMgr.SetRx(p.type, p.source, false);

                        m_WirelanCallList.Remove(p);
                        break;
                    }                   
                }
            }

            var tmp = m_WirelanCallList.Where(p =>
                    p.type == call.type
                    && p.type == TargetType.All ? true : (p.type == TargetType.Private ? p.source == call.source : p.target == call.target));

            if (call.operate == ExecType.Start)
            {
                int count = 0;
                foreach (var item in tmp) count++;
                if (count <= 0)
                {
                    m_WirelanCallList.Add(call);

                    MsgWin.AddNotify(new CNotification()
                    {
                        Type = NotifyType.Call,
                        Time = DateTime.Now,
                        Source = call.type == TargetType.All ? new CMultMember() { Type = SelectionType.All } : (call.type ==  TargetType.Private ?
                         new TargetSimple() { Type = TargetType.Private, ID = call.source }.ToMember().SingleToMult()
                         : new TargetSimple() { Type = TargetType.Group, ID = call.target }.ToMember().SingleToMult()
                        ),
                        Content = new CRxNotification()
                    });
                }

                if (call.isCurrent) ResrcMgr.SetRx(call.type, call.type == TargetType.Group ? call.target : call.source, true);
            }
            else if (call.operate == ExecType.Stop)
            {
                foreach (var item in tmp) m_WirelanCallList.Remove(item);
                ResrcMgr.SetRx(call.type, call.type == TargetType.Group ? call.target : call.source, false);
            }

            this.Dispatcher.Invoke(new Action(() =>
            {
                Rx_RadioGroup.Children.Clear();
                foreach (WirelanCallParam p in m_WirelanCallList)
                {
                    RadioButton rad = new RadioButton()
                    {
                        IsChecked = p.isCurrent,                        
                        Height = 20,
                        Padding = new Thickness(2.5),
                        Margin = new Thickness(2.5),
                        Style = App.Current.Resources["RadioButtonStyleNav"] as Style
                    };

                    rad.Click += delegate(object sender, RoutedEventArgs e) { onCallStatusClick(sender, e); };

                    CMultMember mem = new CMultMember();
                    if (p.type == TargetType.All)
                    {
                        rad.Content = "全呼";
                        if (!p.isCurrent) rad.ToolTip = "单击选择全呼";

                        mem.Type = SelectionType.All;
                        rad.Tag = mem;
                    }
                    else if (p.type == TargetType.Group)
                    {
                        CMember src = new TargetSimple() { Type = TargetType.Group, ID = p.target }.ToMember();
                        rad.Content = src.Name;
                        if (!p.isCurrent) rad.ToolTip = "单击选择" + src.NameInfo;

                        mem.Type = SelectionType.Single;
                        mem.Target = new List<CMember>();
                        mem.Target.Add(src);

                        rad.Tag = mem;
                    }
                    else if (p.type == TargetType.Private)
                    {
                        CMember src = new TargetSimple() { Type = TargetType.Private, ID = p.source }.ToMember();
                        rad.Content = src.Name;
                        if (!p.isCurrent) rad.ToolTip = "单击选择" + src.NameInfo;

                        mem.Type = SelectionType.Single;
                        mem.Target = new List<CMember>();
                        mem.Target.Add(src);

                        rad.Tag = mem;
                    }
                    else
                    {

                    }

                    Rx_RadioGroup.Children.Add(rad);
                }
            }));
        }

        private void OnWirelanCallStatus(string param)
        {
            WirelanCallStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<WirelanCallStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  WirelanCallStatusParam Error");
                return;
            }

            if (sta == null) return;
            if (sta.operate == ExecType.Start)
            {
                CMember targt = new TargetSimple() { Type = sta.type, ID = sta.target}.ToMember();
                if (targt == null)
                {
                    TServer.IsInCalled = false;
                    return;
                }

                if (sta.status == 0)
                {
                    //TODO:success
                    try
                    {
                        EventList.AddEvent("提示：开始呼叫" + targt.SimpleName + "(ID:" + sta.target.ToString() + ")");
                        TServer.IsInCalled = true;

                    }
                    catch (Exception e)
                    {
                        DataBase.InsertLog(e.Message);
                    }
                }
                else if (sta.status == 1)
                {
                    //TODO:connect failure
                    try
                    {
                        EventList.AddEvent("提示：呼叫" + targt.SimpleName + "(ID:" + sta.target.ToString() + ")失败");
                        SubWindow.AddMessage(
                            WirelanOperate.LastCall.type == TargetType.All ? null : new TargetSimple() { Type = WirelanOperate.LastCall.type, ID = WirelanOperate.LastCall.target }.ToMember(),
                           new CHistory()
                           {
                               istx = true,
                               type = NotifyType.Call,
                               time = DateTime.Now,
                               content = "呼叫失败"
                           });
                    }
                    catch (Exception e)
                    {
                        DataBase.InsertLog(e.Message);
                    }

                    TServer.IsInCalled = false;

                }
            }
            else if (sta.operate == ExecType.Stop)
            {
                TServer.IsInCalled = false;              
                try
                {
                    EventList.AddEvent("提示：呼叫结束");
                    SubWindow.AddMessage(
                        WirelanOperate.LastCall.type == TargetType.All ? null : new TargetSimple() { Type = WirelanOperate.LastCall.type, ID = WirelanOperate.LastCall.target }.ToMember(),
                    new CHistory()
                    {
                        istx = true,
                        type = NotifyType.Call,
                        time = DateTime.Now,
                        content = "呼叫结束"
                    });
                }
                catch (Exception e)
                {
                    DataBase.InsertLog(e.Message);
                }
            }

            if (TServer.IsInCalled) myTimer.Start();
            else { myTimer.Stop(); }

            try
            {
                ResrcMgr.SetTx(WirelanOperate.LastCall.type, WirelanOperate.LastCall.target, TServer.IsInCalled);
            }
            catch { }

        }

        private void OnWirelanPlayStatus(string param)
        {
            WirelanPlayStatusParam sta = null;
            try
            {
                sta = JsonConvert.DeserializeObject<WirelanPlayStatusParam>(param);
            }
            catch
            {
                DataBase.InsertLog("Parse  WirelanPlayStatusParam Error");
                return;
            }

            if (sta == null) return;

            if (sta.status == 0)//success
            {
                this.Dispatcher.Invoke(new Action(() =>
                {
                   // foreach(RadioButton rad in Rx_RadioGroup.Children)
                    for (int i = 0; i < Rx_RadioGroup.Children.Count; i++)
                    {
                        try
                        {
                            CMultMember mem = (CMultMember)((RadioButton)Rx_RadioGroup.Children[i]).Tag;

                            if (mem.Type == SelectionType.All)
                            {
                                if (sta.target == -2)
                                {
                                    ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = true;
                                    ResrcMgr.SetRx(TargetType.All, -2, true);
                                }
                                else
                                {
                                    ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = false;
                                    ResrcMgr.SetRx(TargetType.All, -2, false);
                                }
                            }
                            else if (mem.Type != SelectionType.Null)
                            {
                                CMember m = mem.MultToSingle();
                                if (m.Type == MemberType.Group )
                                {
                                    if(m.Group.GroupID == sta.target)
                                    {
                                        ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = true;
                                        ResrcMgr.SetRx(TargetType.Group, m.Group.GroupID, true);
                                    }
                                    else{
                                        ResrcMgr.SetRx(TargetType.Group, m.Group.GroupID, false);
                                        ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = false;
                                    }
                                }
                                else if (sta.target == -1)
                                {
                                    ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = true;
                                    ResrcMgr.SetRx(TargetType.Private, m.Radio.RadioID, true);
                                }
                                else
                                {
                                    ResrcMgr.SetRx(TargetType.Private, m.Radio.RadioID, false);
                                    ((RadioButton)Rx_RadioGroup.Children[i]).IsChecked = false;
                                }
                            }
                        }
                        catch (Exception e)
                        { DataBase.InsertLog(e.Message); }
                    }
                }));
            }
            else
            {

            }           
        }

        private void onCallStatusClick(object sender, RoutedEventArgs e)
        {
            try
            {

                if (!(bool)((RadioButton)sender).IsChecked)
                {
                    ((RadioButton)sender).IsChecked = !((RadioButton)sender).IsChecked;
                    return;
                }

                
                CMultMember mem = (CMultMember)((RadioButton)sender).Tag;
               
                if (mem.Type == SelectionType.All) WirelanOperate.wlPlay(-2);
                else if (mem.Type != SelectionType.Null)
                {
                    CMember m = mem.MultToSingle();
                    if (m.Type == MemberType.Group) WirelanOperate.wlPlay(m.Group.GroupID);
                    else WirelanOperate.wlPlay(-1);
                }

            }
            catch (Exception res)
            { DataBase.InsertLog(res.Message); }
           
        }

    }
}
