using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using Sigmar.Logger;

using Dispatcher.Service;
using Dispatcher.Modules;

namespace Dispatcher.ViewsModules
{
    public class VMBeacon :INotifyPropertyChanged
    {
        private CBeacon _ibeacon;

        public VMBeacon(CBeacon beacon)
        {
            _ibeacon = beacon;
        }

        public CBeacon Beacon { get { return _ibeacon; } }

        public string Name { get { return _ibeacon.Name  + "("+ _ibeacon.Major.ToString() + "，"+ _ibeacon.Minor.ToString() + ")"; } }
        public double X { get { return _ibeacon.X; } }
        public double Y { get { return _ibeacon.Y; } }
        public string Type { get { return "iBeacon"; } }

        private List<VMTarget> _targets = new List<VMTarget>();
        public List<VMTarget> TargetList { get { return _targets; } }

        public class TargetPos_t
        {
            public double X { get; set; }
            public double Y { get; set; }
            public int Z { get; set; }
            public VMTarget target { get; set; }
            public string Type { get { return "LocationInDoorPoint"; } }
        }
        public List<TargetPos_t> Targets { get
        {
            List<TargetPos_t> trgs = new List<TargetPos_t>();
            for (int i = _targets.Count - 1; i >= 0; i--) trgs.Add(new TargetPos_t() {X = X,Y = Y, Z = i, target = _targets[i] });
           return trgs;
        } 
        }

        public ICommand AddPos { get { return new Command(AddPosExec); } }
        private void AddPosExec(object parameter)
        {
           if(parameter == null || !(parameter is VMTarget ))return;
            VMTarget _target = parameter as VMTarget;

            int index = _targets.FindIndex(p => p == _target);
            if(index >=0 )
            {
                _targets[index] = _target;
            }
            else
            {
                _targets.Add(_target);
            }
            SystemStatus.UpdateLocationInDoorPoint();
        }

        public ICommand RemovePos { get { return new Command(RemovePosExec); } }
        private void RemovePosExec(object parameter)
        {
            if (parameter == null || !(parameter is VMTarget)) return;
            VMTarget _target = parameter as VMTarget;

            _targets.RemoveAll(p => p == _target);
            SystemStatus.UpdateLocationInDoorPoint();
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
