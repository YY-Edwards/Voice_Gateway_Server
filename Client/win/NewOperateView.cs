using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    class NewOperateView
    {
        NewOperate m_Win = null;
        public NewOperateView(NewOperate win)
        {
            if (null == win) return;
            m_Win = win;
            m_Win.bdr_MaskCreateTab.ClipToBounds = true;

            CreateTabChange();
        }

        private void CreateTabChange()
        {
            m_Win.rad_CreateCall.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 0;
            };

            m_Win.rad_CreateMsg.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 1;
            };

            m_Win.rad_CreatePosition.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 2;
            };

            m_Win.rad_CreateCtrl.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 3;
            };

            m_Win.rad_CreateJob.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 4;
            };
            m_Win.rad_CreateTracker.Checked += delegate
            {
                m_Win.tab_NewType.SelectedIndex = 5;
            };

        }
    }
}
