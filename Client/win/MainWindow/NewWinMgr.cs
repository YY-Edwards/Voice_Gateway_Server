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


        public void OpenOrCreateTragetWin(CNotification notify)
        {
            COperate op = new COperate(OPType.Dispatch, null);
            switch(notify.Type)
            {
                case NotifyType.Message:
                    op.type = OPType.ShortMessage;
                    break;
                case NotifyType.JobTicker:
                    op.type = OPType.JobTicker;
                    break;
                case NotifyType.Tracker:
                    op.type = OPType.Tracker;
                    break;
                default:
                    op.type = OPType.Dispatch;
                    break;
            }
            
            
            foreach (var group in m_Main.ResrcMgr.m_Target.OrgList)
            {               
                if (true == CRelationShipObj.Compare(notify.Source, group.Key.target))
                {
                    OpenOrCreateTragetWin(group.Key, op, notify);
                    return;
                }
                
                foreach (var item in group.Value)
                {
                    if (true == CRelationShipObj.Compare(notify.Source, item.target))
                    {
                        OpenOrCreateTragetWin(item, op, notify);
                        return;
                    }
                }
            } 
        }
        public void OpenOrCreateTragetWin(COrganization target, COperate op, CNotification notify = null)
        {
            if(!m_TargetWin.ContainsKey(target.index))
            {
                OperateWin newwin = new OperateWin( target.target);
                m_TargetWin.Add(target.index, newwin);
            }

            m_TargetWin[target.index].OwnerWin = m_Main;

            if (null != notify) m_TargetWin[target.index].CurrentNotify = notify;

            if (1 <m_TargetWin.Count)
            {
                double Top = 0, Left = 0;
                bool isshow = false;
                bool isposition = false;
                foreach (var item in m_TargetWin)
                {
                    if (target.index == item.Key)
                    {
                        if (Visibility.Visible == item.Value.Visibility) isshow = true;
                        else continue;
                    }
                   
                    Top = item.Value.Top;
                    Left = item.Value.Left;

                    if (Visibility.Visible != item.Value.Visibility)
                    {
                        isposition = true;
                        break;
                    }
                }

                if (isshow)
                {
                    m_TargetWin[target.index].Activate();
                }
                else if (isposition)
                {
                    m_TargetWin[target.index].Top = Top;
                    m_TargetWin[target.index].Left = Left;
                }
                else
                {
                    MINMAXINFO mmi = m_Main.MinMaxInfo;
                    m_TargetWin[target.index].Top = (m_TargetWin[target.index].Height + Top + 35 + 10 > mmi.ptMaxSize.y) ? 100 : Top + 35;
                    m_TargetWin[target.index].Left = (m_TargetWin[target.index].Width + Left + 35 + 30 > mmi.ptMaxSize.x) ? 300 : Left +35;
                }
                m_TargetWin[target.index].Show();
            }
            else
            {
                m_TargetWin[target.index].WindowStartupLocation = WindowStartupLocation.CenterScreen;
                m_TargetWin[target.index].Show();
            }

           

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


        //Create Fast Operate
        public void OpenCreateFastWin(FastType type)
        {
            //m_CreateFastOperateWindow.FastType = type;
            m_CreateFastOperateWindow.Show();
        }
    }
}
