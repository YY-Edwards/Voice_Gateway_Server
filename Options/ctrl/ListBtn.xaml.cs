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
    public class SpuerConv : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool && (bool)value == true) return Visibility.Visible;
            return Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }
    /// <summary>
    /// ListBtn.xaml 的交互逻辑
    /// </summary>
    public partial class ListBtn : UserControl
    {
        public ListBtn()
        {
            InitializeComponent();

            Loaded += delegate
            {
                if (Normal != null)
                {
                    if (Hover == null) Hover = Normal;
                    if (Press == null) Press = Normal;
                    if (Disable == null) Disable = Normal;                 
                }
                else
                {
                    ControlTemplate baseWindowTemplate = combox.Template;
                    IconBtn icon = (IconBtn)baseWindowTemplate.FindName("btn_Icon", combox);
                    if (null != icon) icon.Visibility = Visibility.Collapsed;
                }

                if (Content == "" || Content == null)
                {
                    ControlTemplate baseWindowTemplate = combox.Template;
                    Button con = (Button)baseWindowTemplate.FindName("btn_Content", combox);
                    if (null != con) con.Visibility = Visibility.Collapsed;
                }                
            };
        }

        public bool IsSuper
        {
            get { return (bool)GetValue(IsSuperProperty); }
            set { SetValue(IsSuperProperty, value); }
        }
        public static readonly DependencyProperty IsSuperProperty =
            DependencyProperty.Register("IsSuper", typeof(bool), typeof(ListBtn));

        public double VOffset
        {
            get { return (double)GetValue(VOffsetProperty); }
            set { SetValue(VOffsetProperty, value); }
        }
        public static readonly DependencyProperty VOffsetProperty =
            DependencyProperty.Register("VOffset", typeof(double), typeof(ListBtn));

        public double HOffset
        {
            get { return (double)GetValue(HOffsetProperty); }
            set { SetValue(HOffsetProperty, value); }
        }
        public static readonly DependencyProperty HOffsetProperty =
            DependencyProperty.Register("HOffset", typeof(double), typeof(ListBtn));

        public new string Content
        {
            get { return (string)GetValue(ContentProperty); }
            set { SetValue(ContentProperty, value); }
        }

        public new static readonly DependencyProperty ContentProperty =
            DependencyProperty.Register("Content", typeof(string), typeof(ListBtn));


        public ImageSource Normal
        {
            get { return (ImageSource)GetValue(NormalProperty); }
            set { SetValue(NormalProperty, value); }
        }
        public static readonly DependencyProperty NormalProperty =
            DependencyProperty.Register("Normal", typeof(ImageSource), typeof(ListBtn));

        public ImageSource Hover
        {
            get { return (ImageSource)GetValue(HoverProperty); }
            set { SetValue(HoverProperty, value); }
        }
        public static readonly DependencyProperty HoverProperty =
            DependencyProperty.Register("Hover", typeof(ImageSource), typeof(ListBtn));

        public ImageSource Press
        {
            get { return (ImageSource)GetValue(PressProperty); }
            set { SetValue(PressProperty, value); }
        }
        public static readonly DependencyProperty PressProperty =
            DependencyProperty.Register("Press", typeof(ImageSource), typeof(ListBtn));

        public ImageSource Disable
        {
            get { return (ImageSource)GetValue(DisableProperty); }
            set { SetValue(DisableProperty, value); }
        }
        public static readonly DependencyProperty DisableProperty =
            DependencyProperty.Register("Disable", typeof(ImageSource), typeof(ListBtn));

        private List<ComboBoxItem> items;
        public List<ComboBoxItem> Items
        {
            get
            {
                if (null == items) items = new List<ComboBoxItem>();
                return items;
            }
            set
            {
                items = Items;
            }
        }


        public int SelectedIndex
        {
            get
            {
               return combox.SelectedIndex;               
            }
            set
            {
                combox.SelectedIndex = value;                
            }
        }

        public event RoutedEventHandler Click
        {
            add { AddHandler(ClickRoutedEvent, value); }
            remove { RemoveHandler(ClickRoutedEvent, value); }
        }

        public static readonly RoutedEvent ClickRoutedEvent =
            EventManager.RegisterRoutedEvent("Click", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ListBtn));

        public event RoutedEventHandler Selected
        {
            add { AddHandler(SelectedRoutedEvent, value); }
            remove { RemoveHandler(SelectedRoutedEvent, value); }
        }

        public static readonly RoutedEvent SelectedRoutedEvent =
            EventManager.RegisterRoutedEvent("Selected", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ListBtn));

        private void combox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (-1 == ((ComboBox)sender).SelectedIndex) return;

            SelectedIndex = ((ComboBox)sender).SelectedIndex;

            RoutedEventArgs newEventArgs = new RoutedEventArgs(SelectedRoutedEvent);
            RaiseEvent(newEventArgs);

            ((ComboBox)sender).SelectedIndex = -1;
        }

        private void btn_Content_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClickRoutedEvent);
            RaiseEvent(newEventArgs);
        }

    }
}
