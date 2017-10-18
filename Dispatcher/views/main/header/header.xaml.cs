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
using Sigmar.Logger;
using Dispatcher.ViewsModules;

namespace Dispatcher.Views
{
    /// <summary>
    /// headxaml.xaml 的交互逻辑
    /// </summary>
    public partial class Header : UserControl
    {
        public Header()
        {
            InitializeComponent();
           
            this.Loaded += delegate 
            {
                Log.Info("Header is Loaded.");                        
            };
        }

        public WindowState State
        {
            get { return (WindowState)GetValue(StateProperty); }
            set { SetValue(StateProperty, value); }
        }
        public static readonly DependencyProperty StateProperty =
            DependencyProperty.Register("State", typeof(WindowState), typeof(Header));

    }
}
