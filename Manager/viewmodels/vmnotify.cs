using System;
using System.Collections.Generic;
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
    public class CVMNotify:INotifyPropertyChanged
    {
        //Command
        #region
        public ICommand Enter { get { return new CDelegateCommand(EnterFunc); } }
        public ICommand Close { get { return new CDelegateCommand(CloseWin); } }

        public ICommand ConnectChanged { get { return new CDelegateCommand(OnConnectChanged); } }

        #endregion


        //Property
        #region
        private bool m_EnterEnable = true;
        public bool EnterEnable { get { return m_EnterEnable; } }

        private string m_NotifyText = string.Empty;
        public string NotifyText { get { return m_NotifyText; } }

        #endregion

        private bool m_IsLocked = false;
        private string m_LockStr = string.Empty;

        public event PropertyChangedEventHandler PropertyChanged;


        //windows Method
        #region
        private void EnterFunc(object parameter)
        {
           if (parameter == null || !(parameter is Notify)) return;
           try
           {
               ((Notify)parameter).DialogResult = true;
           }
           catch
           {

           }
           ((Notify)parameter).Close();
        }

        private void CloseWin(object parameter)
        {
            if (parameter == null || !(parameter is Notify)) return;
            try
            {
                ((Notify)parameter).DialogResult = false;
            }
            catch
            {

            }
            ((Notify)parameter).Close();
        }

        private void OnConnectChanged(object parameter)
        {
            if (parameter == null || !(parameter is TextBox)) return;
            TextBox txt = parameter as TextBox;

            txt.ScrollToEnd();          
        }

        public void SetEnterEnable(bool enable)
        {
            m_EnterEnable = enable;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("EnterEnable"));
        }

        public void Notify(string msg)
        {
            m_NotifyText = msg;
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("NotifyText"));
        }

        public void AppendNotify(string msg)
        {
            if (m_IsLocked)
            {
                m_NotifyText = m_LockStr + "\r\n" + msg;
            }
            else
            {
                m_NotifyText += "\r\n" + msg;
            }
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("NotifyText"));
        }


        public void Lock()
        {
            m_LockStr = m_NotifyText;
            m_IsLocked = true;
        }

        public void UnLock()
        {
            m_LockStr = string.Empty;
            m_IsLocked = false ;
        }
        #endregion
    }
}
