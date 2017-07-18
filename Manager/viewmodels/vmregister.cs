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
    public class CVMRegister : CVMManager, INotifyPropertyChanged
    {

        public CVMRegister()
        {
            if (m_Reg == null)
            {
                m_Reg = new CRegister();
                m_Reg.OnQuery += OnQuery;
                m_Reg.OnRegister += OnRegister;

            }
          
        }

        #region

        public ICommand BrowseKey { get { return new CDelegateCommand(BrowseKeyMethod); } }
        public ICommand Register { get { return new CDelegateCommand(SendKey); } }
        public ICommand EnableRepeater { get { return new CDelegateCommand(EnableRepeaterExec); } }
        public ICommand EnableRadio { get { return new CDelegateCommand(EnableRadioExec); } }
        #endregion


        //basesetting
        private CRegister m_Reg;

        private string m_Type = "车载台";
        public string DeviceType { get { return m_Type; } }
        public string DeviceSN { get {
            if (!m_Reg.IsConnectedDevice) return "获取序列号失败";
            switch(m_Reg.DeviceType)
            {
                case Device.Repeater: return m_Reg.RepeaterSerial;
                default: return m_Reg.RadioSerial;
            } 
        }}

        public string Status { get 
        {
            string res = string.Empty;

            if (!m_Reg.IsRegistered) res = "（未注册）";
            else
            {
                if(m_Reg.IsEver == 0)
                {
                    res = "（已注册," + m_Reg.Expiration.ToString("yyyy年Mm月dd日") + "前有效）";
                }
                else
                {
                    res = "（已注册）";
                }
            }
            return res;        
        }}

        private string m_LicenseKey = string.Empty;
        public string LicenseKey { get { return m_LicenseKey; } set { m_LicenseKey = value; } }

        private string m_Notify = string.Empty;
        public string Notify{ get { return m_Notify;} }

        private Visibility m_NotifyVisible = Visibility.Collapsed;
         public Visibility NotifyVisible{ get { return m_NotifyVisible;} }
       
        private void OnQuery(bool success, CRegister res)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("DeviceType"));
                PropertyChanged(this, new PropertyChangedEventArgs("DeviceSN"));
                PropertyChanged(this, new PropertyChangedEventArgs("Status"));
            }
        }

        private void OnRegister(bool success, CRegister res)
        {            
            if (!success)
            {
                m_Notify =  "注册失败。";
                       
            }
            else
            {
                m_Notify =  "注册成功。";
            }

            m_NotifyVisible = Visibility.Visible;       

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Notify"));
                PropertyChanged(this, new PropertyChangedEventArgs("NotifyVisible"));
                PropertyChanged(this, new PropertyChangedEventArgs("Status"));
            }
        }

        private void BrowseKeyMethod()
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.Title = "选择文件";
            openFileDialog.Filter = "注册文件|*.lic|所有文件|*.*";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;
            System.Windows.Forms.DialogResult result = openFileDialog.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.Cancel)
            {
                return;
            }

            m_LicenseKey = System.IO.File.ReadAllText(openFileDialog.FileName);
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("LicenseKey"));

        }

        private void SendKey()
        {
            m_NotifyVisible = Visibility.Collapsed;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("NotifyVisible"));
            }
            m_Reg.SendKey(m_LicenseKey);
        }

        private void EnableRepeaterExec()
        {
            m_Type = "中继台";
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("DeviceType"));
        }
        private void EnableRadioExec()
        {
            m_Type = "车载台";
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("DeviceType"));
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;

        public void Set()
        {
            m_Reg.Set();
        }

        public void Get()
        {
            m_Reg.Get();
        }
    }
}
