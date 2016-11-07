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
 
        private Dictionary<CMultMember, OperateWin> m_TargetWin = new Dictionary<CMultMember, OperateWin>();
       
        public NewWinMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;
        }

        public void OpenCreateOperateWindow(OPType type)
        {
            NewOperate m_CreateOperateWindow = new NewOperate();
            m_CreateOperateWindow.Owner = m_Main;
            
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
            COperate operate = new COperate(OPType.Dispatch, notify.Source.SingleToMult(), null);
            switch(notify.Type)
            {
                case NotifyType.Message:
                    operate.Type = OPType.ShortMessage;
                    break;
                case NotifyType.JobTicker:
                    operate.Type = OPType.JobTicker;
                    break;
                case NotifyType.Tracker:
                    operate.Type = OPType.Tracker;
                    break;
                default:
                    operate.Type = OPType.Dispatch;
                    break;
            }

            OpenOrCreateTragetWin(operate, notify);
        }

        private CMultMember IsExitsWindow(CMultMember member)
        {
            foreach(var item in m_TargetWin)
                if (item.Key.IsEqual(member)) return item.Key;
            return null;
        }
        public void OpenOrCreateTragetWin(COperate operate, CNotification notify = null)
        {
            //no target
            if ((null == operate) || (null == operate.Target) || (0 >= operate.Target.Target.Count)) return;

            CMultMember target = IsExitsWindow(operate.Target);
            if (null == target)
            {
                OperateWin newwin = new OperateWin(operate.Target);
                m_TargetWin.Add(operate.Target, newwin);
                target = operate.Target;
            }

            m_TargetWin[target].OwnerWin = m_Main;
            if (null != notify) m_TargetWin[target].CurrentNotify = notify;

            if (1 < m_TargetWin.Count)
            {
                double Top = 0, Left = 0;
                bool isshow = false;
                bool isposition = false;
                foreach (var item in m_TargetWin)
                {
                    if (target == item.Key)
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
                    m_TargetWin[target].Activate();
                }
                else if (isposition)
                {
                    m_TargetWin[target].Top = Top;
                    m_TargetWin[target].Left = Left;
                }
                else
                {
                    MINMAXINFO mmi = m_Main.MinMaxInfo;
                    m_TargetWin[target].Top = (m_TargetWin[target].Height + Top + 35 + 10 > mmi.ptMaxSize.y) ? 100 : Top + 35;
                    m_TargetWin[target].Left = (m_TargetWin[target].Width + Left + 35 + 30 > mmi.ptMaxSize.x) ? 300 : Left + 35;
                }
                m_TargetWin[target].Show();
            }
            else
            {
                m_TargetWin[target].WindowStartupLocation = WindowStartupLocation.CenterScreen;
                m_TargetWin[target].Show();
            }



            if (false == m_TargetWin[target].IsActive)
            {
                m_TargetWin[target].Activate();
            }

            m_TargetWin[target].SetOperateType(operate);
        }

        public void AddNotifyToOperateWin(CNotification notify)
        {
            CMultMember target = IsExitsWindow(notify.Source.SingleToMult());
            if(null != target)m_TargetWin[target].RxMessage(notify);            
        }


        //Create Fast Operate
        public void OpenCreateFastWin(FastType type)
        {
            newFast m_CreateFastOperateWindow = new newFast();
            m_CreateFastOperateWindow.Type = type;
            m_CreateFastOperateWindow.Owner = this.m_Main;
            m_CreateFastOperateWindow.ShowDialog();
        }

        public void UpdateOpWin(CMember targt, int mask, bool sta)
        {
            foreach (var item in m_TargetWin)
            {
                if(item.Key !=null && item.Key.Target != null && item.Key.Target.Count > 0)
                {
                    foreach(CMember mem in item.Key.Target)
                    {
                        if (mem.IsLike(targt))
                        {
                            m_TargetWin[item.Key].UpdateSta(mask, sta);
                            break;
                        }
                    }
                }
            }          
        }

        public void AddMessage(CMember targt, CHistory msg)
        {
            foreach (var item in m_TargetWin)
            {               
                if (item.Key != null && item.Key.Target != null && item.Key.Target.Count > 0)
                {
                    foreach (CMember mem in item.Key.Target)
                    {
                        if (mem.IsLike(targt))
                        {
                            m_TargetWin[item.Key].AddMessage(msg);
                            break;
                        }
                    }
                }
            }
        }
    }
}
