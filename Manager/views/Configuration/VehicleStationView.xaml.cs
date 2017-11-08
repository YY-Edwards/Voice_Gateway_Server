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

        //private VMRadioSetting RadioSettingViewModels = new VMRadioSetting();
        //public RadioSettingView()
        //    : base("车载台")
        //{
        //    InitializeComponent();

        //    RadioSettingViewModels.ConfigurationChanged += new Action<SettingType, CRadioSetting>(ConfigurationChanged);
        //    this.DataContext = RadioSettingViewModels;
        //    this.checkRadioEnable.Click += new RoutedEventHandler(CheckRadioEnableClick);
        //   
        //}


        //public event Action<object, bool> EnableChanged;
        //public event Action<object, LocationQueryType_t> QueryTypChanged;

        //private void ConfigurationChanged(SettingType type,CRadioSetting setting)
        //{
        //    this.Dispatcher.BeginInvoke(new Action(() => {
        //        if (EnableChanged != null) EnableChanged(this, setting.IsEnable);
        //        if (setting.IsEnable && QueryTypChanged != null) QueryTypChanged(this, setting.LocationQueryType);
        //    }));           
        //}

        //private void CheckRadioEnableClick(object sender, RoutedEventArgs e)
        //{
        //    if (EnableChanged != null) EnableChanged(this, (bool)(sender as CheckBox).IsChecked);

        //    if ((bool)this.checkRadioEnable.IsChecked && QueryTypChanged != null)
        //    {
        //        RadioButton checkedQueryType = Utility.FindCheckedRadioButton(this.QueryTypeContainer.Children, "RadioQueryType");
        //        LocationQueryType_t queryType = checkedQueryType == null || checkedQueryType.Tag == null ? LocationQueryType_t.General : checkedQueryType.Tag.ToString().ToEnum<LocationQueryType_t>();
        //        QueryTypChanged(this, queryType);
        //    } 
        //}

        private void WorkModeClick(object sender, RoutedEventArgs e)
        {
            RadioButton checkedQueryType = FindCheckedRadioButton(this.WorkModeContainer.Children, "RadioWorkMode");
            VehicleStation.ModeType_t workmode = checkedQueryType == null || checkedQueryType.Tag == null ? VehicleStation.ModeType_t.General : checkedQueryType.Tag.ToString().ToEnum<VehicleStation.ModeType_t>();

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



        //public void Enable()
        //{
        //    this.checkRadioEnable.IsChecked = true;
        //}

        //public void Disable()
        //{
        //    this.checkRadioEnable.IsChecked = false;
        //}
        //public override void Save()
        //{
        //    RadioSettingViewModels.Save.Execute(null);
        //}

        //public override void Update()
        //{
        //    RadioSettingViewModels.Update.Execute(null);
        //}
    }
}
