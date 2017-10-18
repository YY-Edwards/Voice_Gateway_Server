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
using Sigmar.Extension;
using Dispatcher.Service;

namespace Dispatcher.Views
{
    /// <summary>
    /// newoperationwindow.xaml 的交互逻辑
    /// </summary>
    public partial class NewOperationWindow : BaseWindow
    {
        public NewOperationWindow()
        {
            InitializeComponent();
            tabControl.Loaded += delegate { tabControl.SelectedIndex = (int)Type;};
        }

        private void window_close(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        public TaskType_t Type
        {
            get { return (TaskType_t)GetValue(TypeProperty); }
            set { SetValue(TypeProperty, value); }
        }

        public static readonly DependencyProperty TypeProperty = DependencyProperty.Register("Type", typeof(TaskType_t), typeof(NewOperationWindow), new PropertyMetadata(TaskType_t.Schedule, delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            NewOperationWindow quick = source as NewOperationWindow;
            if (quick != null)
            {
                switch ((TaskType_t)e.NewValue)
                {
                    case TaskType_t.Schedule:
                        quick.rad_call.IsChecked = true;
                        quick.tabControl.SelectedIndex = 0;

                        break;
                    case TaskType_t.ShortMessage:
                        quick.rad_message.IsChecked = true;
                        quick.tabControl.SelectedIndex = 1;
                        break;
                    case TaskType_t.Controler:
                        quick.rad_ctrl.IsChecked = true;
                        quick.tabControl.SelectedIndex = 2;
                        break;
                    case TaskType_t.Location:
                        quick.rad_gps.IsChecked = true;
                        quick.tabControl.SelectedIndex = 3;
                        break;
                    case TaskType_t.LocationInDoor:
                        quick.rad_beacon.IsChecked = true;
                        quick.tabControl.SelectedIndex = 4;
                        break;
                    case TaskType_t.JobTicket:
                        quick.rad_job.IsChecked = true;
                        quick.tabControl.SelectedIndex = 5;
                        break;
                    case TaskType_t.Patrol:
                        quick.rad_patrol.IsChecked = true;
                        quick.tabControl.SelectedIndex = 6;
                        break;
                }
            }
        }));

        private void rad_check(object sender, RoutedEventArgs e)
        {
            Type = (((RadioButton)sender).Tag as string).ToEnum<TaskType_t>();            
        }

        private void Border_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
    }
}
