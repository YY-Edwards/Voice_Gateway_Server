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
using Dispatcher;

namespace Dispatcher.Views
{
    /// <summary>
    /// status.xaml 的交互逻辑
    /// </summary>
    public partial class Status : UserControl
    {
        public Status()
        {
            InitializeComponent();

            this.Loaded += delegate
            {
                Log.Info("Status is Loaded");
            };
        }
    }
}
