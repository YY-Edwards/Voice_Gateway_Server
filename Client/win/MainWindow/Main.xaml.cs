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
                TServer.InitializeTServer();  
     
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

                new Thread(new ThreadStart(delegate() {
                    ReceiveMsghread();                            
                })).Start();

                //ResrcMgr.SetRadioOnline(9, true);
                //ResrcMgr.SetRadioOnline(10, true);
                //ResrcMgr.SetRadioOnline(9, false);
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


        private void ReceiveMsghread()
        { 
            while(true)
            {
                lock (TServer.RxRequest)
                { 
                    if(TServer.RxRequest.Count > 0)
                    {
                        TServerRequest req = TServer.RxRequest.Dequeue();
                        if (req.call.ToUpper() == RequestType.message.ToString().ToUpper())
                        {
                                MsgWin.AddNotify(new CNotification(){
                                  Type=  NotifyType.Message,
                                  Time = DateTime.Now,
                                  Source = ResrcMgr.Target.SimpleToMember(new TargetSimple() { Type = ((RadioSmsParam)req.param).Type, ID = ((RadioSmsParam)req.param).Source }),
                                  Content = new CMsgNotification() { Content = ((RadioSmsParam)req.param).Contents }
                                  });            
                        }
                        else if (req.call.ToUpper() == RequestType.sendArs.ToString().ToUpper())                         
                        {
                                ResrcMgr.SetRadioOnline(((RadioArsParam)req.param).Target, ((RadioArsParam)req.param).ISOnline);
                                if(((RadioArsParam)req.param).ISOnline)EventList.AddEvent("ARS：对讲机（ID:" + ((RadioArsParam)req.param).Target + "）上线");
                                else EventList.AddEvent("ARS：对讲机（ID:" + ((RadioArsParam)req.param).Target + "）下线");
                        }
                        else if (req.call.ToUpper() == RequestType.sendGps.ToString().ToUpper())
                        {
                            
                        }                       
                    }
                }
                Thread.Sleep(200);
            }
        }
        private void RegRxHandler()
        {
            TServer.RegRxHanddler(RequestType.message, OnRadioMessage);
            TServer.RegRxHanddler(RequestType.sendArs, OnRadioArs);
            TServer.RegRxHanddler(RequestType.sendGps, OnRadioGps);

            TServer.RegRxHanddler(RequestType.wlCall, OnWirelanCall);
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
            if (null != operate) operate.Exec();            
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
           
        }

        private void btn_Tool_Monitor_Click(object sender, RoutedEventArgs e)
        {
           // tcp.TCPTST();
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


        private void OnRadioMessage(string param)
        {
            RadioSmsParam msg = null;
            try
            {
                msg = JsonConvert.DeserializeObject<RadioSmsParam>(param);
            }
            catch
            {
                Console.Write("Parse  RadioSmsParam Error");
                return;
            }

            if (msg == null) return;

            MsgWin.AddNotify(new CNotification()
            {
                Type = NotifyType.Message,
                Time = DateTime.Now,
                Source = ResrcMgr.Target.SimpleToMember(new TargetSimple() { Type = msg.Type, ID = msg.Source }),
                Content = new CMsgNotification() { Content = msg.Contents }
            });
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
                Console.Write("Parse  RadioArsParam Error");
                return;
            }

            if (ars == null) return;

            ResrcMgr.SetRadioOnline(ars.Target, ars.ISOnline);
            EventList.AddEvent("ARS：对讲机（ID:" + ars.Target.ToString() + "）" + (ars.ISOnline ? "上线" : "下线"));
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
                Console.Write("Parse  GPSParam Error");
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
                Console.Write("Parse  WirelanCallParam Error");
                return;
            }

            if (call == null) return;


            foreach(WirelanCallParam p in m_WirelanCallList)
            {
                if(p.type == call.type && p.target == call.target)
                {
                    if (call.operate == ExecType.Stop)
                    {
                        m_WirelanCallList.Remove(p);
                    }                   
                }
                else{
                    if(call.operate == ExecType.Start)m_WirelanCallList.Add(call);
                }
            }

            foreach (WirelanCallParam p in m_WirelanCallList)
            {
                RadioButton rad = new RadioButton() { IsChecked = p.isCurrent, Content =new TargetSimple(){Type =p.type,ID=p.target }.ToMember().Name, GroupName = "rxgroup", Height = 20, Padding = new Thickness(2.5), Margin = new Thickness(2.5), Style = App.Current.Resources["RadioButtonStyleNav"] as Style, Tag =  p };
                rad.Checked += delegate(object sender, RoutedEventArgs e) { onCallStatusCheck(sender, e); };
                rad.Unchecked += delegate(object sender, RoutedEventArgs e) { onCallStatusUncheck(sender, e); };
                Rx_RadioGroup.Children.Add(rad);
            }
        }

        private void onCallStatusCheck(object sender, RoutedEventArgs e)
        {
            WirelanCallParam call = (WirelanCallParam)((RadioButton)sender).Tag;
            call.isCurrent = true;

            foreach (WirelanCallParam p in m_WirelanCallList)
            {
                if (p.type == call.type && p.target == call.target)
                {
                    if (call.operate == ExecType.Stop)
                    {
                        m_WirelanCallList.Remove(p);
                    }
                }
            }

            m_WirelanCallList.Add(call);
            if (call.type == TargetType.Group) WirelanOperate.wlPlay(call.target);
            else if (call.type == TargetType.Private) WirelanOperate.wlPlay(-1);
            else if (call.type == TargetType.All) WirelanOperate.wlPlay(-2);
           
        }
        private void onCallStatusUncheck(object sender, RoutedEventArgs e)
        {
            WirelanCallParam call = (WirelanCallParam)((RadioButton)sender).Tag;
            call.isCurrent = false;

            foreach (WirelanCallParam p in m_WirelanCallList)
            {
                if (p.type == call.type && p.target == call.target)
                {
                    if (call.operate == ExecType.Stop)
                    {
                        m_WirelanCallList.Remove(p);
                    }
                }
            }

            m_WirelanCallList.Add(call);
        }
    }
}
