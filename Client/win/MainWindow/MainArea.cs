using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

using System.Windows.Controls.Primitives; 

namespace TrboX
{


    public class MainArea
    {
        private Main m_Main;

        public FastOperateWindow FastPanel;     

        private int SourceIndex = -1;          
        private delegate Point GetPositionDelegate(IInputElement element);  


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
            m_Main.lst_dispatch.View = (ViewBase)m_Main.FindResource("IconView");
                       
            m_Main.lst_dispatch.PreviewMouseLeftButtonDown += delegate(object sender, MouseButtonEventArgs e) { lst_dispatch_PreviewMouseLeftButtonDown(sender, e); };
            m_Main.lst_dispatch.Drop += delegate(object sender, DragEventArgs e) { lst_dispatch_Drop(sender, e); };

            m_Main.menu_FastListAddContact.Click +=delegate{m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Contact);};
            m_Main.menu_FastListAddOperate.Click +=delegate{ m_Main.SubWindow.OpenCreateFastWin(FastType.FastType_Operate); };

            m_Main.chk_MinIconView.Checked += delegate {
                if (null != m_Main.lst_dispatch) m_Main.lst_dispatch.View = (ViewBase)m_Main.FindResource("IconView");
            };
            m_Main.chk_MinIconView.Unchecked += delegate { 
                if (null != m_Main.lst_dispatch) m_Main.lst_dispatch.View = (ViewBase)m_Main.FindResource("ImageView"); 
            };

        }
       

        public void PastPanelPressed(object sender, RoutedEventArgs e)
        {
            FastOperate it = (FastOperate)((FastPanel)sender).DataContext;

            Point point = ((FastPanel)sender).TranslatePoint(new Point(0, 0), m_Main.lst_dispatch);

            object data = GetDataFromListBox(m_Main.lst_dispatch, point);
            if (data != null)
            {
                DragDrop.DoDragDrop(m_Main, data, DragDropEffects.Move);
            }
        }

        private static object GetDataFromListBox(ListBox source, Point point)
        {
            UIElement element = source.InputHitTest(point) as UIElement;
            if (element != null)
            {
                object data = DependencyProperty.UnsetValue;
                while (data == DependencyProperty.UnsetValue)
                {
                    data = source.ItemContainerGenerator.ItemFromContainer(element);
                    if (data == DependencyProperty.UnsetValue)
                    {
                        element = VisualTreeHelper.GetParent(element) as UIElement;
                    }
                    if (element == source)
                    {
                        return null;
                    }
                }
                if (data != DependencyProperty.UnsetValue)
                {
                    return data;
                }
            }
            return null;
        }

        private void lst_dispatch_Drop(object sender, DragEventArgs e)
        {
            int TargetIndex = GetCurrentIndex(new GetPositionDelegate(e.GetPosition));
            FastPanel.ChangePosition(SourceIndex, TargetIndex);
        }

        ListViewItem GetListViewItem(int index)
        {
            if (m_Main.lst_dispatch.ItemContainerGenerator.Status != GeneratorStatus.ContainersGenerated)
                return null;

            return m_Main.lst_dispatch.ItemContainerGenerator.ContainerFromIndex(index) as ListViewItem;
        }

        private int GetCurrentIndex(GetPositionDelegate getPosition)
        {
            int index = -1;
            for (int i = 0; i < m_Main.lst_dispatch.Items.Count; ++i)
            {
                ListViewItem item = GetListViewItem(i);
                if (item != null && IsMouseOverTarget(item, getPosition))
                {
                    index = i;
                    break;
                }
            }
            return index;
        }

        private bool IsMouseOverTarget(Visual target, GetPositionDelegate getPosition)
        {
            Rect bounds = VisualTreeHelper.GetDescendantBounds(target);
            Point mousePos = getPosition((IInputElement)target);
            return bounds.Contains(mousePos);
        }


        private void lst_dispatch_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            SourceIndex = GetCurrentIndex(e.GetPosition);

            if (SourceIndex < 0) return;

            m_Main.lst_dispatch.SelectedIndex = SourceIndex;
        }


        private void MapResourceRegister()
        {
            MyWebBrowse Map = new MyWebBrowse("file:///amap/index.html");
            m_Main.MyWebGrid.Children.Insert(0, Map);
        }
        
    }
}
