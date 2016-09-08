using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

using System.Threading;

namespace TrboX
{
    public class MsgBox_t
    {
        public bool show { set; get; }
        public bool expanler { set; get; }
        public int rowindex { set; get; }
        public double height { set; get; }

        public MsgBox_t(bool s, bool e, int r, double h)
        {
            show = s;
            expanler = e;
            rowindex = r;
            height = h;
        }
    };

    public class CNotify
    {
        private Main m_Main;

        private BinaryFormatter m_BinFormat = new BinaryFormatter();//创建二进制序列化器
        private string m_NotifySavePath = "";

        public CNotify(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_NotifySavePath = AppDomain.CurrentDomain.BaseDirectory + "SRWYREGB34THEWTR2Q3WG4WUJ6JNWEG243G.tmp";

            Stream NotifySaveFile = new FileStream(m_NotifySavePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            NotifySaveFile.Position = 0;

            List<CNotification> Notify = new  List<CNotification>();
            try
            {
                Notify = (List<CNotification>)m_BinFormat.Deserialize(NotifySaveFile);
            }
            catch
            {
            };

            foreach (CNotification item in Notify)
            {
                switch(item.Type)
                {
                    case NotifyType.Alarm:
                        m_Main.lst_Alarm.Items.Add(item);
                        m_Main.View.MsgAlarmShow(true);
                        break;
                    case NotifyType.Call:
                         m_Main.lst_Rx.Items.Add(item);
                        m_Main.View.MsgRxShow(true);
                        break;
                    case NotifyType.Message:
                        m_Main.lst_ShortMsg.Items.Add(item);
                        m_Main.View.MsgShortMsgShow(true);
                        break;
                    case NotifyType.JobTicker:
                        m_Main.lst_Job.Items.Add(item);
                        m_Main.View.MsgJobShow(true);
                        break;
                    case NotifyType.Tracker:
                         m_Main.lst_Tracker.Items.Add(item);
                        m_Main.View.MsgTrackerShow(true);
                        break;
                }
            }

            m_Main.lst_Alarm.View = (ViewBase)m_Main.FindResource("AlarmView");
            m_Main.lst_ShortMsg.View = (ViewBase)m_Main.FindResource("MsgView");
            m_Main.lst_Rx.View = (ViewBase)m_Main.FindResource("RxView");
            m_Main.lst_Job.View = (ViewBase)m_Main.FindResource("JobView");
            m_Main.lst_Tracker.View = (ViewBase)m_Main.FindResource("TrackerView");

            m_Main.btn_ClearAlarmMsg.Click += delegate { Clear(NotifyType.Alarm); };
            m_Main.btn_ClearShortMsg.Click += delegate { Clear(NotifyType.Message); };
            m_Main.btn_ClearRxMsg.Click += delegate { Clear(NotifyType.Call); };
            m_Main.btn_ClearJobMsg.Click += delegate { Clear(NotifyType.JobTicker); };
            m_Main.btn_ClearTrackerMsg.Click += delegate { Clear(NotifyType.Tracker); };
        }

        public void Save()
        {
            Stream NotifySaveFile = new FileStream(m_NotifySavePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            NotifySaveFile.SetLength(0);
            m_BinFormat.Serialize(NotifySaveFile, Get());
        }


        public List<CNotification> Get()
        {
            List<CNotification> Notify = new List<CNotification>();
            
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                foreach (CNotification item in m_Main.lst_Alarm.Items) Notify.Add(item);
                foreach (CNotification item in m_Main.lst_Rx.Items) Notify.Add(item);
                foreach (CNotification item in m_Main.lst_ShortMsg.Items) Notify.Add(item);
                foreach (CNotification item in m_Main.lst_Job.Items) Notify.Add(item);
                foreach (CNotification item in m_Main.lst_Tracker.Items) Notify.Add(item);
            }));
            return Notify;
        }

        public void Add(CNotification notify)
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                switch (notify.Type)
                {
                    case NotifyType.Alarm:
                        m_Main.lst_Alarm.Items.Insert(0, notify);
                        m_Main.View.MsgAlarmShow(true);
                        break;
                    case NotifyType.Call:
                        m_Main.lst_Rx.Items.Insert(0, notify);
                        m_Main.View.MsgRxShow(true);
                        break;
                    case NotifyType.Message:
                        m_Main.lst_ShortMsg.Items.Insert(0, notify);
                        m_Main.View.MsgShortMsgShow(true);
                        break;
                    case NotifyType.JobTicker:
                        m_Main.lst_Job.Items.Insert(0, notify);
                        m_Main.View.MsgJobShow(true);
                        break;
                    case NotifyType.Tracker:
                        m_Main.lst_Tracker.Items.Insert(0, notify);
                        m_Main.View.MsgTrackerShow(true);
                        break;
                }

                m_Main.g_IsNeedSaveWorkSpace = true;
                m_Main.SubWindow.AddNotifyToOperateWin(notify);
            }));
        }

        public void Remove(CNotification notify)
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                switch (notify.Type)
                {
                    case NotifyType.Alarm:
                        m_Main.lst_Alarm.Items.Remove(notify);
                        if (0 == m_Main.lst_Alarm.Items.Count) m_Main.View.MsgAlarmShow(false);
                        break;
                    case NotifyType.Call:
                        m_Main.lst_Rx.Items.Remove(notify);
                        if (0 == m_Main.lst_Rx.Items.Count) m_Main.View.MsgRxShow(false);
                        break;
                    case NotifyType.Message:
                        m_Main.lst_ShortMsg.Items.Remove(notify);
                        if (0 == m_Main.lst_ShortMsg.Items.Count) m_Main.View.MsgShortMsgShow(false);
                        break;
                    case NotifyType.JobTicker:
                        m_Main.lst_Job.Items.Remove(notify);
                        if (0 == m_Main.lst_Job.Items.Count) m_Main.View.MsgJobShow(false);
                        break;
                    case NotifyType.Tracker:
                        m_Main.lst_Tracker.Items.Remove(notify);
                        if (0 == m_Main.lst_Tracker.Items.Count) m_Main.View.MsgTrackerShow(false);
                        break;
                }

                m_Main.g_IsNeedSaveWorkSpace = true;
            }));
        }

        public void Clear(NotifyType type)
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                switch (type)
                {
                    case NotifyType.Alarm:
                        m_Main.lst_Alarm.Items.Clear();
                        m_Main.View.MsgAlarmShow(false);
                        break;
                    case NotifyType.Call:
                        m_Main.lst_Rx.Items.Clear();
                        m_Main.View.MsgRxShow(false);
                        break;
                    case NotifyType.Message:
                        m_Main.lst_ShortMsg.Items.Clear();
                       m_Main.View.MsgShortMsgShow(false);
                        break;
                    case NotifyType.JobTicker:
                        m_Main.lst_Job.Items.Clear();
                        m_Main.View.MsgJobShow(false);
                        break;
                    case NotifyType.Tracker:
                        m_Main.lst_Tracker.Items.Clear();
                        m_Main.View.MsgTrackerShow(false);
                        break;
                }

                m_Main.g_IsNeedSaveWorkSpace = true;
            }));
        }

        public void RemoveNotifyBySrc(CRelationShipObj src)
        {
            List<CNotification> notify = Get();
            List<CNotification> willdel = new List<CNotification>();
            foreach (CNotification item in notify)
                if (true == CRelationShipObj.Compare(item.Source, src)) willdel.Add(item);

            foreach (CNotification item in willdel) Remove(item);
        }

    }

    public class MainMsgWin
    {
       private Main m_Main;

       private Dictionary<Border, MsgBox_t> m_bdrMsgList = new Dictionary<Border, MsgBox_t>();
       private CNotify m_Notify;
       public MainMsgWin(Main win)
       {
            if (null == win) return;
            m_Main = win;
       }

        public void Initialize()
        {
            m_bdrMsgList.Add(m_Main.bdr_Msg_Alarm, new MsgBox_t(true, true, 0, m_Main.grd_Msg.RowDefinitions[0].ActualHeight));
            m_bdrMsgList.Add(m_Main.bdr_Msg_ShortMsg, new MsgBox_t(true, false, 1, m_Main.grd_Msg.RowDefinitions[1].ActualHeight));
            m_bdrMsgList.Add(m_Main.bdr_Msg_Rx, new MsgBox_t(true, false, 2, m_Main.grd_Msg.RowDefinitions[2].ActualHeight));
            m_bdrMsgList.Add(m_Main.bdr_Msg_Job, new MsgBox_t(true, false, 3, m_Main.grd_Msg.RowDefinitions[3].ActualHeight));
            m_bdrMsgList.Add(m_Main.bdr_Msg_Tracker, new MsgBox_t(true, false, 4, m_Main.grd_Msg.RowDefinitions[4].ActualHeight));

            MsgWinExplanerRegister();

            m_Main.View.MsgAlarmShow(false); 
            m_Main.View.MsgShortMsgShow(false);
            m_Main.View.MsgRxShow(false); 
            m_Main.View.MsgJobShow(false); 
            m_Main.View.MsgTrackerShow(false); 

            //receive notify
            m_Notify = new CNotify(m_Main);

            Thread t = new Thread(() => { NoficationThread(); });
            t.Start(); 
        }


       private double MsgWidth = 260;
       private void UpdateMsgWinSta()
       {
           int show_count = 0;
           int the_last_explaner_index = -1;
           bool is_need_update_hight = false;
           bool is_first_show = true;
           bool is_last_explaner = false;


           GridSplitter[] message_grid_spl_list = new GridSplitter[4] { m_Main.grdspl_Msg_Row0, m_Main.grdspl_Msg_Row1, m_Main.grdspl_Msg_Row2, m_Main.grdspl_Msg_Row3 };

           foreach (var item in m_bdrMsgList)
           {
               m_bdrMsgList[item.Key].height = m_Main.grd_Msg.RowDefinitions[item.Value.rowindex].ActualHeight;

               if ((true == item.Value.show) && (true == item.Value.expanler) && (31 >= m_Main.grd_Msg.RowDefinitions[item.Value.rowindex].ActualHeight))
               {
                   is_need_update_hight = true;
               }
           }


           foreach (var item in m_bdrMsgList)
           {
               if (true == item.Value.show)
               {
                   item.Key.SetValue(Grid.RowProperty, show_count);

                   if (true == is_first_show)
                   {
                       m_Main.grd_Msg.RowDefinitions[show_count].MinHeight = 31;

                       item.Key.BorderThickness = new Thickness(1);
                   }
                   else
                   {
                       item.Key.BorderThickness = new Thickness(1, 0, 1, 1);
                   }

                   if (true == item.Value.expanler)
                   {
                       m_Main.grd_Msg.RowDefinitions[show_count].MinHeight = 60;

                       if (true == is_need_update_hight)
                       {
                           m_Main.grd_Msg.RowDefinitions[show_count].Height = new GridLength(1, GridUnitType.Star);
                       }
                       else
                       {
                           m_Main.grd_Msg.RowDefinitions[show_count].Height = new GridLength(item.Value.height);
                       }
                       the_last_explaner_index = show_count;

                       if (true == is_last_explaner)
                       {
                           if (1 <= show_count) message_grid_spl_list[show_count - 1].Visibility = Visibility.Visible;
                       }
                       else
                       {
                           if (1 <= show_count) message_grid_spl_list[show_count - 1].Visibility = Visibility.Hidden;
                       }

                       is_last_explaner = true;
                   }
                   else
                   {
                       if (true == is_first_show)
                       {
                           m_Main.grd_Msg.RowDefinitions[show_count].MinHeight = 32;
                           m_Main.grd_Msg.RowDefinitions[show_count].Height = new GridLength(32);
                       }
                       else
                       {
                           m_Main.grd_Msg.RowDefinitions[show_count].MinHeight = 31;
                           m_Main.grd_Msg.RowDefinitions[show_count].Height = new GridLength(31);
                       }

                       if (1 <= show_count) message_grid_spl_list[show_count - 1].Visibility = Visibility.Hidden;
                       is_last_explaner = false;
                   }

                   if (true == is_first_show) is_first_show = false;

                   m_bdrMsgList[item.Key].rowindex = show_count;
                   show_count++;
               }
               else
               {
                   item.Key.SetValue(Grid.RowProperty, 5);
               }
           }

           if (the_last_explaner_index >= 0) m_Main.grd_Msg.RowDefinitions[the_last_explaner_index].Height = new GridLength(1, GridUnitType.Star);

           for (int i = show_count; i < 6; i++)
           {
               m_Main.grd_Msg.RowDefinitions[show_count].MinHeight = 0;
               m_Main.grd_Msg.RowDefinitions[show_count].Height = new GridLength(0);
           }


           if ((false == m_Main.menu_View_Msg_Alarm.IsChecked)
              && (false == m_Main.menu_View_Msg_ShortMsg.IsChecked)
              && (false == m_Main.menu_View_Msg_Rx.IsChecked)
              && (false == m_Main.menu_View_Msg_Job.IsChecked)
              && (false == m_Main.menu_View_Msg_Tracker.IsChecked))
           {
               //hide manager win
               m_Main.grd_main.ColumnDefinitions[2].MinWidth = 0;
               MsgWidth = m_Main.grd_main.ColumnDefinitions[2].ActualWidth;
               m_Main.grd_main.ColumnDefinitions[2].Width = new GridLength(0);

               m_Main.grd_main.ColumnDefinitions[1].Width = new GridLength(1, GridUnitType.Star);
               m_Main.grd_main.ColumnDefinitions[1].MaxWidth = m_Main.grd_main.ActualWidth - m_Main.grd_main.ColumnDefinitions[0].MinWidth;

               m_Main.grdspl_Main_Msg.Visibility = Visibility.Hidden;
           }
           else if(0 == m_Main.grd_main.ColumnDefinitions[2].MinWidth)
           {
               m_Main.grd_main.ColumnDefinitions[2].MinWidth = 100;
               m_Main.grd_main.ColumnDefinitions[2].Width = new GridLength(MsgWidth);

               m_Main.grdspl_Main_Msg.Visibility = Visibility.Visible;
           }
       }


       private void MsgWinExplanerRegister()
       {
           m_Main.chk_MsgExp_Alarm.Checked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Alarm].expanler = true; UpdateMsgWinSta();};
           m_Main.chk_MsgExp_Alarm.Unchecked += delegate{ m_bdrMsgList[m_Main.bdr_Msg_Alarm].expanler = false; UpdateMsgWinSta();};

           m_Main.chk_MsgExp_ShortMsg.Checked += delegate { m_bdrMsgList[m_Main.bdr_Msg_ShortMsg].expanler = true; UpdateMsgWinSta(); };
           m_Main.chk_MsgExp_ShortMsg.Unchecked += delegate{ m_bdrMsgList[m_Main.bdr_Msg_ShortMsg].expanler = false;UpdateMsgWinSta();};

           m_Main.chk_MsgExp_Rx.Checked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Rx].expanler = true;UpdateMsgWinSta();};
           m_Main.chk_MsgExp_Rx.Unchecked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Rx].expanler = false; UpdateMsgWinSta(); };

           m_Main.chk_MsgExp_Job.Checked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Job].expanler = true;UpdateMsgWinSta();};
           m_Main.chk_MsgExp_Job.Unchecked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Job].expanler = false;UpdateMsgWinSta();};

           m_Main.chk_MsgExp_Tracker.Checked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Tracker].expanler = true;UpdateMsgWinSta();};
           m_Main.chk_MsgExp_Tracker.Unchecked += delegate{m_bdrMsgList[m_Main.bdr_Msg_Tracker].expanler = false;UpdateMsgWinSta();};
       }

        public void MsgAlarmShow(bool hide = false)
        {
            if(hide)
            {
                m_bdrMsgList[m_Main.bdr_Msg_Alarm].show = false;
            }
            else
            {
                m_bdrMsgList[m_Main.bdr_Msg_Alarm].show = true;
                m_bdrMsgList[m_Main.bdr_Msg_Alarm].expanler = true;
                m_Main.chk_MsgExp_Alarm.IsChecked = true;
            }

            UpdateMsgWinSta();
        }

        public void MsgShortMsgShow(bool hide = false)
        {
            if(hide)
            {
                m_bdrMsgList[m_Main.bdr_Msg_ShortMsg].show = false;
            }
            else
            {
                m_bdrMsgList[m_Main.bdr_Msg_ShortMsg].show = true;
                m_bdrMsgList[m_Main.bdr_Msg_ShortMsg].expanler = true;
                m_Main.chk_MsgExp_ShortMsg.IsChecked = true;
            }
            UpdateMsgWinSta();
        }

         public void MsgRxShow(bool hide = false)
        {
            if(hide)
            {
                m_bdrMsgList[m_Main.bdr_Msg_Rx].show = false;
            }
            else
            {
                m_bdrMsgList[m_Main.bdr_Msg_Rx].show = true;
                m_bdrMsgList[m_Main.bdr_Msg_Rx].expanler = true;
                m_Main.chk_MsgExp_Rx.IsChecked = true;
            }
            UpdateMsgWinSta();
        }


        public void MsgJobShow(bool hide = false)
        {
            if(hide)
            {
                m_bdrMsgList[m_Main.bdr_Msg_Job].show = false;
            }
            else
            {
                m_bdrMsgList[m_Main.bdr_Msg_Job].show = true;
                m_bdrMsgList[m_Main.bdr_Msg_Job].expanler = true;
                m_Main.chk_MsgExp_Job.IsChecked = true;
            }

            UpdateMsgWinSta();
        }


        public void MsgTrackerShow(bool hide = false)
        {
            if(hide)
            {
                m_bdrMsgList[m_Main.bdr_Msg_Tracker].show = false;
            }
            else
            {
                m_bdrMsgList[m_Main.bdr_Msg_Tracker].show = true;
                m_bdrMsgList[m_Main.bdr_Msg_Tracker].expanler = true;
                m_Main.chk_MsgExp_Tracker.IsChecked = true;
            }
             UpdateMsgWinSta();
        }

        public void AddNotify(CNotification notify)
        {
            m_Notify.Add(notify);
        }
        public List<CNotification> GetNotify(bool asc)
        {
            List<CNotification> notify = m_Notify.Get();
            if (true == asc) notify.Sort((x, y) => x.Time.CompareTo(y.Time));
            else notify.Sort((x, y) => -x.Time.CompareTo(y.Time)); 

            return notify;
        }

        public void ClearTarget(CRelationShipObj target)
        {
            m_Notify.RemoveNotifyBySrc(target);
        }
        public void RemoveNotify(object item)
        {
            m_Notify.Remove(item as CNotification);
        }

        public void ClearNotify(NotifyType type)
        {
            m_Notify.Clear(type);
        }
        public void SaveNotify()
        {
            m_Notify.Save();
        }

        private void NoficationThread()
        {
            int i = 0;
            while (true)
            {
                i++;

                AddNotify(new CNotification() { Type = NotifyType.Alarm, Time = DateTime.Now, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 2 }), Content = new CAlarmNotification() { Content = "test" + i.ToString() } });
                AddNotify(new CNotification() { Type = NotifyType.Alarm, Time = DateTime.Now, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 3 }), Content = new CAlarmNotification() { Content = "Alarm" + i.ToString() } });

               //CAlarmNotification alarm = new CAlarmNotification() {Source= new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio(){id = 2}), Content = "test" + i.ToString(), time = DateTime.Now };
               //m_Notify.Add(alarm);


               //alarm = new CAlarmNotification() { Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 3 }), Content = "test" + i.ToString(), time = DateTime.Now };
               //m_Notify.Add(alarm);

                m_Notify.Add(new CNotification() { Type = NotifyType.Call, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 2 }), Time = DateTime.Now });
                m_Notify.Add(new CNotification() { Type = NotifyType.Message, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 2 }), Time = DateTime.Now, Content = new CMsgNotification() { Content = "Message" + i.ToString() } });
                m_Notify.Add(new CNotification() { Type = NotifyType.JobTicker, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 2 }), Time = DateTime.Now });
                m_Notify.Add(new CNotification() { Type = NotifyType.Tracker, Source = new CRelationShipObj(OrgItemType.Type_Radio, null, null, null, new CRadio() { id = 2 }), Time = DateTime.Now });


                

                //m_Main.EventList.AddEvent("事件" + i.ToString());

                Thread.Sleep(5000);
            }
        }
    }
}
