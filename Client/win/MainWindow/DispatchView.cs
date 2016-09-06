using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX 
{
    public class CDispatchView
    {

        Main m_Win;
        NewOperate m_newOperateWin;

        public CDispatchView(Main win)
        {
              if (null == win) return;
              m_Win = win;

              m_newOperateWin = new NewOperate();

              //MenuFileReg();
              ToolsNewReg();

              MenuTargetReg();
        }


        private void MenuTargetReg()
        {

        }

        private void ToolsNewReg()
        {
            m_Win.cmb_Tool_New.Click += delegate
            {
                m_newOperateWin.rad_CreateCall.IsChecked = true;
                m_newOperateWin.ShowDialog();
            };

            m_Win.cmb_Tool_New.Selected += delegate
            {
                switch (m_Win.cmb_Tool_New.SelectedIndex)
                {
                    case 0:
                        m_newOperateWin.rad_CreateMsg.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 1:
                        m_newOperateWin.rad_CreatePosition.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 2:
                        m_newOperateWin.rad_CreateCtrl.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 3:
                        m_newOperateWin.rad_CreateJob.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    case 4:
                        m_newOperateWin.rad_CreateTracker.IsChecked = true;
                        m_newOperateWin.ShowDialog();
                        break;
                    default:
                        break;
                }
            };

        }


        //m_mainWin.exp_New.PreviewMouseLeftButtonUp += delegate
        //{
        //    //left-top(width, 0) when FlowDirection is Right to left
        //    Point ep = m_mainWin.exp_New.TranslatePoint(new Point(m_mainWin.exp_New.ActualWidth, 0), (UIElement)m_mainWin);
        //    Point mp = Mouse.GetPosition((UIElement)m_mainWin);

        //    //doglle button width :20
        //    if ((mp.X < ep.X + 20) || (mp.X > ep.X + m_mainWin.exp_New.ActualWidth) || (mp.Y < ep.Y) || (mp.Y > ep.Y + m_mainWin.exp_New.ActualHeight))
        //    {
        //        m_newOperateWin.rad_CreateCall.IsChecked = true;
        //        m_newOperateWin.tab_NewType.SelectedIndex = 0;
        //        m_newOperateWin.ShowDialog(); 
        //    }

        //};

        //m_mainWin.lst_Tool_New.SelectionChanged += delegate
        //{

        //    m_mainWin.exp_New.IsExpanded = false;

        //   m_newOperateWin.tab_NewType.SelectedIndex = m_mainWin.lst_Tool_New.SelectedIndex;

        //    switch (m_mainWin.lst_Tool_New.SelectedIndex)
        //    {
        //        case 0:
        //            m_newOperateWin.rad_CreateCall.IsChecked = true;
        //            m_newOperateWin.ShowDialog();

        //            break;
        //        case 1:
        //            m_newOperateWin.rad_CreateMsg.IsChecked = true;
        //            m_newOperateWin.ShowDialog();                      
        //            break;
        //        case 2:
        //            m_newOperateWin.rad_CreatePosition.IsChecked = true;
        //            m_newOperateWin.ShowDialog();                      
        //            break;
        //        case 3:
        //            m_newOperateWin.rad_CreateCtrl.IsChecked = true;
        //            m_newOperateWin.ShowDialog();
        //            break;
        //        case 4:
        //            m_newOperateWin.rad_CreateJob.IsChecked = true;
        //            m_newOperateWin.ShowDialog();
        //            break;
        //        case 5:
        //            m_newOperateWin.rad_CreateTracker.IsChecked = true;
        //            m_newOperateWin.ShowDialog();
        //            break;
        //        default:
        //            break;
        //    }

        //    m_mainWin.lst_Tool_New.SelectedIndex = -1;

        //};

    }
}
