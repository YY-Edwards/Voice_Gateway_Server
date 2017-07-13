using System;
using System.Windows.Input;

namespace Manager
{
    internal class CDelegateCommand : ICommand
    {
        private Action WhattoExecute;
        private Func<bool> WhentoExecute;

        private Action<object> WhattoExecuteWithParameter;
        private Func<object, bool> WhentoExecuteWithParameter;

        private bool _IsCanExecute = true;

        public CDelegateCommand(bool IsCanExecute, Action Execute) // Point 1
        {
            _IsCanExecute = IsCanExecute;
            WhattoExecute = Execute;
        }

        public CDelegateCommand(Action Execute) // Point 1
        {
            WhattoExecute = Execute;
            _IsCanExecute = true;
        }

        public CDelegateCommand(Func<bool> CanExecute, Action Execute) // Point 1
        {
            _IsCanExecute = false;

            WhattoExecute = Execute;
            WhentoExecute = CanExecute;
        }


        public CDelegateCommand(bool IsCanExecute, Action<object> Execute) // Point 1
        {
            _IsCanExecute = IsCanExecute;
            WhattoExecuteWithParameter = Execute;
        }

        public CDelegateCommand(Action<object> Execute) // Point 1
        {
            WhattoExecuteWithParameter = Execute;
            _IsCanExecute = true;
        }

        public CDelegateCommand(Func<object, bool> CanExecute, Action<object> Execute) // Point 1
        {
            _IsCanExecute = false;

            WhattoExecuteWithParameter = Execute;
            WhentoExecuteWithParameter = CanExecute;
        }

        public bool CanExecute(object parameter)
        {
            if (_IsCanExecute) return true;
            if (WhentoExecute != null) return WhentoExecute();
            else if (WhentoExecuteWithParameter != null) return WhentoExecuteWithParameter(parameter);
            return false;
        }

        public void Execute(object parameter)
        {
            if (WhattoExecute != null) WhattoExecute();
            else if (WhattoExecuteWithParameter != null) WhattoExecuteWithParameter(parameter);
        }

        public event EventHandler CanExecuteChanged;
    }
}