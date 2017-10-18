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
using Sigmar.Logger;

namespace Dispatcher.Views
{
    /// <summary>
    /// logger.xaml 的交互逻辑
    /// </summary>
    public partial class Logs : LogWindow
    {
        public Logs()
        {
            InitializeComponent();

            Log.BindingMessage(__logBox_ReceiveMessage);
        }

        private void __logBox_ReceiveMessage(LogContent log)
        {
            if (__logBox == null) return;
            __logBox.AddLog(log);
        }
    }
}
