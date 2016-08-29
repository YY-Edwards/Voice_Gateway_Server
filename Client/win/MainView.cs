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

        newFast m_newFastWin;
        NewOperate m_newOperateWin;

        private Dictionary<Border, MsgBox_t> m_bdrMsgList = new Dictionary<Border, MsgBox_t>();

        private GridLength EventRowLength
            , MsgAlarmLength
            , MsgShortMsgLength
            , MsgRxLength
            , MsgJobLength
            , MsgTrackerLength;

        public MainView(Main win)
        {
            if (null == win) return;
                
            m_mainWin = win;

            m_mainWin.bdr_MaskMainTab.ClipToBounds = true;
            m_mainWin.bdr_MaskMgrTab.ClipToBounds = true;

            m_newFastWin = new newFast();
            m_newOperateWin = new NewOperate();


            menu_View_Tool();

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

            SelectedMainTab();

            CreateNewOperateWindow();
            CreateNewFastWindow();
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
                    m_mainWin.rad_Mgr_Org.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Mgr_Org.Visibility = Visibility.Collapsed;

                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Group.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Group.IsChecked = !m_mainWin.menu_View_Mgr_Group.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Group.IsChecked)
                    m_mainWin.rad_Mgr_Group.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Mgr_Group.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Employee.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Employee.IsChecked = !m_mainWin.menu_View_Mgr_Employee.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Employee.IsChecked)
                    m_mainWin.rad_Mgr_Employee.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Mgr_Employee.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Vehicle.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Vehicle.IsChecked = !m_mainWin.menu_View_Mgr_Vehicle.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Vehicle.IsChecked)
                    m_mainWin.rad_Mgr_Vehicle.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Mgr_Vehicle.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.menu_View_Mgr_Device.Click += delegate
            {
                m_mainWin.menu_View_Mgr_Device.IsChecked = !m_mainWin.menu_View_Mgr_Device.IsChecked;
                if (true == m_mainWin.menu_View_Mgr_Device.IsChecked)
                    m_mainWin.rad_Mgr_Device.Visibility = Visibility.Visible;
                else
                    m_mainWin.rad_Mgr_Device.Visibility = Visibility.Collapsed;
                menu_View_Manager_Check();
            };

            m_mainWin.rad_Mgr_Org.Checked += delegate{ m_mainWin.tab_Mgr.SelectedIndex = 0; };
            m_mainWin.rad_Mgr_Group.Checked += delegate { m_mainWin.tab_Mgr.SelectedIndex = 1; };
            m_mainWin.rad_Mgr_Employee.Checked += delegate { m_mainWin.tab_Mgr.SelectedIndex = 2; };
            m_mainWin.rad_Mgr_Vehicle.Checked += delegate { m_mainWin.tab_Mgr.SelectedIndex = 3; };
            m_mainWin.rad_Mgr_Device.Checked += delegate { m_mainWin.tab_Mgr.SelectedIndex = 4; };
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
                m_mainWin.grd_Nav.RowDefinitions[0].Height = new GridLength(0);

                m_mainWin.grd_Nav.RowDefinitions[1].Height =  new GridLength(1, GridUnitType.Star);
                m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_main.ActualHeight;

                m_mainWin.grdspl_Mgs_Nav.Visibility = Visibility.Hidden;
            }
            else if(m_mainWin.grd_Row_Mgr.Height == new GridLength(0))
            {

                m_mainWin.grd_Nav.RowDefinitions[0].Height = new GridLength(1,GridUnitType.Star);
                m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = 121;

                m_mainWin.grdspl_Mgs_Nav.Visibility = Visibility.Visible;
            }
        }

        private void menu_View_Nav()
        {
            m_mainWin.menu_View_Nav_Map.Click += delegate
            {
                m_mainWin.menu_View_Nav_Map.IsChecked = !m_mainWin.menu_View_Nav_Map.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Map.IsChecked)
                {
                    if (Visibility.Visible != m_mainWin.rad_Nav_Map.Visibility)
                    {
                        m_mainWin.rad_Nav_Map.Visibility = Visibility.Visible;
                        m_mainWin.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30;
                    }
                }
                else
                {
                    if (Visibility.Visible == m_mainWin.rad_Nav_Map.Visibility)
                    {
                        m_mainWin.rad_Nav_Map.Visibility = Visibility.Collapsed;
                        m_mainWin.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30;
                    }
                }
                    
                    
            };

            m_mainWin.menu_View_Nav_Recording.Click += delegate
            {
                m_mainWin.menu_View_Nav_Recording.IsChecked = !m_mainWin.menu_View_Nav_Recording.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Recording.IsChecked)
                {
                    if (Visibility.Visible != m_mainWin.rad_Nav_Recording.Visibility)
                    {
                        m_mainWin.rad_Nav_Recording.Visibility = Visibility.Visible;
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30;
                    }
                }
                else
                {
                    if (Visibility.Visible == m_mainWin.rad_Nav_Recording.Visibility)
                    {
                        m_mainWin.rad_Nav_Recording.Visibility = Visibility.Collapsed;
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30;
                    }
                }

            };

            m_mainWin.menu_View_Nav_Report.Click += delegate
            {
                m_mainWin.menu_View_Nav_Report.IsChecked = !m_mainWin.menu_View_Nav_Report.IsChecked;
                if (true == m_mainWin.menu_View_Nav_Report.IsChecked)
                {
                     if (Visibility.Visible != m_mainWin.rad_Nav_Report.Visibility)
                    {
                        m_mainWin.rad_Nav_Report.Visibility = Visibility.Visible;
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight + 30;
                    }
                }                 
                else
                {
                    if (Visibility.Visible == m_mainWin.rad_Nav_Report.Visibility)
                    {
                        m_mainWin.rad_Nav_Report.Visibility = Visibility.Collapsed;
                        m_mainWin.grd_Nav.RowDefinitions[1].Height = new GridLength(m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                        m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = m_mainWin.grd_Nav.RowDefinitions[1].ActualHeight - 30;
                    }
                }
                    
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
                    m_bdrMsgList[m_mainWin.bdr_Msg_Alarm].expanler = true;
                    m_mainWin.chk_MsgExp_Alarm.IsChecked = true;
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
                    m_bdrMsgList[m_mainWin.bdr_Msg_ShortMsg].expanler = true;
                    m_mainWin.chk_MsgExp_ShortMsg.IsChecked = true;
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
                    m_bdrMsgList[m_mainWin.bdr_Msg_Rx].expanler = true;
                    m_mainWin.chk_MsgExp_Rx.IsChecked = true;
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
                    m_bdrMsgList[m_mainWin.bdr_Msg_Job].expanler = true;
                    m_mainWin.chk_MsgExp_Job.IsChecked = true;
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
                    m_bdrMsgList[m_mainWin.bdr_Msg_Tracker].expanler = true;
                    m_mainWin.chk_MsgExp_Tracker.IsChecked = true;
                }
                update_Msg_Box();
            };
        }

        private void update_Msg_Box()
        {
            int show_count = 0;
            int the_last_explaner_index = -1;
            bool is_need_update_hight = false;
            bool is_first_show = true;
            bool is_last_explaner = false;


            GridSplitter[] message_grid_spl_list = new GridSplitter[4]{ m_mainWin.grdspl_Msg_Row0, m_mainWin.grdspl_Msg_Row1, m_mainWin.grdspl_Msg_Row2, m_mainWin.grdspl_Msg_Row3 };

            foreach(var item in m_bdrMsgList)
            {
                 m_bdrMsgList[item.Key].height = m_mainWin.grd_Msg.RowDefinitions[item.Value.rowindex].ActualHeight;

                 if((true == item.Value.show) && (true == item.Value.expanler) &&  (30 >=m_mainWin.grd_Msg.RowDefinitions[item.Value.rowindex].ActualHeight))
                 {
                     is_need_update_hight = true;
                 }
            }



            foreach (var item in m_bdrMsgList)
            {
                if(true == item.Value.show)
                {
                    item.Key.SetValue(Grid.RowProperty, show_count);

                    if (true == is_first_show)
                    {
                        m_mainWin.grd_Msg.RowDefinitions[show_count].MinHeight = 31;

                        item.Key.BorderThickness = new Thickness(1);
                    }
                    else
                    {
                        item.Key.BorderThickness = new Thickness(1,0,1,1);
                    }

                    if (true == item.Value.expanler)
                    {
                        m_mainWin.grd_Msg.RowDefinitions[show_count].MinHeight = 60;
                        
                        if (true == is_need_update_hight)
                        {
                            m_mainWin.grd_Msg.RowDefinitions[show_count].Height = new GridLength(1, GridUnitType.Star);
                        }
                        else
                        {
                            m_mainWin.grd_Msg.RowDefinitions[show_count].Height = new GridLength(item.Value.height);
                        }
                        the_last_explaner_index = show_count;

                        if (true == is_last_explaner)
                        {
                             if(1 <= show_count)message_grid_spl_list[show_count - 1].Visibility = Visibility.Visible;
                        }
                        else
                        {
                             if(1 <= show_count)message_grid_spl_list[show_count - 1].Visibility = Visibility.Hidden;
                        }

                        is_last_explaner = true;
                    }
                    else
                    {
                        if (true == is_first_show)
                        {
                            m_mainWin.grd_Msg.RowDefinitions[show_count].MinHeight = 31;
                            m_mainWin.grd_Msg.RowDefinitions[show_count].Height = new GridLength(31);
                        }
                        else
                        {
                            m_mainWin.grd_Msg.RowDefinitions[show_count].MinHeight = 30;
                            m_mainWin.grd_Msg.RowDefinitions[show_count].Height = new GridLength(30);
                        }

                        if(1 <= show_count)message_grid_spl_list[show_count - 1].Visibility = Visibility.Hidden;
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

            if (the_last_explaner_index >=0 )m_mainWin.grd_Msg.RowDefinitions[the_last_explaner_index].Height = new GridLength(1, GridUnitType.Star);

            for (int i = show_count; i < 6; i++)
            {
                m_mainWin.grd_Msg.RowDefinitions[show_count].MinHeight = 0;
                m_mainWin.grd_Msg.RowDefinitions[show_count].Height = new GridLength(0);
            }


            if ((false == m_mainWin.menu_View_Msg_Alarm.IsChecked)
               && (false == m_mainWin.menu_View_Msg_ShortMsg.IsChecked)
               && (false == m_mainWin.menu_View_Msg_Rx.IsChecked)
               && (false == m_mainWin.menu_View_Msg_Job.IsChecked)
               && (false == m_mainWin.menu_View_Msg_Tracker.IsChecked))
            {
                //hide manager win
                m_mainWin.grd_main.ColumnDefinitions[2].MinWidth = 0;
                m_mainWin.grd_main.ColumnDefinitions[2].Width = new GridLength(0);

                m_mainWin.grd_main.ColumnDefinitions[1].Width = new GridLength(1, GridUnitType.Star);
                m_mainWin.grd_main.ColumnDefinitions[1].MaxWidth = m_mainWin.grd_main.ActualWidth - m_mainWin.grd_main.ColumnDefinitions[0].MinWidth;

                m_mainWin.grdspl_Main_Msg.Visibility = Visibility.Hidden;
            }
            else if(m_mainWin.grd_main.ColumnDefinitions[2].ActualWidth == 0)
            {
                m_mainWin.grd_main.ColumnDefinitions[2].MinWidth = 100;
                m_mainWin.grd_main.ColumnDefinitions[2].Width = new GridLength(200);

                m_mainWin.grdspl_Main_Msg.Visibility = Visibility.Visible;
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
                m_mainWin.grd_Nav.RowDefinitions[1].MaxHeight = 121;
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


        private void SelectedMainTab()
        {
            m_mainWin.rad_Nav_Dispatch.Checked += delegate
            {
                m_mainWin.tab_main.SelectedIndex = 0;
            };

            m_mainWin.rad_Nav_Map.Checked += delegate
            {
                m_mainWin.tab_main.SelectedIndex = 1;
            };

            m_mainWin.rad_Nav_Recording.Checked += delegate
            {
                m_mainWin.tab_main.SelectedIndex = 2;
            };

            m_mainWin.rad_Nav_Report.Checked += delegate
            {
                m_mainWin.tab_main.SelectedIndex = 3;
            };

        }

        private void CreateNewOperateWindow()
        {

            m_mainWin.exp_New.PreviewMouseLeftButtonUp += delegate
            {
                //left-top(width, 0) when FlowDirection is Right to left
                Point ep = m_mainWin.exp_New.TranslatePoint(new Point(m_mainWin.exp_New.ActualWidth, 0), (UIElement)m_mainWin);
                Point mp = Mouse.GetPosition((UIElement)m_mainWin);

                //doglle button width :20
                if ((mp.X < ep.X + 20) || (mp.X > ep.X + m_mainWin.exp_New.ActualWidth) || (mp.Y < ep.Y) || (mp.Y > ep.Y + m_mainWin.exp_New.ActualHeight))
                {
                    m_newOperateWin.rad_CreateCall.IsChecked = true;
                    m_newOperateWin.tab_NewType.SelectedIndex = 0;
                    m_newOperateWin.ShowDialog(); 
                }

            };
            
            m_mainWin.lst_Tool_New.SelectionChanged += delegate
            {
                
                m_mainWin.exp_New.IsExpanded = false;
                 
               m_newOperateWin.tab_NewType.SelectedIndex = m_mainWin.lst_Tool_New.SelectedIndex;

                switch (m_mainWin.lst_Tool_New.SelectedIndex)
                {
                    case 0:
                        m_newOperateWin.rad_CreateCall.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        
                        break;
                    case 1:
                        m_newOperateWin.rad_CreateMsg.IsChecked = true;
                        m_newOperateWin.ShowDialog();                      
                        break;
                    case 2:
                        m_newOperateWin.rad_CreatePosition.IsChecked = true;
                        m_newOperateWin.ShowDialog();                      
                        break;
                    case 3:
                        m_newOperateWin.rad_CreateCtrl.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 4:
                        m_newOperateWin.rad_CreateJob.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 5:
                        m_newOperateWin.rad_CreateTracker.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    default:
                        break;
                }

                m_mainWin.lst_Tool_New.SelectedIndex = -1;

            };
        }

        private void CreateNewFastWindow()
        {
            m_mainWin.btn_Tool_NewContact.Click += delegate
            {
                m_newFastWin.rad_CreateContact.IsChecked = true;
                m_newFastWin.tab_CreatFast.SelectedIndex = 0;
                m_newFastWin.ShowDialog();
            };

            m_mainWin.btn_Tool_NewGroup.Click += delegate
            {
                m_newFastWin.rad_CreateGroup.IsChecked = true;
                m_newFastWin.tab_CreatFast.SelectedIndex = 1;
                m_newFastWin.ShowDialog();
            };

            m_mainWin.btn_Tool_NewFastOperate.Click += delegate
            {
                m_newFastWin.rad_CreateFastOperate.IsChecked = true;
                m_newFastWin.tab_CreatFast.SelectedIndex = 1;
                m_newFastWin.ShowDialog();
            };
        }
    }
} 