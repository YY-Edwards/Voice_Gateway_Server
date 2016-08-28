using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace TrboX
{
    public class MsgBox_t
    {
        public MsgBox_t(bool s, bool e,int r,  double h)
        {
            show  = s;
            expanler = e;
            rowindex = r;
            height = h;
        }

        public bool show { set; get; }
        public bool expanler { set; get; }
        public int rowindex { set; get; }
        public double height { set; get; }
    };
    class MainView
    {
        private Main m_mainWin;

        private Dictionary<Border, MsgBox_t> m_bdrMsgList = new Dictionary<Border, MsgBox_t>();
        private GridLength MgrRowLength
            , EventRowLength
            , MsgAlarmLength
            , MsgShortMsgLength
            , MsgRxLength
            , MsgJobLength
            , MsgTrackerLength;

        public MainView(Main win)
        {
            if (null == win) return;
                
            m_mainWin = win;
            menu_View_Tool();

            MgrRowLength = m_mainWin.grd_Row_Mgr.Height;
            menu_View_Manager();

            menu_View_Nav();

            EventRowLength = m_mainWin.grd_Row_Event.Height;
            menu_View_Event();

            MsgAlarmLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            MsgShortMsgLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            MsgRxLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            MsgJobLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            MsgTrackerLength = m_mainWin.grd_Row_Msg_Alarm.Height;

            m_bdrMsgList.Add(m_mainWin.bdr_Msg_Alarm, new MsgBox_t(true, true,0, m_mainWin.grd_Msg.RowDefinitions[0].ActualHeight));
            m_bdrMsgList.Add(m_mainWin.bdr_Msg_ShortMsg, new MsgBox_t(true, false,1, m_mainWin.grd_Msg.RowDefinitions[1].ActualHeight));
            m_bdrMsgList.Add(m_mainWin.bdr_Msg_Rx, new MsgBox_t(true, false,2, m_mainWin.grd_Msg.RowDefinitions[2].ActualHeight));
            m_bdrMsgList.Add(m_mainWin.bdr_Msg_Job, new MsgBox_t(true, false,3, m_mainWin.grd_Msg.RowDefinitions[3].ActualHeight));
            m_bdrMsgList.Add(m_mainWin.bdr_Msg_Tracker, new MsgBox_t(true, false,4, m_mainWin.grd_Msg.RowDefinitions[4].ActualHeight));
           
            menu_View_Msg();

            chk_Msg_Expanler();

            grdspl_Control();
        }

        //Tools
        private void menu_View_Tool()
        {           
            m_mainWin.menu_View_Tool_Base.Click += delegate
            {
                m_mainWin.menu_View_Tool_Base.IsChecked = !m_mainWin.menu_View_Tool_Base.IsChecked;
                if (true == m_mainWin.menu_View_Tool_Base.IsChecked)
                    m_mainWin.bdr_Tool_Base.Visibility = Visibility.Visible;
                else
                    m_mainWin.bdr_Tool_Base.Visibility = Visibility.Collapsed;              
            };

            m_mainWin.menu_View_Tool_Fast.Click += delegate
            {
                m_mainWin.menu_View_Tool_Fast.IsChecked = !m_mainWin.menu_View_Tool_Fast.IsChecked;
                if (true == m_mainWin.menu_View_Tool_Fast.IsChecked)
                    m_mainWin.bdr_Tool_Fast.Visibility = Visibility.Visible;            
                else
                    m_mainWin.bdr_Tool_Fast.Visibility = Visibility.Collapsed;
            };
            

            m_mainWin.menu_View_Tool_Ctrl.Click += delegate
            {
                m_mainWin.menu_View_Tool_Ctrl.IsChecked = !m_mainWin.menu_View_Tool_Ctrl.IsChecked;
                if (true == m_mainWin.menu_View_Tool_Ctrl.IsChecked)
                    m_mainWin.bdr_Tool_Ctrl.Visibility = Visibility.Visible;         
                else
                    m_mainWin.bdr_Tool_Ctrl.Visibility = Visibility.Collapsed;
            };

            m_mainWin.menu_View_Tool_Help.Click += delegate
            {
                m_mainWin.menu_View_Tool_Ctrl.IsChecked = !m_mainWin.menu_View_Tool_Ctrl.IsChecked;
                if (true == m_mainWin.menu_View_Tool_Ctrl.IsChecked)
                    m_mainWin.bdr_Tool_Ctrl.Visibility = Visibility.Visible;
                else
                    m_mainWin.bdr_Tool_Ctrl.Visibility = Visibility.Collapsed;
            };
        }

        //Manager
        private void menu_View_Manager()
        {

            m_mainWin.menu_View_Mgr_Org.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Org.IsChecked = !m_mainWin.menu_View_Mgr_Org.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Org.IsChecked)
                    m_mainWin.tab_Mgr_Org.Visibility = Visibility.Visible;
                else
                    m_mainWin.tab_Mgr_Org.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Group.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Group.IsChecked = !m_mainWin.menu_View_Mgr_Group.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Group.IsChecked)
                    m_mainWin.tab_Mgr_Group.Visibility = Visibility.Visible;
                else
                    m_mainWin.tab_Mgr_Group.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Employee.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Employee.IsChecked = !m_mainWin.menu_View_Mgr_Employee.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Employee.IsChecked)
                    m_mainWin.tab_Mgr_Employee.Visibility = Visibility.Visible;
                else
                    m_mainWin.tab_Mgr_Employee.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Vehicle.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Vehicle.IsChecked = !m_mainWin.menu_View_Mgr_Vehicle.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Vehicle.IsChecked)
                    m_mainWin.tab_Mgr_Vehicle.Visibility = Visibility.Visible;
                else
                    m_mainWin.tab_Mgr_Vehicle.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Device.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Device.IsChecked = !m_mainWin.menu_View_Mgr_Device.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Device.IsChecked)
                    m_mainWin.tab_Mgr_Device.Visibility = Visibility.Visible;
                else
                    m_mainWin.tab_Mgr_Device.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };
        }
       
        private void menu_View_Manager_Check()
        {
            if ((false == m_mainWin.menu_View_Mgr_Org.IsChecked)
               && (false == m_mainWin.menu_View_Mgr_Group.IsChecked)
               && (false == m_mainWin.menu_View_Mgr_Employee.IsChecked)
               && (false == m_mainWin.menu_View_Mgr_Vehicle.IsChecked)
               && (false == m_mainWin.menu_View_Mgr_Device.IsChecked))
            {
                //hide manager win
                m_mainWin.grd_Nav.RowDefinitions[0].MinHeight = 0;
                MgrRowLength = m_mainWin.grd_Nav.RowDefinitions[0].Height;
                m_mainWin.grd_Nav.RowDefinitions[0].Height = new GridLength(0);

                m_mainWin.grd_Nav.RowDefinitions[1].Height =  new GridLength(1, GridUnitType.Star);
                m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_main.ActualHeight;

                m_mainWin.grdspl_Mgs_Nav.Visibility = Visibility.Hidden;
            }
            else if(m_mainWin.grd_Row_Mgr.Height == new GridLength(0))
            {
                m_mainWin.grd_Nav.RowDefinitions[0].MinHeight = 31;
                m_mainWin.grd_Row_Mgr.Height = MgrRowLength;
                m_mainWin.grdspl_Mgs_Nav.Visibility = Visibility.Visible;
            }
        }

        private void menu_View_Nav()
        {
            m_mainWin.menu_View_Nav_Map.Click += delegate
            {
                m_mainWin.menu_View_Nav_Map.IsChecked = !m_mainWin.menu_View_Nav_Map.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Map.IsChecked)
                    m_mainWin.rad_Nav_Map.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Nav_Map.Visibility = Visibility.Collapsed;
            };

            m_mainWin.menu_View_Nav_Recording.Click += delegate
            {
                m_mainWin.menu_View_Nav_Recording.IsChecked = !m_mainWin.menu_View_Nav_Recording.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Recording.IsChecked)
                    m_mainWin.rad_Nav_Recording.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Nav_Recording.Visibility = Visibility.Collapsed;
            };

            m_mainWin.menu_View_Nav_Report.Click += delegate
            {
                m_mainWin.menu_View_Nav_Report.IsChecked = !m_mainWin.menu_View_Nav_Report.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Report.IsChecked)
                    m_mainWin.rad_Nav_Report.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Nav_Report.Visibility = Visibility.Collapsed;
            };
        }

        
        private void menu_View_Event()
        {          
            m_mainWin.menu_View_Event.Click += delegate
            {
                m_mainWin.menu_View_Event.IsChecked = !m_mainWin.menu_View_Event.IsChecked;
                if (false == m_mainWin.menu_View_Event.IsChecked)
                {
                    m_mainWin.grd_Disptch.RowDefinitions[1].MinHeight = 0;
                    EventRowLength = m_mainWin.grd_Disptch.RowDefinitions[1].Height;
                    m_mainWin.grd_Disptch.RowDefinitions[1].Height = new GridLength(0);
                    m_mainWin.grd_Disptch.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                    m_mainWin.grd_Disptch.RowDefinitions[0].MaxHeight = m_mainWin.grd_main.ActualHeight;

                    m_mainWin.grdspl_Main_Event.Visibility = Visibility.Hidden;
                }
                else if (new GridLength(0) == m_mainWin.grd_Row_Event.Height)
                {
                    m_mainWin.grd_Disptch.RowDefinitions[1].MinHeight = 31;
                    m_mainWin.grd_Row_Event.Height = EventRowLength;

                    m_mainWin.grdspl_Main_Event.Visibility = Visibility.Visible;
                }
            };
        }

        
        private void menu_View_Msg()
        {
            m_mainWin.menu_View_Msg_Alarm.Click += delegate
            {
                m_mainWin.menu_View_Msg_Alarm.IsChecked = !m_mainWin.menu_View_Msg_Alarm.IsChecked;
                if (false == m_mainWin.menu_View_Msg_Alarm.IsChecked)
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Alarm].show = false;
                }
                else
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Alarm].show = true;
                }
                update_Msg_Box();
            };

            m_mainWin.menu_View_Msg_ShortMsg.Click += delegate
            {
                m_mainWin.menu_View_Msg_ShortMsg.IsChecked = !m_mainWin.menu_View_Msg_ShortMsg.IsChecked;
                if (false == m_mainWin.menu_View_Msg_ShortMsg.IsChecked)
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_ShortMsg].show = false;
                }
                else
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_ShortMsg].show = true;
                }
                update_Msg_Box();
            };

            m_mainWin.menu_View_Msg_Rx.Click += delegate
            {
                m_mainWin.menu_View_Msg_Rx.IsChecked = !m_mainWin.menu_View_Msg_Rx.IsChecked;
                if (false == m_mainWin.menu_View_Msg_Rx.IsChecked)
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Rx].show = false;
                }
                else
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Rx].show = true;
                }
                update_Msg_Box();
            };

            m_mainWin.menu_View_Msg_Job.Click += delegate
            {
                m_mainWin.menu_View_Msg_Job.IsChecked = !m_mainWin.menu_View_Msg_Job.IsChecked;
                if (false == m_mainWin.menu_View_Msg_Job.IsChecked)
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Job].show = false;
                }
                else
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Job].show = true;
                }
                update_Msg_Box();
            };

            m_mainWin.menu_View_Msg_Tracker.Click += delegate
            {
                m_mainWin.menu_View_Msg_Tracker.IsChecked = !m_mainWin.menu_View_Msg_Tracker.IsChecked;
                if (false == m_mainWin.menu_View_Msg_Tracker.IsChecked)
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Tracker].show = false;
                }
                else
                {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Tracker].show = true;
                }
                update_Msg_Box();
            };
        }

        private void update_Msg_Box()
        {
            int msgGrdRow = 0;
            int lastExpBordr = -1;
            bool isexpLast = false;
            bool isupdateheight = false;

            GridSplitter[] gslist = new GridSplitter[4]{m_mainWin.grdspl_Msg_Row0, m_mainWin.grdspl_Msg_Row1, m_mainWin.grdspl_Msg_Row2, m_mainWin.grdspl_Msg_Row3};


            foreach (var item in m_bdrMsgList)
            {
                m_bdrMsgList[item.Key].height = m_mainWin.grd_Msg.RowDefinitions[item.Value.rowindex].ActualHeight;
                if((true == item.Value.show) && (true == item.Value.expanler))isupdateheight = true;
            }

            foreach (var item in m_bdrMsgList)
            {
                if (false == item.Value.show)
                    item.Key.SetValue(Grid.RowProperty, 5);
                else
                {
                    if (true == item.Value.expanler)
                    {
                        lastExpBordr = msgGrdRow;

                        if(true == isupdateheight)
                          m_mainWin.grd_Msg.RowDefinitions[msgGrdRow].Height = new GridLength(1, GridUnitType.Star);                         
                        else
                            m_mainWin.grd_Msg.RowDefinitions[msgGrdRow].Height = new GridLength(item.Value.height);
                        if (1 <= msgGrdRow)
                            if (true == isexpLast)
                                gslist[msgGrdRow - 1].IsEnabled = true;
                            else
                                gslist[msgGrdRow - 1].IsEnabled = false;

                        isexpLast = true;
                    }
                    else 
                    {
                        m_mainWin.grd_Msg.RowDefinitions[msgGrdRow].Height = new GridLength(31);
                        if (1 <= msgGrdRow) gslist[msgGrdRow - 1].IsEnabled = false;
                        isexpLast = false;
                    }

                    m_bdrMsgList[item.Key].rowindex = msgGrdRow;
                    item.Key.SetValue(Grid.RowProperty, msgGrdRow++);
                }
            }

            if (msgGrdRow > 0)
            if(false == isexpLast)m_mainWin.grd_Msg.RowDefinitions[msgGrdRow-1].Height = new GridLength(30);

            if (lastExpBordr >= 0)
            {
                m_mainWin.grd_Msg.RowDefinitions[lastExpBordr].Height = new GridLength(1, GridUnitType.Star);
            }

            for(int i = msgGrdRow; i < 5; i++)
            {
                m_mainWin.grd_Msg.RowDefinitions[i].Height = new GridLength(0);
            }
        }

        private void chk_Msg_Expanler()
        {            
            //alarm
            m_mainWin.chk_MsgExp_Alarm.Checked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Alarm].expanler = true;
                update_Msg_Box();
            };

            m_mainWin.chk_MsgExp_Alarm.Unchecked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Alarm].expanler = false;
                update_Msg_Box();
            };

            //short message
            m_mainWin.chk_MsgExp_ShortMsg.Checked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_ShortMsg].expanler = true;
                update_Msg_Box();
            };

            m_mainWin.chk_MsgExp_ShortMsg.Unchecked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_ShortMsg].expanler = false;
                update_Msg_Box();
            };

            //rx
            m_mainWin.chk_MsgExp_Rx.Checked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Rx].expanler = true;
                update_Msg_Box();
            };

            m_mainWin.chk_MsgExp_Rx.Unchecked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Rx].expanler = false;
                update_Msg_Box();
            };

            //Job
            m_mainWin.chk_MsgExp_Job.Checked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Job].expanler = true;
                update_Msg_Box();
            };

            m_mainWin.chk_MsgExp_Job.Unchecked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Job].expanler = false;
                update_Msg_Box();
            };

            //Tracker
            m_mainWin.chk_MsgExp_Tracker.Checked += delegate
            {
                    m_bdrMsgList[m_mainWin.bdr_Msg_Tracker].expanler = true;
                    update_Msg_Box();
            };

            m_mainWin.chk_MsgExp_Tracker.Unchecked += delegate
            {
                m_bdrMsgList[m_mainWin.bdr_Msg_Tracker].expanler = false;
                update_Msg_Box();
            };
        }

        private void grdspl_Control()
        { 
            
            m_mainWin.grdspl_Nav_Main.PreviewMouseLeftButtonDown += delegate
            {
                m_mainWin.grd_main.ColumnDefinitions[0].MaxWidth = m_mainWin.grd_main.ActualWidth - m_mainWin.grd_main.ColumnDefinitions[2].ActualWidth - m_mainWin.grd_main.ColumnDefinitions[1].MinWidth;
            };

            m_mainWin.grdspl_Main_Msg.PreviewMouseLeftButtonDown += delegate
            {
                m_mainWin.grd_main.ColumnDefinitions[2].MaxWidth = m_mainWin.grd_main.ActualWidth - m_mainWin.grd_main.ColumnDefinitions[0].ActualWidth - m_mainWin.grd_main.ColumnDefinitions[1].MinWidth;
            };
            
            m_mainWin.grdspl_Mgs_Nav.PreviewMouseLeftButtonDown += delegate
            {
                m_mainWin.grd_Nav.RowDefinitions[0].MaxHeight = m_mainWin.grd_main.ActualHeight - 31;
                m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_main.ActualHeight - 30;
                m_mainWin.grd_Nav.RowDefinitions[1].MinHeight = 30;
            };

            m_mainWin.grdspl_Main_Event.PreviewMouseLeftButtonDown += delegate
            {
                m_mainWin.grd_Disptch.RowDefinitions[0].MaxHeight = m_mainWin.grd_main.ActualHeight - 30;
                m_mainWin.grd_Disptch.RowDefinitions[1].MaxHeight = m_mainWin.grd_main.ActualHeight - 30;
                
            };

        }

        public void On_Mouse_Pressed()
        {
            //left-top(width, 0) when FlowDirection is Right to left
            Point ep = m_mainWin.exp_New.TranslatePoint(new Point(m_mainWin.exp_New.ActualWidth, 0), (UIElement)m_mainWin);
            Point mp = Mouse.GetPosition((UIElement)m_mainWin);

            //doglle button width :20
            if ((mp.X < ep.X + 20) || (mp.X > ep.X + m_mainWin.exp_New.ActualWidth) || (mp.Y < ep.Y) || (mp.Y > ep.Y + m_mainWin.exp_New.ActualHeight))
            {
                if (true == m_mainWin.exp_New.IsExpanded) m_mainWin.exp_New.IsExpanded = false;
            }
        }
    }
}
