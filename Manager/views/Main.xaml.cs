using Manager.Models;
using Manager.ViewModels;
using Sigmar;
using Sigmar.Logger;
using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace Manager.Views
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class Main : Window, IToggleLogable
    {
        private MainViewModel MainViewModels;

        public Main()
        {
            InitializeComponent();

            Message.Instance().CustomMessageReceived += new CustomMessageHandler(OnCustomMessageReceived);

            if (MainViewModels == null) MainViewModels = new MainViewModel();
            this.DataContext = MainViewModels;

            this.Loaded += new RoutedEventHandler(WindowLoaded);
            this.Closed += new EventHandler(WindowClosed);

            this._manageList.SelectedChanged += new Action<object, ManageView>(ManageListSelectedChanged);

            this.headerCatcher.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(HeaderCatcherPreviewMouseLeftButtonDown);
            this.windowMinimize.Click += new RoutedEventHandler(WindowMinimizeClick);
            this.windowClose.Click += new RoutedEventHandler(WindowCloseClick);

            this.windowCancal.Click += new RoutedEventHandler(WindowCloseClick);

            this.windowMask.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(WindowMaskPreviewMouseLeftButtonDown);          
        }
        
        private void OnCustomMessageReceived(object sender, CustomMessageArgs e)
        {
            if (e == null) return;
            switch (e.Message)
            {
                case Messages.OpenNotifyWindow:
                    OpenNotifyWindow();
                    break;
            }
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            Logs.SetToggleWindow(this);
            MainViewModels.ConnectServer.Execute(null);
        }


        private void WindowClosed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void ManageListSelectedChanged(object sender, ManageView e)
        {
            if (e == null) return;
            this.settingContent.Content = e;
        }

        private void HeaderCatcherPreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void WindowCloseClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void WindowMinimizeClick(object sender, RoutedEventArgs e)
        {
            this.WindowState = System.Windows.WindowState.Minimized;
        }

        private void WindowMaskPreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private Notify notifyWindow = new Notify();

        private void OpenNotifyWindow()
        {
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                notifyWindow.Owner = this;
                notifyWindow.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterOwner;
                this.windowMask.Visibility = Visibility.Visible;
                notifyWindow.ShowDialog();

                notifyWindow = new Notify();
                this.windowMask.Visibility = Visibility.Hidden;
            }));
        }

        #region LogToggle

        public event Action LogToggled;

        public ICommand ToggleLogger { get { return new Command(() => { if (LogToggled != null)LogToggled(); }); } }

        #endregion LogToggle
    }
}