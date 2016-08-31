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
            switch ((int)value)
            {
                case 0:
                    return FastType.FastType_Contact;
                case 1:
                    return FastType.FastType_Operate;
            }
            return null;
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
        }
        public FastOperate PanelItem 
        {
            get { return (FastOperate)GetValue(PanelItemProperty); }
            set { SetValue(PanelItemProperty, value); }  
        }
        public static readonly DependencyProperty PanelItemProperty =
            DependencyProperty.Register("PanelItem", typeof(FastOperate), typeof(FastPanel), new PropertyMetadata(null));
        

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
       
    }
}
