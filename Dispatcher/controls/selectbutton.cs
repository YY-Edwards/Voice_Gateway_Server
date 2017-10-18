using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;


using System.Collections;
using System.ComponentModel;
using System.Windows.Interop;
using System.Runtime.InteropServices;

using Sigmar.Controls;

namespace Dispatcher.Controls
{
    /// <summary>
    /// 按照步骤 1a 或 1b 操作，然后执行步骤 2 以在 XAML 文件中使用此自定义控件。
    ///
    /// 步骤 1a) 在当前项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:Dispatcher.controls"
    ///
    ///
    /// 步骤 1b) 在其他项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:Dispatcher.controls;assembly=Dispatcher.controls"
    ///
    /// 您还需要添加一个从 XAML 文件所在的项目到此项目的项目引用，
    /// 并重新生成以避免编译错误: 
    ///
    ///     在解决方案资源管理器中右击目标项目，然后依次单击
    ///     “添加引用”->“项目”->[浏览查找并选择此项目]
    ///
    ///
    /// 步骤 2)
    /// 继续操作并在 XAML 文件中使用控件。
    ///
    ///     <MyNamespace:selectbutton/>
    ///
    /// </summary>
    public class SelectButton : Control, INotifyPropertyChanged
    {
        private CheckBox check = null;
        static SelectButton()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(SelectButton), new FrameworkPropertyMetadata(typeof(SelectButton)));
        }

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;
            check = (CheckBox)baseTemplate.FindName("checkbox", this);

            Icon icon = (Icon)baseTemplate.FindName("icon", this);
            ListBox list = (ListBox)baseTemplate.FindName("list", this);
            

            
            if(icon != null)
            {
                icon.Click += new RoutedEventHandler(OnButtonClick);
            }
            if(list != null)
            {
                list.SelectionChanged += new SelectionChangedEventHandler(OnListSelectedChanged);
            }
        }

       

        public SelectButton()
        {
            this.Loaded += delegate
            {
                HwndSource hs = PresentationSource.FromVisual(this) as HwndSource;
                if (hs != null)hs.AddHook(new HwndSourceHook(WndProc));
                
                
                if (Image != null)
                {
                    if (HoverImage == null) HoverImage = Image;
                    if (PressedImage == null) PressedImage = Image;
                    if (DisableImage == null) DisableImage = Image;
                }
            };
 
        }
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0201://WM_LBUTTON_DWON
                case 0x0204://WM_RBUTTON_DWON
                    OnMousePrssed((int)wParam, (int)lParam);               
                    break;

                default: break;
            }
            return (System.IntPtr)0;
        }

        private void OnMousePrssed(int w, int l)
        {
            
            if (check == null || check.IsChecked == false) return;

            Window window = Window.GetWindow(this);
            Point point = this.TransformToAncestor(window).Transform(new Point(0, 0));

            double mouseX = (l & 0xffff);
            double mouseY = ((l >> 16) & 0xffff);

            if (mouseX < point.X || mouseX > point.X + this.ActualWidth) check.IsChecked = false;
            if (mouseY < point.Y || mouseY > point.Y + this.ActualHeight) check.IsChecked = false;
        }


        private bool _isopen = false;
        public bool IsOpen { get { return _isopen; } set { _isopen = value; NotifyPropertyChanged("IsOpen"); } }

        public string Contents
        {
            get { return (string)GetValue(ContentsProperty); }
            set { SetValue(ContentsProperty, value); }
        }
        public static readonly DependencyProperty ContentsProperty =
            DependencyProperty.Register("Contents", typeof(string), typeof(SelectButton));

        public ImageSource Image
        {
            get { return (ImageSource)GetValue(ImageProperty); }
            set { SetValue(ImageProperty, value); }
        }
        public static readonly DependencyProperty ImageProperty =
            DependencyProperty.Register("Image", typeof(ImageSource), typeof(SelectButton));

        public ImageSource HoverImage
        {
            get { return (ImageSource)GetValue(HoverImageProperty); }
            set { SetValue(HoverImageProperty, value); }
        }
        public static readonly DependencyProperty HoverImageProperty =
            DependencyProperty.Register("HoverImage", typeof(ImageSource), typeof(SelectButton));
        public ImageSource PressedImage
        {
            get { return (ImageSource)GetValue(PressedImageProperty); }
            set { SetValue(PressedImageProperty, value); }
        }
        public static readonly DependencyProperty PressedImageProperty =
            DependencyProperty.Register("PressedImage", typeof(ImageSource), typeof(SelectButton));
        public ImageSource DisableImage
        {
            get { return (ImageSource)GetValue(DisableImageProperty); }
            set { SetValue(DisableImageProperty, value); }
        }
        public static readonly DependencyProperty DisableImageProperty =
            DependencyProperty.Register("DisableImage", typeof(ImageSource), typeof(SelectButton));


        public double PopupVerticalOffset
        {
            get { return (double)GetValue(PopupVerticalOffsetProperty); }
            set { SetValue(PopupVerticalOffsetProperty, value); }
        }
        public static readonly DependencyProperty PopupVerticalOffsetProperty =
            DependencyProperty.Register("PopupVerticalOffset", typeof(double), typeof(SelectButton));

        public double PopupHorizontalOffset
        {
            get { return (double)GetValue(PopupHorizontalOffsetProperty); }
            set { SetValue(PopupHorizontalOffsetProperty, value); }
        }
        public static readonly DependencyProperty PopupHorizontalOffsetProperty =
            DependencyProperty.Register("PopupHorizontalOffset", typeof(double), typeof(SelectButton));


        //public List<ListBoxItem> Items
        //{
        //    get { return (List<ListBoxItem>)GetValue(ItemsProperty); }
        //    set { SetValue(ItemsProperty, value); }
        //}
        //public static readonly DependencyProperty ItemsProperty =
        //    DependencyProperty.Register("Items", typeof(List<ListBoxItem>), typeof(SelectButton), new PropertyMetadata(new List<ListBoxItem>()));


        public List<object> Items
        {
            get { return (List<object>)GetValue(ItemsProperty); }
            set { 
                SetValue(ItemsProperty, value); }
        }
        public static readonly DependencyProperty ItemsProperty =
            DependencyProperty.Register("Items", typeof(List<object>), typeof(SelectButton), new PropertyMetadata(new List<object>()));

        public ListBoxItem SelectedItem
        {
            get { return (ListBoxItem)GetValue(SelectedItemProperty); }
            set { SetValue(SelectedItemProperty, value); }
        }

        public static readonly DependencyProperty SelectedItemProperty =
           DependencyProperty.Register("SelectedItem", typeof(ListBoxItem), typeof(SelectButton), new PropertyMetadata(null));

        //events
        public event RoutedEventHandler Click
        {
            add { AddHandler(ClickRoutedEvent, value); }
            remove { RemoveHandler(ClickRoutedEvent, value); }
        }

        public static readonly RoutedEvent ClickRoutedEvent =
            EventManager.RegisterRoutedEvent("Click", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(SelectButton));

        public event RoutedEventHandler Selected
        {
            add { AddHandler(SelectedRoutedEvent, value); }
            remove { RemoveHandler(SelectedRoutedEvent, value); }
        }

        public static readonly RoutedEvent SelectedRoutedEvent =
            EventManager.RegisterRoutedEvent("Selected", RoutingStrategy.Bubble, typeof(SelectionChangedEventHandler), typeof(SelectButton));


        private void OnButtonClick(object sender, RoutedEventArgs e)
        {
            SelectedItem = null;
            RoutedEventArgs args = new RoutedEventArgs(ClickRoutedEvent);
            RaiseEvent(args);
        }
        private void OnListSelectedChanged(object sender, SelectionChangedEventArgs e)
        {
            ListBox list = sender as ListBox;
            if (null == list.SelectedItem) return;

            IsOpen = false;
            SelectedItem = list.SelectedItem as ListBoxItem;
            SelectionChangedEventArgs args = new SelectionChangedEventArgs(SelectedRoutedEvent, e.RemovedItems, e.AddedItems);
            RaiseEvent(args);

            list.SelectedItem = null;
            
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion      
    }
}
