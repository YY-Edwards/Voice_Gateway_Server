using Sigmar.Extension;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Manager.Views
{
    /// <summary>
    /// ConfigureList.xaml 的交互逻辑
    /// </summary>
    public partial class ManageList : UserControl
    {
        private ManageView _selectedManager;

        public event Action<object, ManageView> SelectedChanged;

        //public event MessageHandler Notified;

        //private BaseSettingView _BaseSetting = new BaseSettingView();
        //private RadioSettingView _RadioSetting = new RadioSettingView();
        //private RepeaterSettingView _RepeaterSetting = new RepeaterSettingView();
        //private MnisSettingView _MnisSetting = new MnisSettingView();
        //private LocationSettingView _LocationSetting = new LocationSettingView();
        //private LocationInDoorSettingView _LocationInDoorSetting = new LocationInDoorSettingView();

        public ManageList()
        {
            InitializeComponent();

            //this.baseConfigure.DataContext = _BaseSetting;

            //_RadioSetting.EnableChanged += new Action<object, bool>(OnRadioSettingEnableChanged);
            //_RadioSetting.QueryTypChanged += new Action<object, LocationQueryType_t>(OnRadioSettingQueryTypChanged);
            //this.radioConfigure.DataContext = _RadioSetting;

            //_RepeaterSetting.EnableChanged += new Action<object, bool>(OnRepeaterSettingEnableChanged);
            //this.repeaterConfigure.DataContext = _RepeaterSetting;

            //_MnisSetting.QueryTypChanged += new Action<object, LocationQueryType_t>(OnMnisSettingQueryTypChanged);
            //this.mnisConfigure.DataContext = _MnisSetting;

            //this.locationConfigure.DataContext = _LocationSetting;
            //this.locationInDoorConfigure.DataContext = _LocationInDoorSetting;

            this.Loaded += new RoutedEventHandler(ManageListLoaded);
            this.manageOptions.PreviewMouseLeftButtonUp += new MouseButtonEventHandler(ManageOptionsPreviewMouseLeftButtonUp);

            //RegisterNotifice();
        }

        //public void RegisterNotifice()
        //{
        //    foreach (TreeViewItem item in this.configureOptions.Items)
        //    {
        //        if (item.Parent is TreeView && item.DataContext != null && item.DataContext is SettingControl)
        //        {
        //            (item.DataContext as SettingControl).Notified += new MessageHandler(OnSettingControlMessageHandler);
        //        }
        //    }
        //}

        //private void  OnSettingControlMessageHandler(object sender, MessageArgs e)
        //{
        //    if (Notified != null) Notified(sender, e);
        //}
        private void ManageListLoaded(object sender, RoutedEventArgs e)
        {
            this.baseConfigure.IsSelected = true;
            _selectedManager = this.baseConfigure.Tag as ManageView;
            _selectedManager.DataContext = this.baseConfigure.DataContext;
            if (SelectedChanged != null) SelectedChanged(this, _selectedManager);
        }

        private ManageView FindManageView(TreeViewItem selectedItem)
        {
            if (selectedItem == null) return null;
            if (selectedItem.Tag != null && selectedItem.Tag is ManageView)
            {
                ManageView manager = selectedItem.Tag as ManageView;
                manager.DataContext = selectedItem.DataContext;
                return manager;
            }

            if (selectedItem.Parent != null && selectedItem.Parent is TreeViewItem)
            {
                ManageView manager = FindManageView(selectedItem.Parent as TreeViewItem);
                if (manager != null) return manager;
            }

            return null;
        }

        private void ManageOptionsPreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            TreeViewItem selectedItem = ((TreeView)sender).SelectedItem as TreeViewItem;
            ManageView manager = FindManageView(selectedItem);
            if (manager != null && manager != _selectedManager)
            {
                _selectedManager = manager;
                if (SelectedChanged != null) SelectedChanged(this, _selectedManager);
            }

            if (selectedItem.Tag != null)
            {
                _selectedManager.ScorllToPart(selectedItem.Tag is string ? selectedItem.Tag.ToString().ToInt() : 0);
            }
        }

        //private void OnRadioSettingEnableChanged(object sender, bool enable)
        //{
        //    if (enable)
        //    {
        //        _RepeaterSetting.Disable();
        //    }
        //}

        //private void OnRadioSettingQueryTypChanged(object sender, LocationQueryType_t type)
        //{
        //    if (!_MnisSetting.Enable)
        //    {
        //        _LocationSetting.SetQueryType(type);
        //        _LocationInDoorSetting.SetQueryType(type);
        //    }
        //}

        //private void OnRepeaterSettingEnableChanged(object sender, bool enable)
        //{
        //    if (enable)
        //    {
        //        _RadioSetting.Disable();
        //    }
        //}

        //private void OnMnisSettingQueryTypChanged(object sender, LocationQueryType_t type)
        //{
        //    _LocationSetting.SetQueryType(type);
        //    _LocationInDoorSetting.SetQueryType(type);
        //}

        //public void Update()
        //{
        //    foreach (TreeViewItem item in this.configureOptions.Items)
        //    {
        //        if (item.Parent is TreeView && item.DataContext != null && item.DataContext is SettingControl)
        //        {
        //            (item.DataContext as SettingControl).UpdateSetting();
        //        }
        //    }
        //}

        //private bool IsCancelSave = false;
        //public bool Save()
        //{
        //    foreach (TreeViewItem item in this.configureOptions.Items)
        //    {
        //        if (item.Parent is TreeView && item.DataContext != null && item.DataContext is SettingControl)
        //        {
        //            SettingControl setting = item.DataContext as SettingControl;
        //            setting.SaveSetting();
        //            if(IsCancelSave)
        //            {
        //                return false;
        //            }
        //        }
        //    }

        //    return true;
        //}

        //private void Notify(string message)
        //{
        //    if (Notified != null) Notified(this, new MessageArgs(Messages.AddNotifyLine, message));
        //}
    }
}