using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dispatcher.ViewsModules;
using System.ComponentModel;
using Dispatcher;

namespace Dispatcher.Service
{
    public class SystemStatus
    {
        private static CallStatus_t _systemcallstatus = CallStatus_t.Idle;
        public static CallStatus_t SystemCallStatus 
        {
            get { return _systemcallstatus; }
            set
            {
                _systemcallstatus = value;
                UpdateTargetCallStatus();
            }
        }
        public static bool IsSystemInCall { get { return SystemCallStatus != CallStatus_t.Idle ? true : false; } }


        public static event PropertyChangedEventHandler PropertyChanged;
        public static event OperatedEventHandler OnOperated;

        public static void UpdateTargetCallStatus()
        {
            ResourcesMgr res = ResourcesMgr.Instance();

            foreach(VMTarget traget in res.All)
            {
                traget.NotifyPropertyChanged("EnableCallOrStop");
                traget.NotifyPropertyChanged("CanCall");
                traget.NotifyPropertyChanged("CanStopCall");
            }

            if (PropertyChanged != null) PropertyChanged(null, new PropertyChangedEventArgs("CallStatus")); 
        }

        private static List<VMTarget> _rx = new List<VMTarget>();
        public static List<VMTarget> RX { get { return _rx; } }

        public static void Add(VMTarget target)
        {
            _rx.Add(target);
            if (PropertyChanged != null) PropertyChanged(null , new PropertyChangedEventArgs("CallStatus")); 
        }

        public static void Remove(VMTarget target)
        {
            _rx.Remove(target);
            if (PropertyChanged != null) PropertyChanged(null, new PropertyChangedEventArgs("CallStatus")); 
        }
        

        public static void Clear()
        {
            _rx.Clear();
            if (PropertyChanged != null) PropertyChanged(null, new PropertyChangedEventArgs("CallStatus")); 
        }

       public static void UpdateLocationInDoorPoint()
       {
           if (PropertyChanged != null) PropertyChanged(null, new PropertyChangedEventArgs("LocationInDoorPoint")); 
       }


       public static void DrawAmapPoint(object parameter)
       {
           if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.DrawLocationPoint, parameter));
       }            
    }
}
