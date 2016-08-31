using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    class OpView
    {
        OperateWin m_opWin = null;
        public OpView(OperateWin win)
        {
            if (null == win) return;
            m_opWin = win;

            m_opWin.grdspl_Hositry.PreviewMouseLeftButtonDown += delegate
            {
                m_opWin.grd_OperateWin.ColumnDefinitions[0].MaxWidth = m_opWin.grd_OperateWin.ActualWidth - m_opWin.grd_OperateWin.ColumnDefinitions[1].MinWidth;
                m_opWin.grd_OperateWin.ColumnDefinitions[1].MaxWidth = m_opWin.grd_OperateWin.ActualWidth - m_opWin.grd_OperateWin.ColumnDefinitions[0].MinWidth;
            };

            m_opWin.grdspl_Hositry.PreviewMouseLeftButtonUp += delegate
            {
                m_opWin.grd_OperateWin.ColumnDefinitions[0].MaxWidth = double.PositiveInfinity;
                m_opWin.grd_OperateWin.ColumnDefinitions[1].MaxWidth = double.PositiveInfinity;
            };

            m_opWin.grdspl_Operate.PreviewMouseLeftButtonDown += delegate
            {
                m_opWin.grd_Operate.RowDefinitions[0].MaxHeight = m_opWin.grd_OperateWin.ActualHeight - m_opWin.grd_Operate.RowDefinitions[1].MinHeight;
                m_opWin.grd_Operate.RowDefinitions[1].MaxHeight = m_opWin.grd_OperateWin.ActualHeight - m_opWin.grd_Operate.RowDefinitions[0].MinHeight;

            };

            m_opWin.grdspl_Operate.PreviewMouseLeftButtonUp += delegate
            {
                m_opWin.grd_Operate.RowDefinitions[0].MaxHeight = double.PositiveInfinity;
                m_opWin.grd_Operate.RowDefinitions[1].MaxHeight = double.PositiveInfinity;
            };

        }
    }
}
