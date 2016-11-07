using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

using System.Windows.Automation.Peers;

namespace TrboX
{

    public  class EventWin
    {        
        private Main m_Main;
        public EventWin(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_Main.lst_Event.View = (ViewBase)m_Main.FindResource("EventView");
        }

        public void AddEvent(string content)
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                while (m_Main.lst_Event.Items.Count >= 50)
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
                    Content = content,
                    Time = DateTime.Now
                });

                ListViewAutomationPeer lvap = new ListViewAutomationPeer(m_Main.lst_Event);
                var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
                ((ScrollViewer)svap.Owner).ScrollToEnd();

            }));

            DataBase.InsertLog(content);
        }

    }
}
