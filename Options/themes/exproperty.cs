using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace TrboX
{

    public class CExIconTemplate
    {
        public string Content { set; get; }
        public string Icon { set; get; }
        public Rect Range { set; get; }

        public Thickness Thick { set; get; }
    }
    public class CExIcon : DependencyObject
    {


        public String Content
        {
            set { SetValue(ContentProperty, value); }
            get { return (string)GetValue(ContentProperty); }
        }
        public static void SetContent(DependencyObject obj, string value)
        {
            obj.SetValue(ContentProperty, value);
        }

        public static string GetContent(DependencyObject obj)
        {
            return (string)obj.GetValue(ContentProperty);
        }

        public static readonly DependencyProperty ContentProperty =
            DependencyProperty.RegisterAttached("Content", typeof(string), typeof(CExIcon), new UIPropertyMetadata("", OnContenChanged));
        public static void OnContenChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {
            var element = obj as Button;
            if (element != null)
            {
                CExIconTemplate item = (CExIconTemplate)element.Content;
                if (null == item) item = new CExIconTemplate();
                item.Content = (string)e.NewValue;
                element.Content = item;
            }
        }



        public String Icon
        {
            set { SetValue(IconProperty, value); }
            get { return (string)GetValue(IconProperty); }
        }
        public static void SetIcon(DependencyObject obj, string value)
        {
            obj.SetValue(IconProperty, value);
        }
        public static string GetIcon(DependencyObject obj)
        {
            return (string)obj.GetValue(IconProperty);
        }

        // Using a DependencyProperty as the backing store for Icon.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IconProperty =
            DependencyProperty.RegisterAttached("Icon", typeof(string), typeof(CExIcon), new UIPropertyMetadata("", OnIconChanged));
        public static void OnIconChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {
            var element = obj as Button;
            if (element != null)
            {
                CExIconTemplate item = (CExIconTemplate)element.Content;
                if (null == item) item = new CExIconTemplate();
                item.Icon = (string)e.NewValue;
                element.Content = item;
            }
        }

        public Rect Range
        {
            set { SetValue(RangeProperty, value); }
            get { return (Rect)GetValue(RangeProperty); }
        }

        public static Rect GetRange(DependencyObject obj)
        {
            return (Rect)obj.GetValue(RangeProperty);
        }

        public static void SetRange(DependencyObject obj, Rect value)
        {
            obj.SetValue(RangeProperty, value);
        }




        public static readonly DependencyProperty RangeProperty =
        DependencyProperty.RegisterAttached("Range", typeof(Rect), typeof(CExIcon), new UIPropertyMetadata(new Rect(0, 0, 0, 0), OnRangeChanged));


        public static void OnRangeChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {
            var element = obj as Button;
            if (element != null)
            {
                CExIconTemplate item = (CExIconTemplate)element.Content;
                if (null == item) item = new CExIconTemplate();

                item.Range = (Rect)e.NewValue;
                item.Thick = new Thickness(-item.Range.Left, -item.Range.Top, 0, 0);
                
                element.Content = item;
            }
        }
    }
}
