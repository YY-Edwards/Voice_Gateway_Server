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
        }

        //private VMRepeaterSetting RepeaterSettingViewModels = new VMRepeaterSetting();
        //public RepeaterSettingView()
        //    : base("中继台")
        //{
        //    InitializeComponent();
        //    this.DataContext = RepeaterSettingViewModels;
        //    this.checkReapeaterEnable.Click += new RoutedEventHandler(CheckReapeaterEnableClick);
        //    this.IPSCMode.Click += new RoutedEventHandler(WorkModeClick);
        //    this.LCPMode.Click += new RoutedEventHandler(WorkModeClick);
        //    this.CPCMode.Click += new RoutedEventHandler(WorkModeClick);
        //}

        //public event Action<object, bool> EnableChanged;
        //private void CheckReapeaterEnableClick(object sender, RoutedEventArgs e)
        //{
        //    if (EnableChanged != null) EnableChanged(this, (bool)(sender as CheckBox).IsChecked);
        //}

        //private void WorkModeClick(object sender, RoutedEventArgs e)
        //{
        //    RadioButton checkedWorkMode = Utility.FindCheckedRadioButton(this.WorkModeContainer.Children, "WorkMode");
        //    if(checkedWorkMode != null)
        //    {
        //        CRepeaterSetting.WireLanType workMode = checkedWorkMode == null || checkedWorkMode.Tag == null ? CRepeaterSetting.WireLanType.IPSC : checkedWorkMode.Tag.ToString().ToEnum<CRepeaterSetting.WireLanType>();
        //        RepeaterSettingViewModels.WorkMode = workMode;
        //    }
        //}
        //public void Enable()
        //{
        //    this.checkReapeaterEnable.IsChecked = true;
        //}

        //public void Disable()
        //{
        //    this.checkReapeaterEnable.IsChecked = false;
        //}
        //public override void Save()
        //{
        //    RepeaterSettingViewModels.Save.Execute(null);
        //}

        //public override void Update()
        //{
        //    RepeaterSettingViewModels.Update.Execute(null);
        //}
    }
}