using Sigmar;
using Sigmar.Extension;
using Dispatcher;
using System;
using System.ComponentModel;
using System.Windows.Input;


using System.Windows;
using Dispatcher.Service;
using Dispatcher.Modules;

namespace Dispatcher.ViewsModules
{
    public class VMToolBar : INotifyPropertyChanged
    {
        public event OperatedEventHandler OnOperated;
        private SaveWork _savework;

        public SaveWork Save {get{return _savework;}}
        public VMToolBar()
        {
            if (_savework == null)
            {
                _savework = SaveWork.Instance();
            }
            InitilizeViewTools();
        }

        public bool EnableControlFunc { get { return TargetViewModule != null && TargetViewModule.EnableControlFunc ? true : false; } }
        public bool EnableBaseFunc { get { return TargetViewModule != null  ? true : false; } }
        public bool EnableLocationInDoorFunc { get { return TargetViewModule != null && TargetViewModule.EnableLocationInDoorFunc ? true : false; } }


        //public ICommand UpdateSaveStatus { get { return new Command(UpdateSaveStatusExec); } }

        //private void UpdateSaveStatusExec()
        //{
        //    NotifyPropertyChanged("EnableSaveWorkSpace");
        //}

        public ICommand NewOperate { get { return new Command(NewOperateExec); } }

        private void NewOperateExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                TaskType_t newtype = ((string)parameter).ToEnum<TaskType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenNewOperateWindow, newtype));
            }
        }

        public ICommand SaveWorkspace { get { return new Command(SaveWorkspaceExec); } }

        private void SaveWorkspaceExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.SaveWorkspace));
        }

        public ICommand ExportWorkspace { get { return new Command(ExportWorkspaceExec); } }

        private void ExportWorkspaceExec()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.ExportWorkspace));
        }

        //Target
        private VMTarget _targetviewmodule;

        public VMTarget TargetViewModule { get { return _targetviewmodule; } private set { 
            _targetviewmodule = value; 
            NotifyPropertyChanged("TargetViewModule");
            NotifyPropertyChanged("EnableControlFunc");
            NotifyPropertyChanged("EnableBaseFunc");
            NotifyPropertyChanged("EnableLocationInDoorFunc");
        }
        }

        public ICommand SetTarget { get { return new Command(SetTargetExec); } }
        public void SetTargetExec(object parameter)
        {
            TargetViewModule = parameter as VMTarget;
        }

        public ICommand ToolsFast { get { return new Command(ToolsFastExec); } }

        private void ToolsFastExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                QuickPanelType_t key = ((string)parameter).ToEnum<QuickPanelType_t>();
                if (key == QuickPanelType_t.Target && _targetviewmodule != null)
                {
                    _targetviewmodule.AddFastPanel.Execute(null);
                }
                else
                {
                    if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenNewFastWindow, key));
                } 
            };
        }

        public ICommand Help { get { return new Command(HelpExec); } }

        private void HelpExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                HelpWindowType_t key = ((string)parameter).ToEnum<HelpWindowType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenHelpWindow, key));
            };
            
        }

        private bool _enableViewBaseTools;
        private bool _enableViewFastTools;
        private bool _enableViewControlerTools;
        private bool _enableViewLocationInDoorTools;
        private bool _enableViewHelpTools;

        public Visibility ViewBaseVisible { get { return _enableViewBaseTools ? Visibility.Visible : Visibility.Collapsed; }  }
        public Visibility ViewFastVisible { get { return _enableViewFastTools ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewControlerVisible { get { return _enableViewControlerTools ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewLocationInDoorVisible { get { return _enableViewLocationInDoorTools ? Visibility.Visible : Visibility.Collapsed; } }
        public Visibility ViewHelpVisible { get { return _enableViewHelpTools ? Visibility.Visible : Visibility.Collapsed; } }

        private void InitilizeViewTools()
        {
            _enableViewBaseTools = FunctionConfigure.EnableViewBaseTools;
            _enableViewFastTools = FunctionConfigure.EnableViewFastTools;
            _enableViewControlerTools = FunctionConfigure.EnableViewControlerTools;
            _enableViewLocationInDoorTools = FunctionConfigure.EnableViewLocationInDoorTools;
            _enableViewHelpTools = FunctionConfigure.EnableViewHelpTools;

            NotifyPropertyChanged("ViewBaseVisible");
            NotifyPropertyChanged("ViewFastVisible");
            NotifyPropertyChanged("ViewControlerVisible");
            NotifyPropertyChanged("ViewLocationInDoorVisible");
            NotifyPropertyChanged("ViewHelpVisible");
        }

        public void UpdateView(OperatedEventArgs e)
        {
            try
            {
                switch ((ToolsKey_t)e.parameter)
                {
                    case ToolsKey_t.Base:
                        _enableViewBaseTools = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("ViewBaseVisible");
                        break;
                    case ToolsKey_t.Fast:
                        _enableViewFastTools = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("ViewFastVisible");
                        break;
                    case ToolsKey_t.Controler:
                        _enableViewControlerTools = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("ViewControlerVisible");
                        break;
                    case ToolsKey_t.LocationInDoor:
                        _enableViewLocationInDoorTools = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("ViewLocationInDoorVisible");
                        break;
                    case ToolsKey_t.Help:
                        _enableViewHelpTools = e.Operate == OperateType_t.OpenTools ? true : false;
                        NotifyPropertyChanged("ViewHelpVisible");
                        break;
                    default:
                        break;
                }
            }
            catch (Exception ex)
            {
                //WARNING("There is not type of " + e.parameter.ToString() + " tools");
            }
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