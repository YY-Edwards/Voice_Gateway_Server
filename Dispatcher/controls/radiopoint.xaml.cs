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
using Dispatcher.Modules;

namespace Dispatcher.Controls
{
    /// <summary>
    /// radiopoint.xaml 的交互逻辑
    /// </summary>
    public partial class RadioPoint : UserControl
    {
        public RadioPoint()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty TargetProperty =
        DependencyProperty.Register("Target", typeof(CMember), typeof(RadioPoint), new UIPropertyMetadata(null));

        public CMember Target { set { SetValue(TargetProperty, value); } get { return GetValue(TargetProperty) as CMember; } }

        private void Image_MouseEnter(object sender, MouseEventArgs e)
        {
            bdr_Content.Visibility = System.Windows.Visibility.Visible;
            this.Cursor = Cursors.Hand;
        }

        private void Image_MouseLeave(object sender, MouseEventArgs e)
        {
            bdr_Content.Visibility = System.Windows.Visibility.Hidden;
            this.Cursor = Cursors.Arrow;
        }
    }
}
