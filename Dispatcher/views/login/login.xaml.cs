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

using System.Net;

using System.ComponentModel;

using System.Diagnostics;
using System.IO;

using Dispatcher.ViewsModules;
using Dispatcher;

namespace Dispatcher.Views
{
    /// <summary>
    /// login.xaml 的交互逻辑
    /// </summary>
    public partial class Login : BaseWindow
    {
        private string loginbackgroud = App.RuntimeDir + "a453d476-5a9e-49e9-a767-f30055979069.htm";
        Initilize initilizeWindow;
        public Login()
        {
            InitializeComponent();

            ReleaseBackgroud();
            if (!File.Exists(loginbackgroud))
            {
                Log.Fatal("Background File of Login Window id Not Found.");
            }
            web.FileUrl = loginbackgroud;
            this.Loaded += delegate { Log.Info("Login window is Loaded."); };


            initilizeWindow = new Initilize();
            initilizeWindow.DataContext = this.DataContext;
        }

        private void ReleaseBackgroud()
        {
            if (File.Exists(loginbackgroud)) return;

            try
            {
                if (Directory.Exists(App.RuntimeDir) == false)
                {
                    Directory.CreateDirectory(App.RuntimeDir);
                }

                System.IO.File.WriteAllText(loginbackgroud, global::Dispatcher.resource.Amap.login);
                System.IO.File.SetAttributes(loginbackgroud, FileAttributes.Hidden | FileAttributes.ReadOnly);
            }
            catch (Exception ex)
            {
                Log.Fatal("Can not load file ,because can not release background", ex);
            }
        }

        private void btn_cancel_Click(object sender, RoutedEventArgs e)
        {
            Log.Info("Exit Application");
            Environment.Exit(0);
        }
      
        private void btn_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
    
        private void psd_Password_PasswordChanged(object sender, RoutedEventArgs e)
        {
            ((PasswordBox)sender).Tag = ((PasswordBox)sender).Password;
            (this.DataContext as VMLogin).Info = "";
        }

        private void txt_User_TextChanged(object sender, TextChangedEventArgs e)
        {
            (this.DataContext as VMLogin).Info = "";
            (this.DataContext as VMLogin).UserName = (sender as TextBox).Text;
        }

       

        private void OnLoginOK(object sender, EventArgs e)
        {
            Log.Info("Open Initialize Window.");
            this.Dispatcher.BeginInvoke((Action)delegate()
            {
                initilizeWindow.Show();
                this.Close();
            });      
        }

        private Main main;

        private void OnInitializeCompleted(object sender, EventArgs e)
        {
            Log.Info("Open Main Window.");
            this.Dispatcher.BeginInvoke((Action)delegate()
            {
                if (main == null)
                {
                    main = new Main();
                    main.Show();
                }
                initilizeWindow.Close();
            });  
        }
    }
}
