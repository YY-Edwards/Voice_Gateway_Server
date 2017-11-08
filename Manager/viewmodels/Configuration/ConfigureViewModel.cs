using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using Manager.Models;

namespace Manager.ViewModels
{
    public abstract class ConfigureViewModel<T> : SaveReadableElement, INotifyPropertyChanged
        where T:class,new()
    {
        protected string _configurationName;
        protected bool IsChanged { get; set; }
        protected RequestOpcode ReadOpcode { get { return _configure.ReadOpcode; } set { _configure.ReadOpcode = value; } }
        protected RequestOpcode SaveOpcode { get { return _configure.SaveOpcode; } set { _configure.SaveOpcode = value; } }

        public T _configuration;
        private Configure _configure;
        private bool _isTimeout;

        public ConfigureViewModel()
        {
            _configurationName = string.Empty;
            IsChanged = false;
            _configuration = new T();
            if(_configure == null)
            {
                _configure = new Configure();
                _configure.ReceivedConfiguration += new Action<object, object>(OnReceivedConfguration);
                _configure.Timeout += new Action<object>(OnConfigureTimeout);
            }

            _isTimeout = false;
        }

        private void OnConfigureTimeout(object sender)
        {
            _isTimeout = true ;
        }

        private void OnReceivedConfguration(object sender, object configuration)
        {
            if (configuration != null && configuration is T)
            {
                _configuration = configuration as T;
                OnConfgurationChanged();
            }
        }
     
        public override void Read()
        {
            if (!_isTimeout && _configure != null) _configure.Read<T>();
        }
        public override SaveStatus Save()
        {
             SaveStatus status = SaveStatus.Failure;

            if (!IsChanged) status = SaveStatus.Skip;
            else if (!_isTimeout && _configure != null)
            {
                IsChanged = false;
                status = _configure.Save(_configuration) ? SaveStatus.Success : SaveStatus.Failure;
            }
            
             if (status != SaveStatus.Skip)
            {
                string notify = string.Format(Resource.SaveResult, _configurationName, status == SaveStatus.Success ? Resource.SaveSuccess : Resource.SaveFailure);
                Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.AddNotifyLine, notify));
            }

            return status;
        }


        protected abstract void OnConfgurationChanged();
 

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            if (propertyName != null && this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        public void NotifyPropertyChanged(string[] propertyNames)
        {
            if (propertyNames != null && this.PropertyChanged != null)
            {
                foreach (string name in propertyNames) if (name != null) this.PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }

        #endregion
    }
}
