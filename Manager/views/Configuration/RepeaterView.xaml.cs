using Manager.Models;
using Sigmar.Extension;
using System.Windows;
using System.Windows.Controls;

using Manager.Models;
using Manager.ViewModels;

namespace Manager.Views
{
    /// <summary>
    /// RepeaterSettingView.xaml 的交互逻辑
    /// </summary>
    public partial class RepeaterView : ManageView
    {
        public RepeaterView()
        {
            InitializeComponent();

            this.IPSCMode.Click += new RoutedEventHandler(WorkModeClick);
            this.LCPMode.Click += new RoutedEventHandler(WorkModeClick);
            this.CPCMode.Click += new RoutedEventHandler(WorkModeClick);
        }

        private void WorkModeClick(object sender, RoutedEventArgs e)
        {
            RadioButton checkedWorkMode = FindCheckedRadioButton(this.WorkModeContainer.Children, "RepeaterWorkMode");
            Repeater.WireLanType workmode = checkedWorkMode == null || checkedWorkMode.Tag == null ? Repeater.WireLanType.IPSC : checkedWorkMode.Tag.ToString().ToEnum<Repeater.WireLanType>();

            RepeaterViewModel viewModel = this.DataContext as RepeaterViewModel;
            if (viewModel != null) viewModel.WorkMode = workmode;
        }
    }
}