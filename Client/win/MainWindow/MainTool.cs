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
