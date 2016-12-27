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

namespace TrboX
{
    /// <summary>
    /// IconBtn.xaml 的交互逻辑
    /// </summary>
    public partial class IconBtn : UserControl
    {
        public IconBtn()
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
            };
        }

        public bool IsPressed
        {
            get { return mybtn.IsPressed; }
        }

        public static readonly DependencyProperty IsPressedProperty =
           DependencyProperty.Register("IsPressed", typeof(bool), typeof(IconBtn));

        public ImageSource Normal
        {
            get { return (ImageSource)GetValue(NormalProperty); }
            set { SetValue(NormalProperty, value); }
        }
        public static readonly DependencyProperty NormalProperty =
            DependencyProperty.Register("Normal", typeof(ImageSource), typeof(IconBtn));

        public ImageSource Hover
        {
            get { return (ImageSource)GetValue(HoverProperty); }
            set { SetValue(HoverProperty, value); }
        }
        public static readonly DependencyProperty HoverProperty =
            DependencyProperty.Register("Hover", typeof(ImageSource), typeof(IconBtn));

        public ImageSource Press
        {
            get { return (ImageSource)GetValue(PressProperty); }
            set { SetValue(PressProperty, value); }
        }
        public static readonly DependencyProperty PressProperty =
            DependencyProperty.Register("Press", typeof(ImageSource), typeof(IconBtn));

        public ImageSource Disable
        {
            get { return (ImageSource)GetValue(DisableProperty); }
            set { SetValue(DisableProperty, value); }
        }
        public static readonly DependencyProperty DisableProperty =
            DependencyProperty.Register("Disable", typeof(ImageSource), typeof(IconBtn));

        public event RoutedEventHandler Click
        {
            add { AddHandler(ClickRoutedEvent, value); }
            remove { RemoveHandler(ClickRoutedEvent, value); }
        }

        public static readonly RoutedEvent ClickRoutedEvent =
            EventManager.RegisterRoutedEvent("Click", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(IconBtn));


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClickRoutedEvent);
            RaiseEvent(newEventArgs);
        }

    }
}
