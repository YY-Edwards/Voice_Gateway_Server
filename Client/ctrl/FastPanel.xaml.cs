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
    public class FastPanelTypeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch((FastType)value)
            {
                case  FastType.FastType_Contact:
                    return "#FF97C5F7";
                case FastType.FastType_Operate:
                    return "#FFD3EA84";
                }
            return "#FFFFFFFF";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((string)value)
            {
                case  "#FF97C5F7":
                    return FastType.FastType_Contact;
                case  "#FFD3EA84": 
                    return FastType.FastType_Operate;
            }
            return null;
        }
    }

    public class FastPanelTitleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch((FastType)value)
            {
                case  FastType.FastType_Contact:
                    return "常用联系人";
                case FastType.FastType_Operate:
                    return "快速操作";
                }
            return "未定义面板";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((string)value)
            {
                case "常用联系人":
                    return FastType.FastType_Contact;
                case "快速操作": 
                    return FastType.FastType_Operate;
            }
            return null;
        }
    }

    public class ItemIconConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value[0])
            {
                case FastType.FastType_Contact:
                    if (null != value[1])
                    switch ((OrgItemType)value[1])
                    {
                        case OrgItemType.Type_Group:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/group.png"));
                        case OrgItemType.Type_Employee:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/contact.png"));
                        case OrgItemType.Type_Vehicle:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/vehicle.png"));
                        case OrgItemType.Type_Ride:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/ride.png"));
                        case OrgItemType.Type_Radio:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/radio.png"));
                    }
                    break;
                case FastType.FastType_Operate:
                    if (null != value[2])
                    switch ((OPType)value[2])
                    {
                        case OPType.Dispatch:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/dispatch.png"));
                        case OPType.ShortMessage:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/shortmessage.png"));
                        case OPType.Position:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/position.png"));
                        case OPType.Control:
                            return new BitmapImage(new Uri("pack://application:,,,/themes/resource/control.png"));
                    }
                    break;
            }

            return null;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FastPanelItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value)
            {
                case FastType.FastType_Contact:
                    return 0;
                case FastType.FastType_Operate:
                    return 1;
            }
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class NullHideConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value)
            {
                return Visibility.Collapsed;
            }
            else
            {
                return Visibility.Visible;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FalselHideConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (false == (bool)value)
            {
                return Visibility.Collapsed;
            }
            else
            {
                return Visibility.Visible;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class MonitorConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((OrgItemType)value)
            {
                case OrgItemType.Type_Group:
                    return Visibility.Collapsed;
            }

            return Visibility.Visible;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class ContactTypeItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((OrgItemType)value)
            {
                case OrgItemType.Type_Group:
                    return 0;
                case OrgItemType.Type_Employee:
                    return 1;
                case OrgItemType.Type_Vehicle:
                    return 2;
                case OrgItemType.Type_Ride:
                    return 3;
                case OrgItemType.Type_Radio:
                    return 4;
            }
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class OperateTypeItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((OPType)value)
            {
                case OPType.Dispatch:
                    return 0;
                case OPType.ShortMessage:
                    return 1;
                case OPType.Position:
                    return 2;
                case OPType.Control:
                    return 3;
                case OPType.JobTicker:
                    return 4;
            }
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


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

        ListView dragSource = null; 
        private void bdr_PanelTitle_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

            object data = GetDataFromListBox((ListBox)this.Parent, e.GetPosition(this));
            if (data != null)
            {

                DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
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
    }
}
