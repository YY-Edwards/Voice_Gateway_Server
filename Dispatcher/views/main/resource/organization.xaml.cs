using System;
using System.Collections;
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

using Sigmar;
using Sigmar.Controls;
using Sigmar.Extension;
using Dispatcher.ViewsModules;
using Dispatcher.Service;
using Dispatcher;

namespace Dispatcher.Views
{
    /// <summary>
    /// organization.xaml 的交互逻辑
    /// </summary>
    public partial class Organization : UserControl
    {
        public Organization()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(window_loaded);
        }
        private void window_loaded(object sender, RoutedEventArgs e)
        {
            Log.Info("Organization is Loaded");
        }

        private void tree_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            Point p = e.GetPosition((ItemsControl)sender);

            HitTestResult htr = VisualTreeHelper.HitTest((ItemsControl)sender, p);

            if (htr.VisualHit != null)
            {
                TreeViewItem item = htr.VisualHit.FindParent<TreeViewItem>();
                if (item == null) return;
                item.Focus();
                item.IsSelected = true;
            }
        }

        public void SetSelectionItemNull()
        {           
            FindAndUnSelectedNode(tree.Items);
            if (this.DataContext != null) (this.DataContext as VMOrganization).SelectedItemChanged.Execute(tree.SelectedItem);
        }

        private void FindAndUnSelectedNode(IEnumerable items)
        {
            foreach(TreeViewItem item in items)
            {
                if (item.IsSelected) item.IsSelected = false;
                if (item.Items != null) FindAndUnSelectedNode(item.Items);
            }
        }
    }
}
