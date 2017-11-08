using Manager.Models;
using Manager.ViewModels;
using System;
using System.Windows;
using System.Windows.Controls;

namespace Manager
{
    /// <summary>
    /// Notify.xaml 的交互逻辑
    /// </summary>
    public partial class Notify : Window
    {
       
        public Notify()
        {
            InitializeComponent();

            Message.Instance().CustomMessageReceived += new CustomMessageHandler(OnCustomMessageReceived);

            this.notifyContents.TextChanged += new TextChangedEventHandler(NotifyContentsTextChanged);
            this.enterButton.Click += new RoutedEventHandler(EnterButtonClick);
            this.closeButton.Click += new RoutedEventHandler(CloseButtonClick);
        }

        private void OnCustomMessageReceived(object sender, CustomMessageArgs e)
        {
            if (e == null) return;
            switch (e.Message)
            {
                case Messages.SetNotifyEnable:
                    SetEnterButtonEnable((bool)e.Parameter);
                    break;

                case Messages.AddNotifyLine:
                    AddLine(e.Parameter as string);
                    break;

                case Messages.ClearNotify:
                    Clear();
                    break;
            }
        }

        private void NotifyContentsTextChanged(object sender, TextChangedEventArgs e)
        {
            notifyScroll.ScrollToEnd();
            (sender as TextBox).ScrollToEnd();
        }

        private void EnterButtonClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void CloseButtonClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        public void SetEnterButtonEnable(bool enable)
        {
            this.closeButton.Dispatcher.BeginInvoke(new Action(() =>
            {
                this.closeButton.IsEnabled = enable;
                this.enterButton.IsEnabled = enable;
            }));
        }

        public void AddLine(string message)
        {
            if (message == null) return;
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                this.notifyContents.Text += message + "\r\n";
            }));
        }

        public void Clear()
        {
            this.notifyContents.Dispatcher.BeginInvoke(new Action(() =>
            {
                this.notifyContents.Text = "";
            }));
        }
    }
}