﻿using System;
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


using System.ComponentModel;

using System.Drawing.Design;
using System.Collections;
using System.ComponentModel.Design;
using System.Collections.ObjectModel;

using System.Globalization;

namespace TrboX
{

    public class BoolToOpConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return 0.3;

            if (value is bool)
            {
                if (false == (bool)value) return 0.3;
                return 1;
            }

            CMember target = value as CMember;
            if (MemberType.Group == target.Type) return 1;

            if (null != target.Radio)
                if (true == target.Radio.IsOnline) return 1;

            return 0.3;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    /// <summary>
    /// ComboButton.xaml 的交互逻辑
    /// </summary>
    public partial class ComboButton : UserControl
    {
        public ComboButton()
        {
            InitializeComponent();
            this.DataContext = this;

            this.Loaded += delegate
            {

                if (null!=items)
                if (items.Count > 0)
                {
                    combox.Visibility = Visibility.Visible;
                    button.Margin = new Thickness(0, 0, 14, 0);
                }
                else
                {
                    combox.Visibility = Visibility.Collapsed;
                    button.Margin = new Thickness(0);
                }

                //if (Normal != null)
                //{
                //    if (Hover == null) Hover = Normal;
                //    if (Pressed == null) Pressed = Normal;
                //    if (Disable == null) Disable = Normal;
                //}
                   
            };
        }

        public ImageSource Normal
        {
            get { return (ImageSource)GetValue(NormalProperty); }
            set { SetValue(NormalProperty, value); }
        }
        public static readonly DependencyProperty NormalProperty =
            DependencyProperty.Register("Normal", typeof(ImageSource), typeof(ComboButton));

        public ImageSource Hover
        {
            get { return (ImageSource)GetValue(HoverProperty); }
            set { SetValue(HoverProperty, value); }
        }
        public static readonly DependencyProperty HoverProperty =
            DependencyProperty.Register("Hover", typeof(ImageSource), typeof(ComboButton));

        public ImageSource Press
        {
            get { return (ImageSource)GetValue(PressProperty); }
            set { SetValue(PressProperty, value); }
        }
        public static readonly DependencyProperty PressProperty =
            DependencyProperty.Register("Press", typeof(ImageSource), typeof(ComboButton));

        public ImageSource Disable
        {
            get { return (ImageSource)GetValue(DisableProperty); }
            set { SetValue(DisableProperty, value); }
        }
        public static readonly DependencyProperty DisableProperty =
            DependencyProperty.Register("Disable", typeof(ImageSource), typeof(ComboButton));

        public new string Content
        {
            get { return (string)GetValue(ContentProperty); }
            set { SetValue(ContentProperty, value); }
        }

        public new static readonly DependencyProperty ContentProperty =
            DependencyProperty.Register("Content", typeof(string), typeof(ComboButton));

        private List<ComboBoxItem> items;
        public List<ComboBoxItem> Items
        {
            get {
                if (null == items) items =  new List<ComboBoxItem>();
                return items;               
                }
            set { items = Items;
            }
        }

        public int SelectedIndex
        {
            get
            {
                ControlTemplate baseWindowTemplate = combox.Template;
                ComboBox combo = (ComboBox)baseWindowTemplate.FindName("combo", combox);

                if (null == combo) return -1;
                else
                {
                    return combo.SelectedIndex;
                }
            }
            set {
                ControlTemplate baseWindowTemplate = combox.Template;
                ComboBox combo = (ComboBox)baseWindowTemplate.FindName("combo", combox);

                if (null != combo)
                {
                    combo.SelectedIndex = value;
                }
            }
        }


        public event RoutedEventHandler Click
        {
            add { AddHandler(ClickRoutedEvent, value); }
            remove { RemoveHandler(ClickRoutedEvent, value); }
        }

        public static readonly RoutedEvent ClickRoutedEvent =
            EventManager.RegisterRoutedEvent("Click", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ComboButton));

        public event RoutedEventHandler Selected
        {
            add { AddHandler(SelectedRoutedEvent, value); }
            remove { RemoveHandler(SelectedRoutedEvent, value); }
        }

        public static readonly RoutedEvent SelectedRoutedEvent =
            EventManager.RegisterRoutedEvent("Selected", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ComboButton));
        private void combo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (-1 == ((ComboBox)sender).SelectedIndex) return;

            //SelectedIndex = ((ComboBox)sender).SelectedIndex;
           
            RoutedEventArgs newEventArgs = new RoutedEventArgs(SelectedRoutedEvent);
            RaiseEvent(newEventArgs);
          
            ((ComboBox)sender).SelectedIndex = -1;
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {           
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClickRoutedEvent);
            RaiseEvent(newEventArgs);
        }
    }
}
