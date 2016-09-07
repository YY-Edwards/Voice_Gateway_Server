using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Globalization;

namespace TrboX
{
    /// <summary>
    /// FastPanel.xaml 的交互逻辑
    /// </summary>
    public partial class FastPanel : UserControl
    {
        public FastPanel()
        {
            InitializeComponent();
            updatecyclelist(null, null);
        }
        public FastOperate PanelItem 
        {
            get { return (FastOperate)GetValue(PanelItemProperty); }
            set { SetValue(PanelItemProperty, value); }  
        }

        public static readonly DependencyProperty PanelItemProperty =
            DependencyProperty.Register("PanelItem", typeof(FastOperate), typeof(FastPanel), new UIPropertyMetadata(null, delegate(DependencyObject obj, DependencyPropertyChangedEventArgs e){

                if (OPType.Position == ((FastOperate)e.NewValue).m_Operate.type)
                {
                    var element = obj as FastPanel;
                    List<double> cyclelist = CPosition.UpdateCycleList(((FastOperate)e.NewValue).m_Operate.position.iscsbk, ((FastOperate)e.NewValue).m_Operate.position.isenh);
                    element.cmb_CycleLst.Items.Clear();
                    foreach (double cycle in cyclelist)
                        element.cmb_CycleLst.Items.Add(new ComboBoxItem() {Content = cycle.ToString() + "s", Tag = cycle });
                }
            }));


        public event RoutedEventHandler Closing
        {
            add { AddHandler(ClosingRoutedEvent, value); }
            remove { RemoveHandler(ClosingRoutedEvent, value); }
        }

        public static readonly RoutedEvent ClosingRoutedEvent =
            EventManager.RegisterRoutedEvent("Closing", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void btn_Close_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClosingRoutedEvent);
            RaiseEvent(newEventArgs);
        }

        private void teste_Selected(object sender, RoutedEventArgs e)
        {
            int i = 0;
        }

        private void teste_Click(object sender, RoutedEventArgs e)
        {
            int i = 0;
        }

        private void updatecyclelist(object sender, RoutedEventArgs e)
        {
            List<double> cyclelist = CPosition.UpdateCycleList((bool)chk_CSBK.IsChecked, (bool)chk_Enh.IsChecked);
            cmb_CycleLst.Items.Clear();
            foreach (double cycle in cyclelist)
                cmb_CycleLst.Items.Add(new ComboBoxItem() { Content = cycle.ToString() + "s", Tag = cycle });
        }


        public event RoutedEventHandler TitlePressed
        {
            add { AddHandler(TitlePressedRoutedEvent, value); }
            remove { RemoveHandler(TitlePressedRoutedEvent, value); }
        }

        public static readonly RoutedEvent TitlePressedRoutedEvent =
           EventManager.RegisterRoutedEvent("TitlePressed", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(FastOperate));

        private void bdr_PanelTitle_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(TitlePressedRoutedEvent);
            RaiseEvent(newEventArgs);
        }

    }
}
