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
    public class NewWinMgr
    {
        private Main m_Main;

        private NewOperate m_CreateOperateWindow = new NewOperate();
        private newFast m_CreateFastOperateWindow = new newFast();
        private Dictionary<int, OperateWin> m_TargetWin = new Dictionary<int, OperateWin>();
       
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

        public void OpenOrCreateTragetWin(COrganization target, OPType op)
        {
            if(!m_TargetWin.ContainsKey(target.index))
            {
                OperateWin newwin = new OperateWin(target.target);
                m_TargetWin.Add(target.index, newwin);
            }

            m_TargetWin[target.index].Owner = m_Main;

            m_TargetWin[target.index].Show();
            if (false == m_TargetWin[target.index].IsActive)
            {
                m_TargetWin[target.index].Activate();
            }            

            m_TargetWin[target.index].SetOperateType(op);
        }

        public void AddNotifyToOperateWin(CNotification notify)
        {                         
            foreach (var item in m_TargetWin)
            {
                if (true == CRelationShipObj.Compare(notify.Source, item.Value.Target))
                    item.Value.RxMessage(notify);
            }
        }
    }
}
