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

using System.Windows.Media.Animation;
using Sigmar.Controls;
using Dispatcher.ViewsModules;
using Dispatcher.Service;

namespace Dispatcher.Views
{
    /// <summary>
    /// quickicon.xaml 的交互逻辑
    /// </summary>
    public partial class QuickIcon : UserControl
    {
        public QuickIcon()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(window_loaded);
        }

        private void window_loaded(object sender, RoutedEventArgs e)
        {
            //if (this.DataContext != null) this.AddLogger(this.DataContext as VMQuickItem);
        }
    }
}
