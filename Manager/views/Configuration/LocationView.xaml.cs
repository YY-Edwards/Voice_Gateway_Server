namespace Manager.Views
{
    /// <summary>
    /// LocationSettingView.xaml 的交互逻辑
    /// </summary>
    public partial class LocationView : ManageView
    {
        public LocationView()
        {
            InitializeComponent();
        }

        //private VMLocationSetting LocationSettingViewModels = new VMLocationSetting();
        //private LocationQueryType_t _LocationType = LocationQueryType_t.General;
        //public LocationSettingView()
        //    : base("GPS位置")
        //{
        //    InitializeComponent();
        //    this.DataContext = LocationSettingViewModels;
        //}

        //public void SetQueryType(LocationQueryType_t type)
        //{
        //    LocationSettingViewModels.SetQueryType.Execute(type);
        //}

        //public override void Save()
        //{
        //    LocationSettingViewModels.Save.Execute(null);
        //}

        //public override void Update()
        //{
        //    LocationSettingViewModels.Update.Execute(null);
        //}
    }
}