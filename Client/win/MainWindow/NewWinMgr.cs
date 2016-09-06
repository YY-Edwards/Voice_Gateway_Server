using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{

    public class NewWinMgr
    {
        private Main m_Main;

        private NewOperate m_CreateOperateWindow = new NewOperate();
        private newFast m_CreateFastOperateWindow = new newFast();
       
        public NewWinMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;
        }

        public void OpenCreateOperateWindow(OPType type)
        {
            switch(type)
            {
                case OPType.Dispatch:
                    m_CreateOperateWindow.rad_CreateCall.IsChecked = true;
                    break;
                case OPType.ShortMessage:
                    m_CreateOperateWindow.rad_CreateMsg.IsChecked = true;
                    break;
                case OPType.Control:
                    m_CreateOperateWindow.rad_CreateCtrl.IsChecked = true;
                    break;
                case OPType.Position:
                    m_CreateOperateWindow.rad_CreatePosition.IsChecked = true;
                    break;
                case OPType.JobTicker:
                    m_CreateOperateWindow.rad_CreateJob.IsChecked = true;
                    break;
                case OPType.Tracker:
                    m_CreateOperateWindow.rad_CreateTracker.IsChecked = true;
                    break;
                default:break;
            }

           m_CreateOperateWindow.ShowDialog();
        }
    }
}
