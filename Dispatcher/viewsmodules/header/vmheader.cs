using Dispatcher.Service;
using Sigmar;
using Sigmar.Logger;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;

namespace Dispatcher.ViewsModules
{
    public class VMHeader :  INotifyPropertyChanged
    {
        private VMMenu _menuviewmodule;

        public VMMenu MenuViewModule
        {
            get
            {
                return _menuviewmodule;
            }
            private set
            {
                _menuviewmodule = value;
                NotifyPropertyChanged("MenuViewModule");
            }
        }

        public string CurrentUser { get { return "JHDEV8"; } }

        private Visibility _restorevisible = Visibility.Hidden;
        public Visibility RestoreButtonVisble { get { return _restorevisible; } }
        public Visibility MaxButtonVisble { get { return _restorevisible == Visibility.Visible ? Visibility.Hidden : Visibility.Visible; } }

        public event OperatedEventHandler OnOperated;

        public VMHeader()
        {
            MenuViewModule = new VMMenu();
            MenuViewModule.OnOperated += new OperatedEventHandler(OnMenuOperated);
        }

        private void OnMenuOperated(OperatedEventArgs e)
        {
            if (OnOperated != null) OnOperated(e);
        }

        public ICommand WindowLoaded { get { return new Command(WindowLoadedExec); } }

        private void WindowLoadedExec(object parameter)
        {
            if (parameter != null && parameter is Window)
            {
                Window win = parameter as Window;

                if (win.WindowState == WindowState.Maximized) _restorevisible = Visibility.Visible;
                else _restorevisible = Visibility.Hidden;

                this.NotifyPropertyChanged("RestoreButtonVisble");
                this.NotifyPropertyChanged("MaxButtonVisble");
            }
        }

        public ICommand WindowSizeChanged { get { return new Command(WindowSizeChangedExec); } }

        private void WindowSizeChangedExec(object parameter)
        {
            if (parameter != null && parameter is Window)
            {
                Window win = parameter as Window;

                if (win.WindowState == WindowState.Maximized) _restorevisible = Visibility.Visible;
                else _restorevisible = Visibility.Hidden;

                this.NotifyPropertyChanged("RestoreButtonVisble");
                this.NotifyPropertyChanged("MaxButtonVisble");
            }
        }

        public ICommand Move { get { return new Command(MoveHeader); } }

        private void MoveHeader()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowMove));
        }

        public ICommand Min { get { return new Command(MinHeader); } }

        private void MinHeader()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowMinimize));
        }

        public ICommand Max { get { return new Command(MaxHeader); } }

        private void MaxHeader()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowMaximize));
        }

        public ICommand Restore { get { return new Command(RestoreHeader); } }

        private void RestoreHeader()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowRestore));
        }

        public ICommand Close { get { return new Command(CloseHeader); } }

        private void CloseHeader()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.WindowClose));
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion INotifyPropertyChanged Members
    }
}