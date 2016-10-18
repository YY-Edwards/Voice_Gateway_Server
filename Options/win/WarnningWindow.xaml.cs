using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TrboX
{
    public enum ResultType
    {
        Enter,
        Exit,
        Cancle
    };
    /// <summary>
    /// WarnningWindow.xaml 的交互逻辑
    /// </summary>
    public partial class WarnningWindow : MyWindow
    {


        public ResultType Reslut = ResultType.Cancle; 
        
        public WarnningWindow()
        {
            InitializeComponent();
        }

        private void btn_Enter_Click(object sender, RoutedEventArgs e)
        {
            Reslut = ResultType.Enter;
            this.Close();
        }

        private void btn_Exit_Click(object sender, RoutedEventArgs e)
        {
            Reslut = ResultType.Exit;
            this.Close();
        }

        private void btn_Cancel_Click(object sender, RoutedEventArgs e)
        {
            Reslut = ResultType.Cancle;
            this.Close();
        }




    }
}
