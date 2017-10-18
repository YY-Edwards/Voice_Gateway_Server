using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

using System.ComponentModel;
using System.Windows.Interop;
using System.Runtime.InteropServices;

using Sigmar;
using Sigmar.Controls;

using Dispatcher.ViewsModules;

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
    ///     <MyNamespace:targetselector/>
    ///
    /// </summary>
    public class TargetSelector : Control, INotifyPropertyChanged
    {
        private CheckBox check = null;
        private ListBox list = null;
        static TargetSelector()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(TargetSelector), new FrameworkPropertyMetadata(typeof(TargetSelector)));
        }


        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;
            check = (CheckBox)baseTemplate.FindName("checkbox", this);
            list = (ListBox)baseTemplate.FindName("list", this);
        }

        public TargetSelector()
        {
            this.Loaded += delegate
            {
                HwndSource hs = PresentationSource.FromVisual(this) as HwndSource;
                if (hs != null) hs.AddHook(new HwndSourceHook(WndProc));
               
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

        public double PopupVerticalOffset
        {
            get { return (double)GetValue(PopupVerticalOffsetProperty); }
            set { SetValue(PopupVerticalOffsetProperty, value); }
        }
        public static readonly DependencyProperty PopupVerticalOffsetProperty =
            DependencyProperty.Register("PopupVerticalOffset", typeof(double), typeof(TargetSelector));

        public double PopupHorizontalOffset
        {
            get { return (double)GetValue(PopupHorizontalOffsetProperty); }
            set { SetValue(PopupHorizontalOffsetProperty, value); }
        }
        public static readonly DependencyProperty PopupHorizontalOffsetProperty =
            DependencyProperty.Register("PopupHorizontalOffset", typeof(double), typeof(TargetSelector));

        public class Targets_t:INotifyPropertyChanged
        {

            public event EventHandler NeedCleer;
            public VMTarget Target { set; get; }

            private bool _isselected = false;
            public bool IsSelected { set { _isselected = value; } get { return _isselected; } }


            public ICommand SelectedChanged { get { return new Command(SelectedChangedExec); } }

            private void SelectedChangedExec(object parameter)
            {
                if (NeedCleer != null) NeedCleer(this,null);
                IsSelected = !IsSelected;
                NotifyPropertyChanged("IsSelected");
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

        public Visibility MultipleCheckBoxVisible { get { return IsMultiple ? Visibility.Visible: Visibility.Collapsed;} }


        private bool _isopen = false;
        public bool IsOpen { get { return _isopen; } set { _isopen = value; NotifyPropertyChanged("IsOpen"); } }

        public bool IsMultiple
        {
            get { return (bool)GetValue(IsMultipleProperty); }
            set { SetValue(IsMultipleProperty, value); }
        }

        public static readonly DependencyProperty IsMultipleProperty =
           DependencyProperty.Register("IsMultiple", typeof(bool), typeof(TargetSelector), new PropertyMetadata(true, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
           {
               TargetSelector selector = source as TargetSelector;
               selector.NotifyPropertyChanged("MultipleCheckBoxVisible");
               selector.NotifyPropertyChanged("SelectedItemsDisplay");
           }));


        private List<VMTarget> _selectitems;
        public List<VMTarget> SelectedItems
        {
            get {
                return (List<VMTarget>)GetValue(SelectedItemsProperty); 
            }
            set {
                SetValue(SelectedItemsProperty, value);
            }
        }

        public static readonly DependencyProperty SelectedItemsProperty =
           DependencyProperty.Register("SelectedItems", typeof(List<VMTarget>), typeof(TargetSelector), new PropertyMetadata(new List<VMTarget>() , delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
           {
               TargetSelector selector = source as TargetSelector;
               selector.NotifyPropertyChanged("Items");
               selector.NotifyPropertyChanged("SelectedItemsDisplay");
           }));

        public List<object> SelectedItemsDisplay
        {
            get
            {
                _selectitems = new List<VMTarget>();
                foreach (VMTarget target in SelectedItems) _selectitems.Add(target);

                if (_selectitems == null) return null;
                else
                {                   
                    List<object> items = new List<object>();
                    for (int i = 0; i < _selectitems.Count; i++)
                    {                      
                        if (i >= 2)
                        {
                            items.Add("...");
                            break;
                        }
                        items.Add(_selectitems[i]);
                    }
                    return items;
                }
            }
        }
        public ICollectionView ItemSource
        {
            get { return (ICollectionView)GetValue(ItemSourceProperty); }
            set { SetValue(ItemSourceProperty, value); }
        }
        public static readonly DependencyProperty ItemSourceProperty =
            DependencyProperty.Register("ItemSource", typeof(ICollectionView), typeof(TargetSelector), new PropertyMetadata(null, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            TargetSelector selector = source as TargetSelector;
            selector.NotifyPropertyChanged("Items");
        }));

        private List<Targets_t> _alltargets;

        private void OnSelectedChanged(object sender, PropertyChangedEventArgs e)
        {
            if (Items == null) _selectitems = null;
            
            else
            {
                List<Targets_t> targets = Items.FindAll(p => p.IsSelected);
                if (targets != null)
                {
                    //SelectedItems = new List<VMTarget>();
                    List<VMTarget> items = new List<VMTarget>();
                    foreach (Targets_t target in targets)
                    {
                        items.Add(target.Target);
                        //SelectedItems.Add(target.Target);
                    }
                    _selectitems = items;
                }
                else _selectitems = null;

                SelectedItems = _selectitems;
            }

           
    
            //SetValue(SelectedItemsProperty, _selectitems);
            //SelectedItems = _selectitems;// new ListCollectionView(_selectitems);

            NotifyPropertyChanged("SelectedItemsDisplay");

            if (!IsMultiple) IsOpen = false;
        }


        private void OnNeedClear(object sender, EventArgs e)
        {
         if(!IsMultiple)for (int i = 0; i < _alltargets.Count; i++) _alltargets[i].IsSelected = false;
        }


        public List<Targets_t> Items
        {
            get {
                if (_alltargets == null)
                {
                    if (ItemSource == null) _alltargets = null ;
                    else
                    {
                        _alltargets = new List<Targets_t>();
                        foreach (VMTarget target in ItemSource)
                        {
                            Targets_t item = new Targets_t() { Target = target };
                            if (_selectitems.Contains(target)) item.IsSelected = true;
                            item.PropertyChanged += OnSelectedChanged;
                            item.NeedCleer += OnNeedClear;

                            _alltargets.Add(item);
                        }
                    }                   
                }
                return _alltargets;               
            }               
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

    public class TargetView : ViewBase
    {
        private DataTemplate itemTemplate;
        public DataTemplate ItemTemplate
        {
            set { itemTemplate = value; }
            get { return itemTemplate; }
        }

        protected override object DefaultStyleKey
        {
            get
            {
                return new ComponentResourceKey(GetType(), "TargetView");
            }
        }

        protected override object ItemContainerDefaultStyleKey
        {
            get
            {
                return new ComponentResourceKey(GetType(), "TargetViewItem");
            }
        }
    }

}
