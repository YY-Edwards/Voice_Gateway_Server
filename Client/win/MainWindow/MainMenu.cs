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
    public class ShortCutKey
    {
        public static RoutedUICommand NewCall =

            new RoutedUICommand("菜单管理(_M)", "MenusCommand", typeof(ShortCutKey),

                new InputGestureCollection(new InputGesture[] {

                    new KeyGesture(Key.N, ModifierKeys.Control ) }));


       // public static RoutedUICommand MangeCommand =

       //new RoutedUICommand("我的管理(_A)", "MangeCommand", typeof(MyCommands),

       //    new InputGestureCollection(new InputGesture[] {

       //             new KeyGesture(Key.A , ModifierKeys.Control) }));

    }
    public class MainMenu
    {
        private Main m_Main;

        public MainMenu(Main win)
        {
            if (null == win) return;           
            m_Main = win;

            FileIteMRegister();
            ViewItemRegister();
            TargetItemRegister();
            RecordingItemRegister();
            TrackerItemRegister();
            ToolsItemRegister();
            HelpItemRegister();
        }


        private void FileIteMRegister()
        {
            //New
            CommandBinding cb = new CommandBinding();
            cb.Command = ShortCutKey.NewCall;
            cb.Executed += new ExecutedRoutedEventHandler(delegate(object sender, ExecutedRoutedEventArgs e) { m_Main.SubWindow.OpenCreateOperateWindow(OPType.Dispatch); });
            m_Main.CommandBindings.Add(cb);
            //m_Main.menu_File_NewDispatch.Command = ShortCutKey.NewCall;
            //m_Main.menu_File_NewDispatch.Command.Execute += new ExecutedRoutedEventHandler(cb_Executed);
            
            //cb.Command = 
            //ShortCutKey.NewCall.
            m_Main.menu_File_NewDispatch.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.Dispatch); };
            m_Main.menu_File_NewMessage.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.ShortMessage); };
            m_Main.menu_File_NewPosition.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.Position); };
            m_Main.menu_File_NewControl.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.Control); };
            m_Main.menu_File_NewJobTicket.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.JobTicker); };
            m_Main.menu_File_NewTracker.Click += delegate { m_Main.SubWindow.OpenCreateOperateWindow(OPType.Tracker); };

            //save
            m_Main.menu_File_Save.Click += delegate { m_Main.SaveWorkSpace(); };

            //Export
            m_Main.menu_File_Export.Click += delegate { };

            //Printf
            m_Main.menu_File_Print.Click += delegate { };

            //Exit
            m_Main.menu_File_Exit.Click += delegate { m_Main.Close(); };
        }

        private void ViewItemRegister()
        {
            //tools bar
            m_Main.menu_View_Tool_Base.Click += delegate { m_Main.View.ToolsBaseShow(!m_Main.menu_View_Tool_Base.IsChecked, true); };
            m_Main.menu_View_Tool_Fast.Click += delegate { m_Main.View.ToolsFastShow(!m_Main.menu_View_Tool_Fast.IsChecked, true); };
            m_Main.menu_View_Tool_Ctrl.Click += delegate { m_Main.View.ToolsCtrlShow(!m_Main.menu_View_Tool_Fast.IsChecked, true); };
            m_Main.menu_View_Tool_Help.Click += delegate { m_Main.View.ToolsHelpShow(!m_Main.menu_View_Tool_Fast.IsChecked, true); };

            //resource management
            m_Main.menu_View_Mgr_Org.Click += delegate {m_Main.View.ResrcMgrOrgShow(!m_Main.menu_View_Mgr_Org.IsChecked);};
            m_Main.menu_View_Mgr_Group.Click += delegate {m_Main.View.ResrcMgrGroupShow(!m_Main.menu_View_Mgr_Group.IsChecked);};
            m_Main.menu_View_Mgr_Employee.Click += delegate { m_Main.View.ResrcMgrEmployeeShow(!m_Main.menu_View_Mgr_Employee.IsChecked); };
            m_Main.menu_View_Mgr_Vehicle.Click += delegate { m_Main.View.ResrcMgrVehicleShow(!m_Main.menu_View_Mgr_Vehicle.IsChecked); };
            m_Main.menu_View_Mgr_Device.Click += delegate { m_Main.View.ResrcMgrDeviceShow(!m_Main.menu_View_Mgr_Device.IsChecked); };

            //navigation
            m_Main.menu_View_Nav_Map.Click += delegate { m_Main.View.NavMapShow(!m_Main.menu_View_Nav_Map.IsChecked); };
            m_Main.menu_View_Nav_Recording.Click += delegate { m_Main.View.NavRecordingShow(!m_Main.menu_View_Nav_Recording.IsChecked); };
            m_Main.menu_View_Nav_Report.Click += delegate { m_Main.View.NavReportShow(!m_Main.menu_View_Nav_Report.IsChecked); };

            //event message
            m_Main.menu_View_Event.Click += delegate { m_Main.View.EventMessageShow(!m_Main.menu_View_Event.IsChecked); };

            //message
            m_Main.menu_View_Msg_Alarm.Click += delegate { m_Main.View.MsgAlarmShow(!m_Main.menu_View_Msg_Alarm.IsChecked); };
            m_Main.menu_View_Msg_ShortMsg.Click += delegate { m_Main.View.MsgShortMsgShow(!m_Main.menu_View_Msg_ShortMsg.IsChecked); };
            m_Main.menu_View_Msg_Rx.Click += delegate { m_Main.View.MsgRxShow(!m_Main.menu_View_Msg_Rx.IsChecked); };
            m_Main.menu_View_Msg_Job.Click += delegate { m_Main.View.MsgJobShow(!m_Main.menu_View_Msg_Job.IsChecked); };
            m_Main.menu_View_Msg_Tracker.Click += delegate { m_Main.View.MsgTrackerShow(!m_Main.menu_View_Msg_Tracker.IsChecked); };
        }


        private void TargetItemRegister()
        {
            //check
            m_Main.menu_Target_Check.Click += delegate { };

            //monitor
            m_Main.menu_Target_Monitor.Click += delegate { };

            //call
            m_Main.menu_Target_Call.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Dispatch,m_Main.CurrentTraget, null)); };

            //message
            m_Main.menu_Target_Message.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.ShortMessage, m_Main.CurrentTraget, null)); };

            //position
            m_Main.menu_Target_Position.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_PositionCycle.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_PositionCSBK.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_PositionCSBKCycle.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_PositionEnh.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_PositionEnhCycle.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.menu_Target_Trail.Click += delegate { };

            //control
            m_Main.menu_Target_StartUp.Click += delegate { };
            m_Main.menu_Target_Shut.Click += delegate { };
            m_Main.menu_Target_Sleep.Click += delegate { };
            m_Main.menu_Target_Week.Click += delegate { };

            //job ticket
            m_Main.menu_Target_JobTicket.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.JobTicker, m_Main.CurrentTraget, null)); };
                           
        }

        private void RecordingItemRegister()
        {
            m_Main.menu_Rec_Update.Click += delegate { };
            m_Main.menu_Rec_Search.Click += delegate { };
            m_Main.menu_Rec_Replay.Click += delegate { };
            m_Main.menu_Rec_Import.Click += delegate { };
            m_Main.menu_Rec_Export.Click += delegate { };            
        }
        private void TrackerItemRegister()
        {
            m_Main.menu_Tracker_Task.Click += delegate { };
            m_Main.menu_Tracker_NewTask.Click += delegate { };
            m_Main.menu_Tracker_NewTimeTask.Click += delegate { };
            m_Main.menu_Tracker_Tracker.Click += delegate { };          
        }
        private void ToolsItemRegister()
        {
            m_Main.menu_Tools_NewContact.Click += delegate { m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Contact); };
            m_Main.menu_Tools_NewOperate.Click += delegate { m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Operate); };
            m_Main.menu_Tools_Export.Click += delegate { };
            m_Main.menu_Tools_Import.Click += delegate { };
            m_Main.menu_Tools_Potions.Click += delegate { };           
        }
        private void HelpItemRegister()
        {
            m_Main.menu_Help_Look.Click += delegate { };
            m_Main.menu_Help_About.Click += delegate { };      
        }
    }
}
