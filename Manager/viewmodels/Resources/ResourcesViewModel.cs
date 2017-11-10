using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{
    public abstract class ResourcesViewModel<T> :SaveReadableElement, INotifyPropertyChanged
        where T : RElement,new()
    {
        private T _selectedele;

        protected string _resourcesName;
        public bool IsChanged { get; set; }
        public RequestOpcode ResourceOpcode { get { return _resources.ResourceOpcode; } set { _resources.ResourceOpcode = value; } }
        public string RecordsName { get { return _resources.RecordsName; } set { _resources.RecordsName = value; } }


        public T SelectedElement
        { 
            get
            {
                return _selectedele;
            }
            set 
            { 
                _selectedele = value;
                if(_selectedele != null)
                {
                    T backup = _selectedele.Copy() as T;
                    if (backup != null) Element = backup;
                    else Element = new T();               
                } 
                else if(Element == null)
                {
                    Element = new T();
                }

                OnSelectedChanged();
            } 
        }
              

        protected T Element;
        protected List<T> _elements;

        public ICollectionView Elements { get { return new ListCollectionView(_elements); } }

        protected ResourcesManage _resources;
        private bool _isTimeout;

        public ResourcesViewModel()
        {
            _resourcesName = string.Empty;
            IsChanged = false;
            Element = new T();
            _elements = new List<T>();

            if (_resources == null)
            {
                _resources = new ResourcesManage();
                _resources.ResourceChanged += new Action<object, object>(OnResourceChanged);
                _resources.Timeout += new Action<object>(OnResourcesTimeout);
            }

            _isTimeout = false;

            this.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Elements") OnResourceChanged();          
        }

        private void OnResourcesTimeout(object sender)
        {
            _isTimeout = true;
        }

        private void OnResourceChanged(object sender, object elements)
        {
            if (elements != null && elements is List<T>)
            {
                _elements = elements as List<T>;

                if (_elements != null) NotifyPropertyChanged("Elements");
               
            }
        }

        public override void Read()
        {
            //if (!_isTimeout && _resources != null) _resources.Read<T>();
            if (_resources != null) _resources.Read<T>();
        }

        public override SaveStatus Save()
        {
            SaveStatus status = SaveStatus.Failure;

            if (!IsChanged) status = SaveStatus.Skip;
            //else if (!_isTimeout && _resources != null)
            else if (_resources != null)
            {
                IsChanged = false;
                status = _resources.Save() ? SaveStatus.Success : SaveStatus.Failure;
            }

            if (status != SaveStatus.Skip)
            {
                string notify = string.Format(Resource.SaveResult, _resourcesName, status == SaveStatus.Success ? Resource.SaveSuccess : Resource.SaveFailure);
                Message.Instance().PostCustomMessage(this, new CustomMessageArgs(Messages.AddNotifyLine, notify));
            }

            return status;
        }

        protected abstract void OnResourceChanged();
        protected abstract void OnSelectedChanged();
        private void CreateElement()
        {
            Element.ID = (DateTime.Now - new DateTime(2000, 1,1)).Ticks;
            _resources.Insert(Element);

            if (_elements == null) _elements = new List<T>();
            _elements.Add(Element);
            NotifyPropertyChanged("Elements");

            SelectedElement = Element;
            NotifyPropertyChanged("SelectedElement");

            IsChanged = true;
        }

        private void ModifyElement()
        {
            if (_selectedele == null) return;
            Element.ID = _selectedele.ID;

            _resources.Update(Element);
            if (_elements != null)
            {
                int index = _elements.FindIndex(p => p.ID == Element.ID);
                if (index >= 0 && index < _elements.Count)
                {
                    _elements[index] = Element;
                    NotifyPropertyChanged("Elements");

                    SelectedElement = Element;
                    NotifyPropertyChanged("SelectedElement");
                }
            }

            IsChanged = true;
        }


        private void DeleteElement()
        {
            if (_selectedele == null) return;

            _resources.Delete(_selectedele);

            if (_elements != null)
            {
                int index = _elements.FindIndex(p => p.ID == _selectedele.ID);
                if (index >= 0 && index < _elements.Count)
                {
                    _elements.RemoveAt(index);
                    NotifyPropertyChanged("Elements");

                    SelectedElement = index - 1 >= 0 ? _elements[index - 1] : null;
                    NotifyPropertyChanged("SelectedElement");
                }
            }

            IsChanged = true;
        }


        public ICommand Create
        {
            get
            {
                return new Command(CreateElement);
            }
        }

        public ICommand Modify
        {
            get
            {
                return new Command(ModifyElement);
            }
        }

        public ICommand Delete
        {
            get
            {
                return new Command(DeleteElement);
            }
        }


        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;
        protected void NotifyPropertyChanged(string propertyName)
        {
            if (propertyName != null && this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        protected void NotifyPropertyChanged(string[] propertyNames)
        {
            if (propertyNames != null && this.PropertyChanged != null)
            {
                foreach (string name in propertyNames) if (name != null) this.PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }

        #endregion
    }
}
