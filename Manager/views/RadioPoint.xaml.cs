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

namespace Manager
{
    /// <summary>
    /// CRadioPos.xaml 的交互逻辑
    /// </summary>
    public partial class RadioPoint : UserControl
    {
        public RadioPoint()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty ImageProperty =
       DependencyProperty.Register("Radio", typeof(CRadio), typeof(RadioPoint), new UIPropertyMetadata(null));

        public CRadio Radio { set { SetValue(ImageProperty, value); } get { return GetValue(ImageProperty) as CRadio; } }

        private void Image_MouseEnter(object sender, MouseEventArgs e)
        {
            bdr_Content.Visibility = System.Windows.Visibility.Visible;

        }

        private void Image_MouseLeave(object sender, MouseEventArgs e)
        {
            bdr_Content.Visibility = System.Windows.Visibility.Hidden;
        }
    }
}
