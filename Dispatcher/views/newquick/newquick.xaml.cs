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
using System.Windows.Shapes;

using Sigmar.Windows;
using Dispatcher.Service;

namespace Dispatcher.Views
{
    /// <summary>
    /// newquick.xaml 的交互逻辑
    /// </summary>
    public partial class NewQuick : BaseWindow
    {
        public NewQuick()
        {
            InitializeComponent();
        }

        private void window_close(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        public QuickPanelType_t Type
        {
            get { return (QuickPanelType_t)GetValue(TypeProperty); }
            set { SetValue(TypeProperty, value); }
        }

        public static readonly DependencyProperty TypeProperty = DependencyProperty.Register("Type", typeof(QuickPanelType_t), typeof(NewQuick), new PropertyMetadata(QuickPanelType_t.Target, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            NewQuick quick = source as NewQuick;
            if (quick != null)
            {
                switch ((QuickPanelType_t)e.NewValue)
                {
                    case QuickPanelType_t.Target:
                        quick.rad_target.IsChecked = true;
                       quick.tabControl.SelectedIndex = 0;
                       
                        break;
                    case QuickPanelType_t.Operation:
                        quick.rad_operation.IsChecked = true;
                        quick.tabControl.SelectedIndex = 1;
                        break;
                }
            }
        }));

        private void rad_check(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).Content as string == "常用联系人") Type = QuickPanelType_t.Target;
            else if (((RadioButton)sender).Content as string == "快速操作") Type = QuickPanelType_t.Operation;
        }

        private void Border_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
    }
}
