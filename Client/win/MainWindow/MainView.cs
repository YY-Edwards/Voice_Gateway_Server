using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace TrboX
{



    public class MainView
    {
        private Main m_Main;
        
        //private Main m_mainWin;

        //public NotifyView m_NotifyView;
        private Dictionary<Border, MsgBox_t> m_bdrMsgList = new Dictionary<Border, MsgBox_t>();



        public MainView(Main win)
        {
            if (null == win) return;
            m_Main = win;
            //m_mainWin = win;

            GrdSplControlRegister();
            //m_mainWin.bdr_MaskMainTab.ClipToBounds = true;
            //m_mainWin.bdr_MaskMgrTab.ClipToBounds = true;

            //m_newFastWin = new newFast();
           
            //m_NotifyView = new NotifyView(m_mainWin);

            //m_mainWin.lst_Event.View = (ViewBase)m_mainWin.FindResource("EventView");
          

            ////menu_View_Tool();

            //menu_View_Manager();

            //menu_View_Nav();

            //EventRowLength = m_mainWin.grd_Row_Event.Height;
            //menu_View_Event();

            //MsgAlarmLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            //MsgShortMsgLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            //MsgRxLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            //MsgJobLength = m_mainWin.grd_Row_Msg_Alarm.Height;
            //MsgTrackerLength = m_mainWin.grd_Row_Msg_Alarm.Height;

            //m_bdrMsgList.Add(m_mainWin.bdr_Msg_Alarm, new MsgBox_t(true, true,0, m_mainWin.grd_Msg.RowDefinitions[0].ActualHeight));
            //m_bdrMsgList.Add(m_mainWin.bdr_Msg_ShortMsg, new MsgBox_t(true, false,1, m_mainWin.grd_Msg.RowDefinitions[1].ActualHeight));
            //m_bdrMsgList.Add(m_mainWin.bdr_Msg_Rx, new MsgBox_t(true, false,2, m_mainWin.grd_Msg.RowDefinitions[2].ActualHeight));
            //m_bdrMsgList.Add(m_mainWin.bdr_Msg_Job, new MsgBox_t(true, false,3, m_mainWin.grd_Msg.RowDefinitions[3].ActualHeight));
            //m_bdrMsgList.Add(m_mainWin.bdr_Msg_Tracker, new MsgBox_t(true, false,4, m_mainWin.grd_Msg.RowDefinitions[4].ActualHeight));
           
            //menu_View_Msg();

            //chk_Msg_Expanler();


            //SelectedMainTab();

        }

        private void GrdSplControlRegister()
        {

            m_Main.grdspl_Nav_Main.PreviewMouseLeftButtonDown += delegate
            {
                m_Main.grd_main.ColumnDefinitions[0].MaxWidth = m_Main.grd_main.ActualWidth - m_Main.grd_main.ColumnDefinitions[2].ActualWidth - m_Main.grd_main.ColumnDefinitions[1].MinWidth;
            };

            m_Main.grdspl_Main_Msg.PreviewMouseLeftButtonDown += delegate
            {
                m_Main.grd_main.ColumnDefinitions[2].MaxWidth = m_Main.grd_main.ActualWidth - m_Main.grd_main.ColumnDefinitions[0].ActualWidth - m_Main.grd_main.ColumnDefinitions[1].MinWidth;
            };

            m_Main.grdspl_Mgs_Nav.PreviewMouseLeftButtonDown += delegate
            {
                m_Main.grd_Nav.RowDefinitions[0].MaxHeight = m_Main.grd_main.ActualHeight - 31;
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = 121;
                m_Main.grd_Nav.RowDefinitions[1].MinHeight = 30;
            };

            m_Main.grdspl_Main_Event.PreviewMouseLeftButtonDown += delegate
            {
                m_Main.grd_Disptch.RowDefinitions[0].MaxHeight = m_Main.grd_main.ActualHeight - 30;
                m_Main.grd_Disptch.RowDefinitions[1].MaxHeight = m_Main.grd_main.ActualHeight - 30;

            };

        }

        public void ToolsBaseShow(bool show, bool enable)
        {
            m_Main.menu_View_Tool_Base.IsChecked = show;          
            m_Main.ToolBar.OperateShow(!show);
            m_Main.ToolBar.OperateEnable(!enable);
        }

        public void ToolsFastShow(bool show, bool enable)
        {
            m_Main.menu_View_Tool_Fast.IsChecked = show;  
            m_Main.ToolBar.FastShow(!show);
        }

        public void ToolsCtrlShow(bool show, bool enable)
        {
            m_Main.menu_View_Tool_Ctrl.IsChecked = show;  
            m_Main.ToolBar.ControlShow(!show);
            m_Main.ToolBar.ControlEnable(!enable);
        }

        public void ToolsHelpShow(bool show, bool enable)
        {
            m_Main.menu_View_Tool_Help.IsChecked = show;  
            m_Main.ToolBar.HelpShow(!show);
        }

        public void ResrcMgrOrgShow(bool show)
        {
            m_Main.menu_View_Mgr_Org.IsChecked = show;
            if (show)
            {
                m_Main.rad_Mgr_Org.Visibility = Visibility.Visible;
                m_Main.rad_Mgr_Org.IsChecked = true;
            }               
            else
                m_Main.rad_Mgr_Org.Visibility = Visibility.Collapsed;

            CheckResrcMgrVisibieIndex();
            ResrcMgrShowCheck();
        }
        public void ResrcMgrGroupShow(bool show)
        {
            m_Main.menu_View_Mgr_Group.IsChecked = show;
            if (show)
            {
                m_Main.rad_Mgr_Group.Visibility = Visibility.Visible;
                m_Main.rad_Mgr_Group.IsChecked = true;
            }
            else
                m_Main.rad_Mgr_Group.Visibility = Visibility.Collapsed;

            CheckResrcMgrVisibieIndex();
            ResrcMgrShowCheck();
        }
        public void ResrcMgrEmployeeShow(bool show)
        {
            m_Main.menu_View_Mgr_Employee.IsChecked = show;
            if (show)
            {
                m_Main.rad_Mgr_Employee.Visibility = Visibility.Visible;
                m_Main.rad_Mgr_Employee.IsChecked = true;
            }
            else
                m_Main.rad_Mgr_Employee.Visibility = Visibility.Collapsed;

            CheckResrcMgrVisibieIndex();
            ResrcMgrShowCheck();
        }
        public void ResrcMgrVehicleShow(bool show)
        {
            m_Main.menu_View_Mgr_Vehicle.IsChecked = show;
            if (show)
            {
                m_Main.rad_Mgr_Vehicle.Visibility = Visibility.Visible;
                m_Main.rad_Mgr_Vehicle.IsChecked = true;
            }
            else
                m_Main.rad_Mgr_Vehicle.Visibility = Visibility.Collapsed;

            CheckResrcMgrVisibieIndex();
            ResrcMgrShowCheck();
        }
        public void ResrcMgrDeviceShow(bool show)
        {
            m_Main.menu_View_Mgr_Device.IsChecked = show;
            if (show)
            {
                m_Main.rad_Mgr_Device.Visibility = Visibility.Visible;
                m_Main.rad_Mgr_Device.IsChecked = true;
            }
            else
                m_Main.rad_Mgr_Device.Visibility = Visibility.Collapsed;

            CheckResrcMgrVisibieIndex();
            ResrcMgrShowCheck();
        }

        private void CheckResrcMgrVisibieIndex()
        {
            RadioButton[] disp = new RadioButton[5] {                
            m_Main.rad_Mgr_Org, 
            m_Main.rad_Mgr_Group, 
            m_Main.rad_Mgr_Employee,
            m_Main.rad_Mgr_Vehicle,
            m_Main.rad_Mgr_Device,
            };

            if (m_Main.tab_Mgr.SelectedIndex >= 0)
            {
                if (Visibility.Collapsed == disp[m_Main.tab_Mgr.SelectedIndex].Visibility)
                {
                    for(int i = 0; i < 5; i++)
                    {
                        if (Visibility.Visible == disp[i].Visibility)
                        {
                            disp[i].IsChecked = true;
                            return;
                        }
                    }
                }
            }
        }

        private double NavigationMaxHeight = 0;
        private void ResrcMgrShowCheck()
        {
            if ((Visibility.Collapsed == m_Main.rad_Mgr_Org.Visibility)
               && (Visibility.Collapsed == m_Main.rad_Mgr_Group.Visibility)
               && (Visibility.Collapsed == m_Main.rad_Mgr_Employee.Visibility)
               && (Visibility.Collapsed == m_Main.rad_Mgr_Vehicle.Visibility)
               && (Visibility.Collapsed == m_Main.rad_Mgr_Device.Visibility))
            {
                m_Main.grd_Nav.RowDefinitions[0].MinHeight = 0;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(0);

                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(1, GridUnitType.Star);
                NavigationMaxHeight = m_Main.grd_Nav.RowDefinitions[1].MaxHeight;
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_main.ActualHeight;

                m_Main.grdspl_Mgs_Nav.Visibility = Visibility.Hidden;
            }
            else
            {
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = NavigationMaxHeight == 0 ? m_Main.grd_main.ActualHeight : NavigationMaxHeight;

                m_Main.grdspl_Mgs_Nav.Visibility = Visibility.Visible;
            }
        }


        public void NavMapShow(bool show)
        {
            m_Main.menu_View_Nav_Map.IsChecked = show;
            m_Main.rad_Nav_Map.IsChecked = show;
            if(show)
            {
                m_Main.rad_Nav_Map.Visibility = Visibility.Visible;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30;
            }
            else
            {
                m_Main.rad_Nav_Dispatch.IsChecked = true;
                m_Main.rad_Nav_Map.Visibility = Visibility.Collapsed;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30;
            }
        }
        public void NavRecordingShow(bool show)
        {
            m_Main.menu_View_Nav_Recording.IsChecked = show;
            m_Main.rad_Nav_Recording.IsChecked = show;
            if (show)
            {
                m_Main.rad_Nav_Recording.Visibility = Visibility.Visible;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30;
            }
            else
            {
                m_Main.rad_Nav_Dispatch.IsChecked = true;
                m_Main.rad_Nav_Recording.Visibility = Visibility.Collapsed;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30;
            }
        }

        public void NavTrackerShow(bool show)
        {

        }
        public void NavReportShow(bool show)
        {
            m_Main.menu_View_Nav_Report.IsChecked = show;
            m_Main.rad_Nav_Report.IsChecked = show;
            if (show)
            {
                m_Main.rad_Nav_Report.Visibility = Visibility.Visible;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight + 30;
            }
            else
            {
                m_Main.rad_Nav_Dispatch.IsChecked = true;
                m_Main.rad_Nav_Report.Visibility = Visibility.Collapsed;
                m_Main.grd_Nav.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Nav.RowDefinitions[1].Height = new GridLength(m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30);
                m_Main.grd_Nav.RowDefinitions[1].MaxHeight = m_Main.grd_Nav.RowDefinitions[1].ActualHeight - 30;
            }
        }


        private GridLength EventRowLength;
        public void EventMessageShow(bool show)
        {
            m_Main.menu_View_Event.IsChecked = show;
            if(show)
            {
                m_Main.grd_Disptch.RowDefinitions[1].MinHeight = 31;
                m_Main.grd_Row_Event.Height = EventRowLength;

                m_Main.grdspl_Main_Event.Visibility = Visibility.Visible;
            }
            else
            {
                m_Main.grd_Disptch.RowDefinitions[1].MinHeight = 0;
                EventRowLength = m_Main.grd_Disptch.RowDefinitions[1].Height;
                m_Main.grd_Disptch.RowDefinitions[1].Height = new GridLength(0);
                m_Main.grd_Disptch.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                m_Main.grd_Disptch.RowDefinitions[0].MaxHeight = m_Main.grd_main.ActualHeight;

                m_Main.grdspl_Main_Event.Visibility = Visibility.Hidden;
            }
        }


        public void MsgAlarmShow(bool show)
        {
            m_Main.menu_View_Msg_Alarm.IsChecked = show;
            m_Main.MsgWin.MsgAlarmShow(!show);
        }

        public void MsgShortMsgShow(bool show)
        {
            m_Main.menu_View_Msg_ShortMsg.IsChecked = show;
            m_Main.MsgWin.MsgShortMsgShow(!show);
        }

        public void MsgRxShow(bool show)
        {
            m_Main.menu_View_Msg_Rx.IsChecked = show;
            m_Main.MsgWin.MsgRxShow(!show);
        }

        public void MsgJobShow(bool show)
        {
            m_Main.menu_View_Msg_Job.IsChecked = show;
            m_Main.MsgWin.MsgJobShow(!show);
        }

        public void MsgTrackerShow(bool show)
        {
            m_Main.menu_View_Msg_Tracker.IsChecked = show;
            m_Main.MsgWin.MsgTrackerShow(!show);
        }
    }
} 