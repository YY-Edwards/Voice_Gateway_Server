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
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class Main : Window
    {
        public Main()
        {
            InitializeComponent();

            //fortest
            this.Loaded += delegate {

                Normal.Visibility = Visibility.Visible;
                LocationInDoor_NetWork.Visibility = Visibility.Visible;
                dck_LocationInDoor_iBeacons.Visibility = Visibility.Visible;
                dck_LocationInDoor_Areas.Visibility = Visibility.Visible;
                
            };
        }
    }
}