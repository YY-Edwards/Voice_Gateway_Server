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
    public partial class AreaView : UserControl
    {
        public AreaView()
        {
            InitializeComponent();
            this._openLocalMap.Click += new RoutedEventHandler(OnOpenLocalMapCLick);
        }

        private void OnOpenLocalMapCLick(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.Title = "选择文件";
            openFileDialog.Filter = "图片文件|*.jpg;*.bmp|所有文件|*.*";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;
            System.Windows.Forms.DialogResult result = openFileDialog.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.Cancel)
            {
                return;
            }

            System.IO.FileInfo fileInfo = new System.IO.FileInfo(openFileDialog.FileName);

            this._mapUrl.Text = fileInfo.FullName;

            AreaViewModel viewModel = this.DataContext as AreaViewModel;
            if (viewModel != null)
            {
                viewModel.ExtName = fileInfo.Extension;
                viewModel.MapUrl = fileInfo.FullName;
            }
        }

    }
}
