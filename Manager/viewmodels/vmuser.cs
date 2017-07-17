using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;

namespace Manager
{
    public class CVMUser:CVMManager, INotifyPropertyChanged
    {
        public CVMUser()
        {
            if (m_User == null)
            {
                m_User = new CUserMgr();
                m_User.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditUser == null) m_EditUser = new CUser();           
        }


        private CUserMgr m_User;
        public ObservableCollection<CRElement> Users { get { return new ObservableCollection<CRElement>(m_User.List); } }

        private CUser m_EditUser;
        public CUser EditUser
        {
            get { return m_EditUser; }
            set
            {
                if (value == null)
                {
                    m_EditUser = new CUser();
                    m_User.IsNew = true;
                }
                else
                {
                    m_EditUser = value;
                    m_User.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("user_UserName"));
                    PropertyChanged(this, new PropertyChangedEventArgs("user_Type_index"));
                }
            }
        }

        public string user_UserName { set { m_EditUser.UserName = value; } get { if (m_EditUser == null)return null; return m_EditUser.UserName; } }
        public int user_Type_index { set { m_EditUser.Type = value == 1 ? UserType.Admin : UserType.Guest; } get { if (m_EditUser == null)return -1; return m_EditUser.Type == UserType.Admin ? 1 : 0; } }

        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.user && PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Users"));
            }
        }


        //command
        public ICommand New { get { return new CDelegateCommand(NewUser); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteUser); } }
        public ICommand Save { get { return new CDelegateCommand(SaveUser); } }


        private void NewUser()
        {
            m_User.IsNew = true;
            m_EditUser = new CUser();
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("EditUser"));
                PropertyChanged(this, new PropertyChangedEventArgs("user_UserName"));
                PropertyChanged(this, new PropertyChangedEventArgs("user_Type_index"));
            }

        }

        private void DeleteUser()
        {
            if (m_EditUser == null) return;
            m_User.Delete(m_EditUser.ID);
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Users"));
            }

        }

        //parameter:password,can not binding on passwordbox
        private void SaveUser(object parameter)
        {
            if (m_EditUser == null) return;

            try
            {
                object[] objs = parameter as object[];
                if (objs == null || objs.Length < 2) return;
                if (objs[0] == null || objs[1] == null) return;
                if (!(objs[0] is ListView) || !(objs[1] is PasswordBox)) return;

                ListView lst = objs[0] as ListView;
                PasswordBox pbx = objs[1] as PasswordBox;

                m_EditUser.Password = pbx.Password;
                if (m_User.IsNew)
                {
                    m_User.Add(m_EditUser);
                }
                else
                {
                    m_User.Modify(m_EditUser.ID, m_EditUser);
                }

                pbx.Password = string.Empty;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Users"));
                }

                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_User.IsNew = false;
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_User.Save();
        }

        public void Get()
        {
            m_User.Query();
        }

    }
}
