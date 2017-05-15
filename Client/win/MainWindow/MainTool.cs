using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
namespace TrboX 
{
    public class MainTool
    {
        private Main m_Main;

        public MainTool(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_Main.lbtn_New.Click += delegate {
                m_Main.SubWindow.OpenCreateOperateWindow(OPType.Dispatch); 
            };
            m_Main.lbtn_New.Selected += delegate { 
                switch(m_Main.lbtn_New.SelectedIndex)
                {
                    case 0:m_Main.SubWindow.OpenCreateOperateWindow(OPType.Dispatch);break;
                    case 1:m_Main.SubWindow.OpenCreateOperateWindow(OPType.ShortMessage);break;
                    case 2:m_Main.SubWindow.OpenCreateOperateWindow(OPType.Position); break;
                    case 3: m_Main.SubWindow.OpenCreateOperateWindow(OPType.Control); break;
                    case 4: m_Main.SubWindow.OpenCreateOperateWindow(OPType.JobTicker); break;
                    case 5: m_Main.SubWindow.OpenCreateOperateWindow(OPType.Tracker); break;
                    default:break;
                }
            };

            m_Main.btn_Save.Click += delegate { m_Main.SaveWorkSpace(); };


            m_Main.btn_Tool_NewContact.Click += delegate { m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Contact); };
            m_Main.btn_Tool_NewFastOperate.Click += delegate { m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Operate); };

            m_Main.btn_Tool_PTT.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Dispatch, m_Main.CurrentTraget, null)); };
            m_Main.btn_Tool_Msg.Click += delegate {  m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.ShortMessage, m_Main.CurrentTraget, null)); };
            m_Main.btn_Tool_Position.Click += delegate {  m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, m_Main.CurrentTraget, null)); };
            m_Main.btn_Tool_Job.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.JobTicker, m_Main.CurrentTraget, null)); };

             m_Main.btn_Tool_Check.Click += delegate { 
                 new COperate(OPType.Control, m_Main.CurrentTraget, new CControl(){Type = ControlType.Check}).Exec();
                 m_Main.EventList.AddEvent("提示：在线检测（" + m_Main.CurrentTraget.NameInfo  + ")"); };
            m_Main.btn_Tool_Monitor.Click += delegate {   
                new COperate(OPType.Control, m_Main.CurrentTraget, new CControl() { Type = ControlType.Monitor }).Exec();
                m_Main.EventList.AddEvent("提示：远程监听（" + m_Main.CurrentTraget.NameInfo + ")"); };
            m_Main.btn_Tool_Start.Click += delegate
            {
                new COperate(OPType.Control, m_Main.CurrentTraget, new CControl() { Type = ControlType.StartUp }).Exec();
                m_Main.EventList.AddEvent("提示：遥开（" + m_Main.CurrentTraget.NameInfo + ")");
            };
            m_Main.btn_Tool_Shut.Click += delegate
            {
                new COperate(OPType.Control, m_Main.CurrentTraget, new CControl() { Type = ControlType.ShutDown }).Exec();
                m_Main.EventList.AddEvent("提示：遥毙（" + m_Main.CurrentTraget.NameInfo + ")");
            };
            m_Main.btn_Tool_Sleep.Click += delegate
            {
                new COperate(OPType.Control, m_Main.CurrentTraget, new CControl() { Type = ControlType.Sleep }).Exec();
                m_Main.EventList.AddEvent("提示：遥晕（" + m_Main.CurrentTraget.NameInfo + ")");
            };


            m_Main.btn_Help.Click += delegate { m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Dispatch, m_Main.CurrentTraget, null)); };
            m_Main.btn_About.Click += delegate
            {
                About aboutwin = new About();
                aboutwin.ShowDialog();
            };
        }

        public void OperateShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Base.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Base.Visibility = Visibility.Collapsed;
        }

        public void OperateEnable(bool disable = false)
        {
            if (false == disable)
                m_Main.bdr_Tool_Base.IsEnabled = true;
            else
                m_Main.bdr_Tool_Base.IsEnabled = false;
        }

        public void FastShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Fast.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Fast.Visibility = Visibility.Collapsed;
        }

        public void ControlShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Ctrl.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Ctrl.Visibility = Visibility.Collapsed;
        }

        public void ControlEnable(bool disable = false)
        {
            if (false == disable)
                m_Main.bdr_Tool_Ctrl.IsEnabled = true;
            else
                m_Main.bdr_Tool_Ctrl.IsEnabled = false;
        }

        public void HelpShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Help.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Help.Visibility = Visibility.Collapsed;
        }
    }
}
