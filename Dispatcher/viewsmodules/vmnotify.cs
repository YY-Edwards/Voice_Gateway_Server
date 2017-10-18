using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using Sigmar;
using Sigmar.Logger;
using Sigmar.Extension;
using Dispatcher.Service;

namespace Dispatcher.ViewsModules
{
    public class VMNotify :INotifyPropertyChanged
    {      
        private static int MaxNotifyCount = 500;

        private List<VMNotice> _notices = new List<VMNotice>();

        public ICollectionView Alarm { get { return new ListCollectionView(_notices.FindAll(p => p.Type == NotifyKey_t.Alarm)); } }
        public ICollectionView Called { get { return new ListCollectionView(_notices.FindAll(p => p.Type == NotifyKey_t.Called)); } }
        public ICollectionView ShortMessage { get { return new ListCollectionView(_notices.FindAll(p => p.Type == NotifyKey_t.ShortMessage)); } }
        public ICollectionView JobTicket { get { return new ListCollectionView(_notices.FindAll(p => p.Type == NotifyKey_t.JobTicket)); } }
        public ICollectionView Patrol { get { return new ListCollectionView(_notices.FindAll(p => p.Type == NotifyKey_t.Patrol)); } }


        public ICommand Add { get { return new Command(AddExec); } }
        public void AddExec(object parameter)
        {
            if (parameter == null && !(parameter is VMNotice)) return;
            VMNotice notice = parameter as VMNotice;
            if (_notices.Count >= MaxNotifyCount)
            {
                string needupdate = _notices[0].Type.ToString();              
                _notices.RemoveAt(0);
                NotifyPropertyChanged(needupdate);
            }

            _notices.Add(notice);
            NotifyPropertyChanged(notice.Type.ToString());
        }


        public ICommand Remove { get { return new Command(RemoveExec); } }
        public void RemoveExec(object parameter)
        {
            if (parameter == null && !(parameter is VMNotice)) return;
            _notices.RemoveAll(p => p == parameter as VMNotice);
            NotifyPropertyChanged((parameter as VMNotice).Type.ToString());
        }

        public ICommand Clear { get { return new Command(ClearExec); } }
        public void ClearExec(object parameter)
        {
            if (parameter == null && !(parameter is string)) return;
            NotifyKey_t key = (parameter as string).ToEnum<NotifyKey_t>();
            _notices.RemoveAll(p => p.Type == key);
            NotifyPropertyChanged(key.ToString());           
        }

        public ICommand Open { get { return new Command(OpenExec); } }
        public void OpenExec(object parameter)
        {
            if (parameter == null && !(parameter is VMNotice)) return;
            VMNotice notice = parameter as VMNotice;
            VMTarget source = notice.Remote;
            if(source == null)return;

            int index = source.Notices.FindIndex(p => p == notice);
            source.CurrentNotifyIndex = index < 0 ? (source.Notices.Count - 1) : index;
            switch(notice.Type)
            {
                case NotifyKey_t.ShortMessage:
                    source.OpenOperateWindow.Execute(TaskType_t.ShortMessage.ToString());
                    break;
                case NotifyKey_t.JobTicket:
                    source.OpenOperateWindow.Execute(TaskType_t.JobTicket.ToString());
                    break;
                case NotifyKey_t.Patrol:
                    source.OpenOperateWindow.Execute(TaskType_t.Patrol.ToString());
                    break;
                default:
                    source.OpenOperateWindow.Execute(TaskType_t.Schedule.ToString());
                    break;
            }
        }

        public VMNotify()
        {
          
        }


        public event OperatedEventHandler OnOperated;
        public event OperatedEventHandler OnViewModulesOperated;
        public void UpdateView(OperatedEventArgs e)
        {
            if(OnViewModulesOperated != null)OnViewModulesOperated(e);
        }

        public void OnNotifyHidden()
        {
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.CloseAllNotify));
        }

        public ICommand Close { get { return new Command(CloseExec); } }
        private void CloseExec(object parameter)
        {
            if (parameter == null || !(parameter is string)) return;
            NotifyKey_t key = (parameter as string).ToEnum<NotifyKey_t>();
            if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.CloseNotify, key));
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

        #endregion


        public class VMNotice
        {
            
            private VMTarget _remote;
            private CNotice _notice;
            private bool _istx;
            public VMNotice(VMTarget remote, CNotice notice, bool istx = true)
            {
                _remote = remote;
                _notice = notice;
                _istx = istx;
            }


            public NotifyKey_t Type { get { return _notice.Type; } }
            public DateTime Time { get { return _notice.Time; } }
            public VMTarget Remote { get { return _remote; } }
            public bool IsTx { get { return _istx; } }
            public string ContentBiref { get { return _notice.Contents.ToString(); } }


            public ImageSource Icon
            {
                get
                {
                    switch (_notice.Type)
                    {
                        case NotifyKey_t.Alarm:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/alarm_18_18.png"));
                        case NotifyKey_t.Called:
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/call_33_34.png"));
                        case NotifyKey_t.ShortMessage:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/message_34_34.png"));
                        case NotifyKey_t.Control:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/control_43_43.png"));
                        case NotifyKey_t.Location:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/positon_29_38.png"));
                        case NotifyKey_t.LocationInDoor:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/locationindoor_32_32.png"));
                        case NotifyKey_t.JobTicket:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/job_tickets_29_36.png"));
                        case NotifyKey_t.Patrol:
                            return new BitmapImage(new Uri("pack://application:,,/resource/images/tacker_43_43.png"));

                    }
                    return null;
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

            #endregion
        }

    }
}
