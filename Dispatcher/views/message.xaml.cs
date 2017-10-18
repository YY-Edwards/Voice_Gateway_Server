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

namespace Dispatcher.Views
{
    /// <summary>
    /// message.xaml 的交互逻辑
    /// </summary>
    public partial class Message : BaseWindow
    {
        public Message()
        {
            InitializeComponent();
        }

        public ResultType Result = ResultType.Cancel; 
       
        private void enter(object sender, RoutedEventArgs e)
        {
            Result = ResultType.Enter;
            this.Close();
        }

        private void exit(object sender, RoutedEventArgs e)
        {
            Result = ResultType.Exit;
            this.Close();
        }

        private void close(object sender, RoutedEventArgs e)
        {
           this.Close();
        }

        public enum ResultType
        {
            Enter,
            Exit,
            Cancel
        }
    }
}
