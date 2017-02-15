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

using System.Net;

using System.ComponentModel;

using System.Diagnostics;
using System.IO;

namespace TrboX
{
    /// <summary>
    /// Login.xaml 的交互逻辑
    /// </summary>
    public partial class Login : MyWindow
    {
        Main MainWindow = new Main();

        public Login()
        {
            InitializeComponent();

            //TServer Starup 
            //StaertTServer();

            new CServer().StartService("Trbox.Monitor");

            LogServer.InitializeServer();  
            //DataBase.open(DateTime.Now.Year.ToString() 
            //    + DateTime.Now.Month.ToString()
            //    + DateTime.Now.Day.ToString()
            //    + DateTime.Now.Hour.ToString()
            //    + DateTime.Now.Minute.ToString()
            //    + DateTime.Now.Second.ToString()
            //    + DateTime.Now.Millisecond.ToString() 
            //    + ".lg");


            CommandBinding cb = new CommandBinding();
            cb.Command = ShortCutKey.Login;
            cb.Executed += new ExecutedRoutedEventHandler(delegate(object sender, ExecutedRoutedEventArgs e) {LoginMeath(); });
            CommandBindings.Add(cb);


            CommandBinding cq = new CommandBinding();
            cq.Command = ShortCutKey.Quit;
            cq.Executed += new ExecutedRoutedEventHandler(delegate(object sender, ExecutedRoutedEventArgs e) { Environment.Exit(0); });
            CommandBindings.Add(cq);

            DataBase.open("DebugLog.lg");
            DataBase.InsertLog("---Records Start----------------------------------------------------------");
        }
        private void StaertTServer()
        {
            Process[] processes;
            processes = Process.GetProcessesByName("TServer");

            if (processes.Count() > 0) return;
            else
            {
                if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + "/Svr/TServer.exe"))
                {
                    System.Diagnostics.Process.Start(AppDomain.CurrentDomain.BaseDirectory + "/Svr/TServer.exe");
                }
            }          
        }

       private void MyWindow_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }


       private void LoginMeath()
       {
           UserMgr.Auth(txt_User.Text, psd_Password.Password,
               delegate(User user)
               {

                   this.Dispatcher.Invoke(new Action(() =>
                   {
                       MainWindow.Show();
                       grd_LoginErr.Visibility = Visibility.Collapsed;
                       this.Hide();
                   }));

               },
               delegate(User user)
               {
                   this.Dispatcher.Invoke(new Action(() =>
                   {
                       grd_LoginErr.Visibility = Visibility.Visible;
                   }));
               });
       }
        private void btn_Login_Click(object sender, RoutedEventArgs e)
        {

            LoginMeath();

        }

        private void btn_cancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void psd_Password_PasswordChanged(object sender, RoutedEventArgs e)
        {
            ((PasswordBox)sender).Tag = ((PasswordBox)sender).Password;
            grd_LoginErr.Visibility = Visibility.Collapsed;
        }

        private void btn_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void btn_SysClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void txt_User_TextChanged(object sender, TextChangedEventArgs e)
        {
            grd_LoginErr.Visibility = Visibility.Collapsed;
        }

    }
}
