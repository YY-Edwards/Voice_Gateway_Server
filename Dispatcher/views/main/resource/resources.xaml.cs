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
    /// resources.xaml 的交互逻辑
    /// </summary>
    public partial class Resources : UserControl
    {
        public Resources()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(window_loaded);
        }

        private void window_loaded(object sender, RoutedEventArgs e)
        {
            if (this.DataContext != null)
            {               
                //organization.RegisterLoggerEvents(this.DataContext as VMResources);
                //this.AddLogger(this.DataContext as VMResources);

                (this.DataContext as VMResources).OnResourcesInitilized += new EventHandler(OnResourcesInitilized);
            }

            Log.Info("Resources is Loaded");
        }
        private void OnResourcesInitilized(object sender, EventArgs e)
        {
            this.Dispatcher.BeginInvoke((Action)delegate()
            {
                if (this.DataContext != null) (this.DataContext as VMResources).UpdateResource.Execute(null);
            });
        }

        private int _tabcontrolindex = -1;
        private void tabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (((TabControl)sender).SelectedIndex != _tabcontrolindex)
            {
                _tabcontrolindex = ((TabControl)sender).SelectedIndex;

                organization.SetSelectionItemNull();
                groups.SelectedIndex = -1;
                staffs.SelectedIndex = -1;
                vehicles.SelectedIndex = -1;
                device.SelectedIndex = -1;
                ibeacons.SelectedIndex = -1;
            }
        }
    }
}
