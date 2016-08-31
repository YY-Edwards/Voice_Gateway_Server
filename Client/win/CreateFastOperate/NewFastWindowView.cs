using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    class NewFastWindowView
    {
        newFast m_Win;
        public NewFastWindowView(newFast win)
        {
            if (null == win) return;
            m_Win = win;

            m_Win.bdr_MaskCreateTab.ClipToBounds = true;

            CreateTabChange();
        }

        private void CreateTabChange()
        {
            m_Win.rad_CreateContact.Checked += delegate
            {
                m_Win.tab_CreatFast.SelectedIndex = 0;
            };

            m_Win.rad_CreateGroup.Checked += delegate
            {
                m_Win.tab_CreatFast.SelectedIndex = 1;
            };

            m_Win.rad_CreateFastOperate.Checked += delegate
            {
                m_Win.tab_CreatFast.SelectedIndex = 2;
            }; 
        }
    }
}
