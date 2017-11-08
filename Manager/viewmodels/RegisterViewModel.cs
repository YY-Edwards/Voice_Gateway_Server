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

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{
    public class RegisterViewModel :  INotifyPropertyChanged
    {
        private Register.License_t _licenseStatus;
        private Register _register;
        private bool _isTimeout;

        private bool _isRegiser;
        private Register.Device_t _deviceType;

        private string _registerResult;
        private Visibility _registerResultVisible;

        public RegisterViewModel()
        {
            _isRegiser = false;

            if(_register == null)
            {
                _register = new Register(); 
                _register.ReceivedLicenseMessage += new Action<object, bool, Register.License_t>(OnReceivedLicenseMessage);
                _register.ReceivedRegisterReply += new Action<object, bool, Register.License_t>(OnReceivedRegisterReply);
                _register.Timeout += new Action<object>(OnRegisterTimeout);
            }

            _deviceType = Register.Device_t.VehicleStation;
            _registerResult = string.Empty;
            _registerResultVisible = Visibility.Collapsed;
        }
        private void OnRegisterTimeout(object sender)
        {
            _isTimeout = true;
        }
        public string RegisterStatus
        {
            get
            {
                if (!_isRegiser || _licenseStatus == null) return "（认证失败）";
                else
                {
                    if (_licenseStatus.IsEver == 0)
                    {
                        return string.Format("（认证成功,{0}前有效）", _licenseStatus.ExpirationTime.ToString("yyyy年Mm月dd日"));
                    }
                    else
                    {
                        return "（认证成功）";
                    }
                }
            }
        }      
        public Register.Device_t DeviceType
        {
            get
            {
                if (_licenseStatus == null) return _deviceType;
                else return _licenseStatus.RegisterDeviceType; 
            }
            set
            {
                _deviceType = value;
                NotifyPropertyChanged("DeviceTypeDept");
            }        
        }
        public string DeviceTypeDept
        {
            get
            {
                switch(DeviceType)
                {
                    case Register.Device_t.VehicleStation:
                        return "车载台";
                    case Register.Device_t.Repeater:
                        return "中继台";
                    case Register.Device_t.Portable:
                        return "手持台";
                    case Register.Device_t.PC:
                        return "计算机";
                }

                return "无效设备";
            }
        }
        public string DeviceSN 
        {
            get
            {
                if (_licenseStatus == null) return "未知";
                switch(DeviceType)
                {
                    case Register.Device_t.VehicleStation:
                        return _licenseStatus.RadioSerial;
                    case Register.Device_t.Repeater:
                        return _licenseStatus.RepeaterSerial;
                    
                    case Register.Device_t.Portable:
                    case Register.Device_t.PC:
                    default:return "未知";
                }

            }
        }


        public string RegisterResult { get{return _registerResult;} private set{_registerResult = value; NotifyPropertyChanged("RegisterResult");} }
        public Visibility RegisterResultVisible { get{return _registerResultVisible;} private set{_registerResultVisible = value; NotifyPropertyChanged("RegisterResultVisible");} }
       

        private void OnReceivedLicenseMessage(object sender,bool status, Register.License_t license)
        {
            _licenseStatus = license;
            if (status && license != null)
            {
                _isRegiser = true;  
            }

            NotifyPropertyChanged(new string[] { "RegisterStatus", "DeviceTypeDept", "DeviceSN" });
        }

        private void OnReceivedRegisterReply(object sender,bool status, Register.License_t license)
        {
            if (license != null)
            {
                _licenseStatus.RegisterTime = license.RegisterTime;
                _licenseStatus.IsEver = license.IsEver;
                _licenseStatus.ExpirationTime = license.ExpirationTime;
            }

            if (status)
            {
                _isRegiser = true;
                RegisterResult = "认证成功";
                NotifyPropertyChanged(new string[] { "RegisterStatus", "DeviceTypeDept", "DeviceSN" });
            }
            else
            {
                _isRegiser = false;
                RegisterResult = "认证失败";
                NotifyPropertyChanged(new string[] { "RegisterStatus"});
            }
        }



        public ICommand Query{get{return new Command(()=>
        {
            RegisterResultVisible = Visibility.Collapsed;
            if(!_isTimeout && _register != null)_register.QueryLicense();  
        });}}

        public ICommand SetLicense{get{return new Command((object parameter)=>
        {
            RegisterResultVisible = Visibility.Visible;
            RegisterResult = "正在认证...";
           
            if (_isTimeout || _register == null || !_register.SetLicense(parameter as string))
            {
                RegisterResult = "认证失败";
            }
        });}}

  
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
