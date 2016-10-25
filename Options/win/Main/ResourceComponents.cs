using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    public class ResourceComponents
    {
        private Main m_Main;
        private TargetMgr m_TargetMgr = new TargetMgr();
        private CTargetRes m_TargetList = new CTargetRes();

        public ResourceComponents(Main main)
        {
            if (null != main) m_Main = main;
            m_TargetMgr.UpdateTragetList();
            m_TargetList = m_TargetMgr.TargetList;

            SetGroupResource();
        }

        public void SetResource()
        {

        }
        private void SetGroupResource()
        {
            m_Main.SettingComponents.FileGroupList(m_TargetMgr.g_GroupList);
        }
    }
}
