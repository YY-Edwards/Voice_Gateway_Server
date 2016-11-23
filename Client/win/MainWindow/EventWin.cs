using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

using System.Windows.Automation.Peers;

using System.Threading;

namespace TrboX
{

    public  class EventWin
    {        
        private Main m_Main;
        private Queue<string> eventque = new Queue<string>();
        public EventWin(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_Main.lst_Event.View = (ViewBase)m_Main.FindResource("EventView");

            new Thread(new ThreadStart(delegate() { UpdateEventThread(); })).Start();
        }


        private void UpdateEventThread()
        {
            while(true)
            {
                try{
                    lock(eventque)
                    {
                         if(eventque.Count > 0)
                         {
                            while(eventque.Count > 0)
                            {                     
                                string contents = eventque.Dequeue();

                                 m_Main.Dispatcher.Invoke(new Action(() =>
                                 {
                                    while (m_Main.lst_Event.Items.Count >= 100)
                                    {
                                        try
                                        {
                                            m_Main.lst_Event.Items.RemoveAt(0);
                                        }
                                        catch
                                        {
                                        }
                                    }

                                    m_Main.lst_Event.Items.Add(new CEvent()
                                    {
                                        Content = contents,
                                        Time = DateTime.Now
                                    });
                                }));

                                DataBase.InsertLog(contents);
                            }

                            m_Main.Dispatcher.Invoke(new Action(() =>{
                            ListViewAutomationPeer lvap = new ListViewAutomationPeer(m_Main.lst_Event);
                            var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                            ((ScrollViewer)svap.Owner).ScrollToEnd();}));
                        }
                    }    
                }
                catch
                {
                    DataBase.InsertLog("Add Event Error:");
                }

                Thread.Sleep(1000);
            }
        }


        public void AddEvent(string content)
        {
            try{
             new Thread(new ThreadStart(delegate() {
                  lock(eventque)
                  {
                      eventque.Enqueue(content);
                  }
             })).Start();
            }
            catch
            {
                DataBase.InsertLog("Add Event Error:");
            }
            //DataBase.InsertLog("Add Event:");
        }

    }
}
