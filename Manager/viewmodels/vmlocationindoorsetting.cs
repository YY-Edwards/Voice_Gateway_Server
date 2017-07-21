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
    public class CVMLocationInDoorSetting : CVMManager, INotifyPropertyChanged
    {

        public CVMLocationInDoorSetting()
        {
            if (m_LocationInDoor == null)
            {
                m_LocationInDoor = new CLocationInDoorSetting();
                m_LocationInDoor.OnConfigurationChanged += OnConfiguratuinChanged;
            }
          
        }

        #region
      
        #endregion


        //basesetting
        private CLocationInDoorSetting m_LocationInDoor;
        public bool IsEnable { get { return m_LocationInDoor.IsEnable; } set { m_LocationInDoor.IsEnable = value; m_LocationInDoor.NeedSave(); } }

        public int CAI { get { return m_LocationInDoor.CAI; } set { m_LocationInDoor.CAI = value; m_LocationInDoor.NeedSave(); } }

        public string IP { get { return m_LocationInDoor.IP; } set { m_LocationInDoor.IP = value; m_LocationInDoor.NeedSave(); } }

        public int ID { get { return m_LocationInDoor.ID; } set { m_LocationInDoor.ID = value; m_LocationInDoor.NeedSave(); } }

        public int Port { get { return m_LocationInDoor.Port; } set { m_LocationInDoor.Port = value; m_LocationInDoor.NeedSave(); } }

        public int IntervalIndex { get { 
           switch(m_LocationInDoor.Interval)
           {
               case 30:
                   return 0;
               case 60:
                   return 1;
               case 120:
                   return 2;
               default:
                   return -1;
           }
        } set { 
            switch(value)
            {
                case 1:
                    m_LocationInDoor.Interval = 60;
                    break;
                case 2:
                    m_LocationInDoor.Interval = 120;
                    break;
                default:
                    m_LocationInDoor.Interval = 30;
                    break;
            }
            
            m_LocationInDoor.NeedSave(); } }
        public int iBeaconNumber { get { return m_LocationInDoor.iBeaconNumber; } set { m_LocationInDoor.iBeaconNumber = value; m_LocationInDoor.NeedSave(); } }


        public bool IsImmediate
        {
            get { return !m_LocationInDoor.IsTriggered; }
            set
            {
                if (value && m_LocationInDoor.IsTriggered != false)
                {
                    m_LocationInDoor.IsTriggered = false;
                    m_LocationInDoor.NeedSave();
                }
            }
        }
        public bool IsTriggered
        {
            get { return m_LocationInDoor.IsTriggered; }
            set
            {
                if (value && m_LocationInDoor.IsTriggered != true)
                {
                    m_LocationInDoor.IsTriggered = true;
                    m_LocationInDoor.NeedSave();
                }
            }
        }

       public int QueryTypeIndex { get { return (int)m_LocationInDoor.QueryType; } set { m_LocationInDoor.QueryType = (QueryType)value; m_LocationInDoor.NeedSave(); } }

        public bool IsEmergency { get { return m_LocationInDoor.IsEmergency; } set { m_LocationInDoor.IsEmergency = value; m_LocationInDoor.NeedSave(); } }
        private void OnConfiguratuinChanged(SettingType type, object config)
        {
            if (m_LocationInDoor == null || type != SettingType.LocationInDoor) return;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsEnable"));
                PropertyChanged(this, new PropertyChangedEventArgs("CAI"));
                PropertyChanged(this, new PropertyChangedEventArgs("IP"));
                PropertyChanged(this, new PropertyChangedEventArgs("ID"));
                PropertyChanged(this, new PropertyChangedEventArgs("Port"));
                PropertyChanged(this, new PropertyChangedEventArgs("Interval"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconNumber"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsImmediate"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsTriggered"));
                PropertyChanged(this, new PropertyChangedEventArgs("QueryType"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsEmergency "));             
            }
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;

        public void Set()
        {
            m_LocationInDoor.Set();
        }

        public void Get()
        {
            m_LocationInDoor.Get();
        }
    }
}
