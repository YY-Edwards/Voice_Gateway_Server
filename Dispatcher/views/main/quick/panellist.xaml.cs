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

using Sigmar.Controls;
using Dispatcher.ViewsModules;
using Dispatcher.Service;
using Sigmar.Logger;

namespace Dispatcher.Views
{
    /// <summary>
    /// panellist.xaml 的交互逻辑
    /// </summary>
    public partial class QuickList : UserControl
    {
        public QuickList()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(control_loaded);
        }

        private void control_loaded(object sender, RoutedEventArgs e)
        {
            if (null != list) list.View = (ViewBase)this.FindResource("IconView");

            Log.Info("Quick List is Loaded");
        }

        public ViewType_t View
        {
            get { return (ViewType_t)GetValue(ViewProperty); }
            set { SetValue(ViewProperty, value); }
        }

        public static readonly DependencyProperty ViewProperty = DependencyProperty.Register("View", typeof(ViewType_t), typeof(QuickList), new PropertyMetadata(ViewType_t.IconView, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            QuickList quick = source as QuickList;
            if (quick != null)
            {
                switch ((ViewType_t)e.NewValue)
                {
                    case ViewType_t.IconView:
                        quick.list.View = (ViewBase)quick.FindResource("IconView");
                        break;
                    case ViewType_t.PanelView:
                        quick.list.View = (ViewBase)quick.FindResource("PanelView");
                        break;
                }
            }
        }));
      
        public event RoutedEventHandler ViewChanged
        {
            add { AddHandler(ViewChangedRoutedEvent, value); }
            remove { RemoveHandler(ViewChangedRoutedEvent, value); }
        }

        public static readonly RoutedEvent ViewChangedRoutedEvent =
           EventManager.RegisterRoutedEvent("ViewChanged", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(QuickList));
        private void list_PreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            View = ViewType_t.PanelView;
            RaiseEvent(new RoutedEventArgs(ViewChangedRoutedEvent));

        }
    }
}
