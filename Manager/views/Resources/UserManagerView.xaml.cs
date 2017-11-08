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

using Manager.ViewModels;
namespace Manager.Views
{
    /// <summary>
    /// UserManagerView.xaml 的交互逻辑
    /// </summary>
    public partial class UserManagerView : ManageView
    {
        public UserManagerView()
        {
            InitializeComponent();
            this._userPassword.PasswordChanged += new RoutedEventHandler(UserPasswordPasswordChanged);
        }

        private void UserPasswordPasswordChanged(object sender, RoutedEventArgs e)
        {
            UsersViewModel viewModel = this.DataContext as UsersViewModel;
            if (viewModel != null) viewModel.Password = (sender as PasswordBox).Password;
        }
    }
}
