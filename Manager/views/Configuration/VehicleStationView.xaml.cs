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

using Sigmar.Extension;

using Manager.Models;
using Manager.ViewModels;

namespace Manager.Views
{
    /// <summary>
    /// RadioSettingView.xaml 的交互逻辑
    /// </summary>
    public partial class VehicleStationView : ManageView
    {
        public VehicleStationView()
        {
            InitializeComponent();

            this.GeneralMode.Click += new RoutedEventHandler(WorkModeClick);
            this.ClusterMode.Click += new RoutedEventHandler(WorkModeClick);

            this.GeneralQuery.Click += new RoutedEventHandler(QueryTypeClick);
            this.CSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
            this.EnhCSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
        }

      
        private void WorkModeClick(object sender, RoutedEventArgs e)
        {
            RadioButton checkedWorkMode = FindCheckedRadioButton(this.WorkModeContainer.Children, "RadioWorkMode");
            VehicleStation.ModeType_t workmode = checkedWorkMode == null || checkedWorkMode.Tag == null ? VehicleStation.ModeType_t.General : checkedWorkMode.Tag.ToString().ToEnum<VehicleStation.ModeType_t>();

            VehicleStationViewModel viewModel = this.DataContext as VehicleStationViewModel;
            if (viewModel != null) viewModel.RideWorkMode = workmode;
        }

        private void QueryTypeClick(object sender, RoutedEventArgs e)
        {
            RadioButton checkedQueryType = FindCheckedRadioButton(this.QueryTypeContainer.Children, "RadioQueryType");
            Configuration.LocationQueryType_t queryType = checkedQueryType == null || checkedQueryType.Tag == null ? Configuration.LocationQueryType_t.General : checkedQueryType.Tag.ToString().ToEnum<Configuration.LocationQueryType_t>();

            VehicleStationViewModel viewModel = this.DataContext as VehicleStationViewModel;
            if (viewModel != null) viewModel.LocationQueryType = queryType;
        }

    }
}
