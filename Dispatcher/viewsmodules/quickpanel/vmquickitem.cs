using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Media;

using Sigmar;
using Dispatcher;
using Dispatcher.Service;
using Dispatcher.Views;
using Dispatcher.Modules;

namespace Dispatcher.ViewsModules
{
    public class VMQuickItem : INotifyPropertyChanged
    {
        private VMTarget _target;
        private VMOperation _operation;

        private QuickPanelType_t _type;

        public VMQuickItem(VMTarget target)
        {
            _target = target;
            _type = QuickPanelType_t.Target;
            _target.PropertyChanged += new PropertyChangedEventHandler(OnTargetPropertyChanged);
        }

        public VMQuickItem(VMOperation operation)
        {
            _operation = operation;
            _type = QuickPanelType_t.Operation;
            operation.PropertyChanged += new PropertyChangedEventHandler(OnOperationPropertyChanged);
        }

        public VMTarget Target{get{return _target;}}
        public VMOperation Operation { get { return _operation; } }


        public int TypeIndex { get { return _type == QuickPanelType_t.Target ? 0 : 1; } }
        public object ViewModule { get { return _type == QuickPanelType_t.Target ? _target as object : _operation as object; } }

        public Visibility WaitIconVisible { get { return _type == QuickPanelType_t.Target ? _target.WaitIconVisible : Visibility.Collapsed; } }
        public Visibility FailureIconVisible { get { return _type == QuickPanelType_t.Target ? _target.FailureIconVisible : Visibility.Collapsed; } }




        public QuickPanelType_t Type { get { return _type; } }

        private void OnTargetPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            NotifyPropertyChanged("IconBackGround");

            if (e.PropertyName == "WaitIconVisible") NotifyPropertyChanged("WaitIconVisible");
            else if (e.PropertyName == "FailureIconVisible") NotifyPropertyChanged("FailureIconVisible");
            //NotifyPropertyChanged("Icon");
        }

        private void OnOperationPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            NotifyPropertyChanged("IconBackGround");
        }


        public string  Name{get{return _type == QuickPanelType_t.Target ? _target.Name:_operation.TargetName;}}
        public ImageSource  Icon{get{return _type == QuickPanelType_t.Target ? _target.BigIcon:_operation.Icon;}}

        public SolidColorBrush IconBackGround 
        {
            get
            {
                if(_type == QuickPanelType_t.Target)
                {
                    if (!_target.IsOnline) return LocalStyle.TargetPanelOffineBackground;
                    else if (_target.IsInCall) return LocalStyle.TargetPanelInCallBackground;
                    else return LocalStyle.TargetPanelBackground;
                }
                else
                {
                    if (!_operation.IsEnable) return LocalStyle.OperationPanelOffineBackground;
                    else return LocalStyle.OperationPanelBackground;
                }
            }
        }

        public bool Equal(VMQuickItem dest)
        {
            if (Type != dest.Type) return false;
            else if (dest.Type == QuickPanelType_t.Target) return Target == dest.Target;
            else if (dest.Type == QuickPanelType_t.Operation) return Operation.Equal(dest.Operation);
           
            return true;
        }

        public event EventHandler Closed;

        public ICommand Close { get { return new Command(CloseExec); } }
        public void CloseExec()
        {
            if (Closed != null) Closed(this, new EventArgs());
        }




        #region INotifyPropertyChanged

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion
    }
}
