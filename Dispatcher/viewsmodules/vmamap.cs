using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;

using Sigmar;
using System.Windows.Resources;
using System.IO;

using Dispatcher.Service;
using Sigmar.Logger;

namespace Dispatcher.ViewsModules
{
    public class VMAmap : INotifyPropertyChanged
    {

        public delegate void InitializeLocationHandler(string entrance);
        public event InitializeLocationHandler InitializeLocation;


        public delegate void DrawPointHandler(DrawLocationReportArgs args);
        public event DrawPointHandler OnDrawPoint;

        
        private string _amaplocal = App.RuntimeDir + "location/";

        private bool _isinitializelocation = false;
        public bool IsInitializeLocation { get { return _isinitializelocation; } }
        public string Entrance { get { return _amaplocal + "index.html"; } }
        public VMAmap()
        {
            ExportResource();
            _isinitializelocation = true;
            if (InitializeLocation != null) InitializeLocation(Entrance);
        }

        public ICommand DrawPoint { get { return new Command(DrawPointExec); } }
        public void DrawPointExec(object parameter)
        {
            if (parameter == null || !(parameter is DrawLocationReportArgs)) return;
            if (OnDrawPoint != null) OnDrawPoint(parameter as DrawLocationReportArgs);
        }

        private void ExportResource()
        {
            ExportFile("index.html", global::Dispatcher.resource.Amap.index);
            ExportFile("trace.js", global::Dispatcher.resource.Amap.trace);   
            
            if(!Directory.Exists(_amaplocal + "infowin.png"))global::Dispatcher.resource.Amap.infowin.Save(_amaplocal + "infowin.png");
            if (!Directory.Exists(_amaplocal + "msg_close_18_18_n.png")) global::Dispatcher.resource.Amap.msg_close_18_18_n.Save(_amaplocal + "msg_close_18_18_n.png");

            if (!Directory.Exists(_amaplocal + "pos_radio_36_77.png")) global::Dispatcher.resource.Amap.pos_radio_36_77.Save(_amaplocal + "pos_radio_36_77.png");
            if (!Directory.Exists(_amaplocal + "pos_ride_36_77.png")) global::Dispatcher.resource.Amap.pos_ride_36_77.Save(_amaplocal + "pos_ride_36_77.png");
            if (!Directory.Exists(_amaplocal + "pos_staff_36_77.png")) global::Dispatcher.resource.Amap.pos_staff_36_77.Save(_amaplocal + "pos_staff_36_77.png");
            if (!Directory.Exists(_amaplocal + "pos_vehicle_36_77.png")) global::Dispatcher.resource.Amap.pos_vehicle_36_77.Save(_amaplocal + "pos_vehicle_36_77.png");

            if (!Directory.Exists(_amaplocal + "radio_40_40.png")) global::Dispatcher.resource.Amap.radio_40_40.Save(_amaplocal + "radio_40_40.png");
            if (!Directory.Exists(_amaplocal + "ride_40_40.png")) global::Dispatcher.resource.Amap.ride_40_40.Save(_amaplocal + "ride_40_40.png");
            if (!Directory.Exists(_amaplocal + "staff_40_40.png")) global::Dispatcher.resource.Amap.staff_40_40.Save(_amaplocal + "staff_40_40.png");
            if (!Directory.Exists(_amaplocal + "vehicle_40_40.png")) global::Dispatcher.resource.Amap.vehicle_40_40.Save(_amaplocal + "vehicle_40_40.png");
        }


        private void ExportFile(string name, byte[] filebytes)
        {
            string strPath = _amaplocal + name;

            if (File.Exists(strPath)) return;
            if (Directory.Exists(_amaplocal) == false)
            {
                Directory.CreateDirectory(_amaplocal);
                File.SetAttributes(_amaplocal, FileAttributes.Hidden | FileAttributes.ReadOnly);
            }

            using (FileStream fs = new FileStream(strPath, FileMode.Create))
            {
                fs.Write(filebytes, 0, filebytes.Length);
            }
        }

        private void ExportFile(string name, string filecontents)
        {
            ExportFile(name, Encoding.UTF8.GetBytes(filecontents));
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
