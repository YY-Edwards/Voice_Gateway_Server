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


    public class MainArea
    {
        private Main m_Main;
        public FastOperateWindow FastPanel;

        public MainArea(Main win)
        {
            if (null == win) return;
            m_Main = win;

            DispatchResourceRegister();
            MapResourceRegister();
        }

        private void DispatchResourceRegister()
        {
            FastPanel = new FastOperateWindow(m_Main);
            m_Main.lst_dispatch.View = (ViewBase)m_Main.FindResource("ImageView");
        }
        private void MapResourceRegister()
        {
            MyWebBrowse Map = new MyWebBrowse("file:///E:/Home/Projects/TrboX 3.0/Prj/TrboX/Debug/amap/index.html");
            m_Main.MyWebGrid.Children.Insert(0, Map);     
        }

        
    }
}
