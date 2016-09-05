using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Threading;

using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;


namespace TrboX
{

    public class CNotify
    {
        [Serializable]
        private struct Notify_t
        {
            public List<CAlarmNotification> alarm;
            public List<CRxNotification> rx;
            public List<CMsgNotification> message;
            public List<CJobTicketNotification> job;
            public List<CTackerNotification> tacker;
        };

        private Main m_Win;

        private BinaryFormatter m_BinFormat = new BinaryFormatter();//创建二进制序列化器
        private string m_NotifySavePath = "";

        public CNotify(Main win)
        {
            if (null == win) return;
            m_Win = win;

            m_NotifySavePath = AppDomain.CurrentDomain.BaseDirectory + "SRWYREGB34THEWTR2Q3WG4WUJ6JNWEG243G.TMP";

            Stream NotifySaveFile = new FileStream(m_NotifySavePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            NotifySaveFile.Position = 0;

            Notify_t Notify = new Notify_t();
            try
            {
                Notify = (Notify_t)m_BinFormat.Deserialize(NotifySaveFile);           
            }
            catch 
            {
            };

            if (null != Notify.alarm)
            foreach (CAlarmNotification item in Notify.alarm)
            {
                m_Win.lst_Alarm.Items.Add(item);
            }

            if (null != Notify.message)
                foreach (CMsgNotification item in Notify.message)
                {
                    m_Win.lst_ShortMsg.Items.Add(item);
                }

            if (null != Notify.rx)
                foreach (CRxNotification item in Notify.rx)
                {
                    m_Win.lst_Rx.Items.Add(item);
                }

            if (null != Notify.job)
                foreach (CJobTicketNotification item in Notify.job)
                {
                    m_Win.lst_Job.Items.Add(item);
                }

            if (null != Notify.tacker)
                foreach (CTackerNotification item in Notify.tacker)
                {
                    m_Win.lst_Tracker.Items.Add(item);
                }


            m_Win.lst_Alarm.View = (ViewBase)m_Win.FindResource("AlarmView");
            m_Win.lst_ShortMsg.View = (ViewBase)m_Win.FindResource("MsgView");
            m_Win.lst_Rx.View = (ViewBase)m_Win.FindResource("RxView");
            m_Win.lst_Job.View = (ViewBase)m_Win.FindResource("JobView");
            m_Win.lst_Tracker.View = (ViewBase)m_Win.FindResource("TrackerView");

            m_Win.btn_ClearAlarmMsg.Click += delegate
            {
                ClearAlarm();
            };
            m_Win.btn_ClearShortMsg.Click += delegate
            {
                ClearShortMsg();
            };
            m_Win.btn_ClearRxMsg.Click += delegate
            {
                ClearRx();
            };
            m_Win.btn_ClearJobMsg.Click += delegate
            {
                ClearJob();
            };
            m_Win.btn_ClearTrackerMsg.Click += delegate
            {
                ClearTracker();
            };

        }

        public void Save()
        {
            Notify_t Notify = new Notify_t();
            Notify.alarm = new List<CAlarmNotification>();
            Notify.rx = new List<CRxNotification>();
            Notify.message = new List<CMsgNotification>();
            Notify.job = new List<CJobTicketNotification>();
            Notify.tacker = new List<CTackerNotification>();

            m_Win.Dispatcher.Invoke(new Action(() =>{
                foreach (CAlarmNotification item in m_Win.lst_Alarm.Items)
                {
                    Notify.alarm.Add(item);
                }

                foreach (CRxNotification item in m_Win.lst_Rx.Items)
                {
                    Notify.rx.Add(item);
                }

                foreach (CMsgNotification item in m_Win.lst_ShortMsg.Items)
                {
                    Notify.message.Add(item);
                }

                foreach (CJobTicketNotification item in m_Win.lst_Job.Items)
                {
                    Notify.job.Add(item);
                }

                foreach (CTackerNotification item in m_Win.lst_Tracker.Items)
                {
                    Notify.tacker.Add(item);
                }

            }));

            Stream NotifySaveFile = new FileStream(m_NotifySavePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            NotifySaveFile.SetLength(0);
            m_BinFormat.Serialize(NotifySaveFile, Notify);
        }


        public void Add(CAlarmNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Alarm.Items.Insert(0, item);
            }));
        }

        public void Remove(CAlarmNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Alarm.Items.Remove(item);
            }));
        }

        public void ClearAlarm()
        {
            m_Win.lst_Alarm.Items.Clear();
        }

        public void Add(CMsgNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_ShortMsg.Items.Insert(0, item);
            }));
        }

        public void Remove(CMsgNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_ShortMsg.Items.Remove(item);
            }));
        }

        public void ClearShortMsg()
        {
            m_Win.lst_ShortMsg.Items.Clear();
        }

        public void Add(CRxNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Rx.Items.Insert(0, item);
            }));
        }

        public void Remove(CRxNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Rx.Items.Remove(item);
            }));
        }

        public void ClearRx()
        {
            m_Win.lst_Rx.Items.Clear();
        }

        public void Add(CJobTicketNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Job.Items.Insert(0, item);
            }));
        }

        public void Remove(CJobTicketNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Job.Items.Remove(item);
            }));
        }

        public void ClearJob()
        {
            m_Win.lst_Job.Items.Clear();
        }

        public void Add(CTackerNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Tracker.Items.Insert(0, item);
            }));
        }

        public void Remove(CTackerNotification item)
        {
            m_Win.Dispatcher.Invoke(new Action(() =>
            {
                m_Win.lst_Tracker.Items.Remove(item);
            }));
        }

        public void ClearTracker()
        {
            m_Win.lst_Tracker.Items.Clear();
        }

    }

    public class NotifyView
    {
        Main m_Win;

        public CNotify m_Notify;

        public NotifyView(Main win)
        {
            if (null == win) return;               
            m_Win = win;
            m_Notify = new CNotify(m_Win);

            Thread t = new Thread(()=>{ MsgViewNofication();});
            t.Start(); 
        }

        private void MsgViewNofication()
        {
            int i= 0 ;
            while(true)
            {
                i++;
                 //m_Notify.Add(new CAlarmNotification() { Content = "test" + i.ToString(), time = DateTime.Now });
                 //m_Notify.Add(new CMsgNotification() { Content = "test" + i.ToString(), time = DateTime.Now });
                 //m_Notify.Add(new CRxNotification() { time = DateTime.Now });
                 //m_Notify.Add(new CJobTicketNotification() {  time = DateTime.Now });
                 //m_Notify.Add(new CTackerNotification() {  time = DateTime.Now });


                 //m_Win.m_View.AddEvent("事件" + i.ToString());

                Thread.Sleep(2000);
            }
        }
    }
}
