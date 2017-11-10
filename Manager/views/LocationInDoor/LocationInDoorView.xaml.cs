namespace Manager.Views
{
    /// <summary>
    /// LocationInDoorSettingView.xaml 的交互逻辑
    /// </summary>
    public partial class LocationInDoorView : ManageView
    {
        public LocationInDoorView()
        {
            InitializeComponent();
        }

        //private CVMLocationInDoorSetting LocationInDoorSettingViewModels = new CVMLocationInDoorSetting();
        //private LocationQueryType_t _LocationType = LocationQueryType_t.General;
        //public LocationInDoorSettingView()
        //    : base("室内位置")
        //{
        //    InitializeComponent();
        //    this.GeneralDock.DataContext = LocationInDoorSettingViewModels;
        //}

        //public void SetQueryType(LocationQueryType_t type)
        //{
        //    LocationInDoorSettingViewModels.SetQueryType.Execute(type);
        //}

        //public override void Save()
        //{
        //    LocationInDoorSettingViewModels.Save.Execute(null);
        //}

        //public override void Update()
        //{
        //    LocationInDoorSettingViewModels.Update.Execute(null);
        //}
    }
}