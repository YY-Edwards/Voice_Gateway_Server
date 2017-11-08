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
    /// MnisSettingView.xaml 的交互逻辑
    /// </summary>
    public partial class MnisView : ManageView
    {
        public MnisView()
        {
            InitializeComponent();

            this.GeneralQuery.Click += new RoutedEventHandler(QueryTypeClick);
            this.CSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
            this.EnhCSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
        }
        //private VMMnisSetting MnisSettingViewModels = new VMMnisSetting();
        //public MnisSettingView():base("MNIS")
        //{
        //    InitializeComponent();
        //    MnisSettingViewModels.ConfigurationChanged += new Action<SettingType, CMnisSetting>(ConfigurationChanged);

        //    this.DataContext = MnisSettingViewModels;


        //    this.GeneralQuery.Click += new RoutedEventHandler(QueryTypeClick);
        //    this.CSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
        //    this.EnhCSBKQuery.Click += new RoutedEventHandler(QueryTypeClick);
        //}

        //public event Action<object, LocationQueryType_t> QueryTypChanged;

        //private void ConfigurationChanged(SettingType type, CMnisSetting setting)
        //{
        //    this.Dispatcher.BeginInvoke(new Action(() =>
        //    {
        //        if (setting.IsEnable && QueryTypChanged != null) QueryTypChanged(this, setting.LocationQueryType);
        //    }));
        //}

        private void QueryTypeClick(object sender, RoutedEventArgs e)
        {
            RadioButton checkedQueryType = FindCheckedRadioButton(this.QueryTypeContainer.Children, "MnisQueryType");
            Configuration.LocationQueryType_t queryType = checkedQueryType == null || checkedQueryType.Tag == null ? Configuration.LocationQueryType_t.General : checkedQueryType.Tag.ToString().ToEnum<Configuration.LocationQueryType_t>();

            MnisViewModel viewModel = this.DataContext as MnisViewModel;
            if (viewModel != null) viewModel.LocationQueryType = queryType;
        }

        //public bool Enable { get { return (bool)this.checkMnisEnable.IsChecked; } }
        //public override void Save()
        //{
        //    MnisSettingViewModels.Save.Execute(null);
        //}

        //public override void Update()
        //{
        //    MnisSettingViewModels.Update.Execute(null);
        //}
    }
}
