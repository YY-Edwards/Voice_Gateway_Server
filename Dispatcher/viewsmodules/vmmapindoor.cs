using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using Sigmar;
using Sigmar.Extension;

using Dispatcher.Service;
using Dispatcher.Modules;
using Dispatcher.Views;

using Sigmar.Logger;

namespace Dispatcher.ViewsModules
{
    public class VMMapInDoor : INotifyPropertyChanged
    {
        private ICollectionView _arealistview;
        public ICollectionView AreaList { get { return _arealistview; } private set { _arealistview = value; NotifyPropertyChanged("AreaList"); } }

        private int _areaindex = -1;
        public int AreaIndex { get { return _areaindex; } set { _areaindex = value; NotifyPropertyChanged("AreaIndex"); } }

        public VMMapInDoor()
        {
            ResourcesMgr.Instance().OnResourcesLoaded += new EventHandler(OnResourcesLoaded);
        }

        private void OnResourcesLoaded(object sender, EventArgs e)
        {
            if (ResourcesMgr.Instance().Areas.Count <= 0)
            {
                AreaList = new ListCollectionView(new List<VMArea>(){new VMArea(new CArea(){ID = -1, Name="没有有效区域"})});
            }
            else
            {
                 AreaList = new ListCollectionView(ResourcesMgr.Instance().Areas);
            }
        }


        //public ICommand DrawPoint { get { return new Command(DrawPointExec); } }
        //public void DrawPointExec(object parameter)
        //{
        //    if (parameter == null && !(parameter is LocationInDoorReports)) return;
        //    LocationInDoorReports reports = parameter as LocationInDoorReports;

        //    VMArea area = ResourcesMgr.Instance().Areas.Find(p => p.ID == reports.Beacon.Beacon.Area);
        //    if (area != null) area.DrawPoint.Execute(parameter);
        //}


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
    public class VMArea :INotifyPropertyChanged
    {
        private CArea _area;
        public long ID { get { return _area.ID; } }
        public string Name { get { return _area.Name; } }
        public ImageSource MapImage { get {
            try
            {
                if(System.IO.File.Exists(_area.Map))//location file
                {
                    return new BitmapImage(new Uri(_area.Map, UriKind.Absolute));
                }
                else
                {
                    return new BitmapImage(new Uri(CTServer.Instance().ImageUrl + _area.Map));
                }
            }
            catch(Exception ex)
            {
                Log.Warning(_area.Map + "Not Found",ex);
                return null;
            }
        }
        }
        public List<VMBeacon> Beacons { get { return ResourcesMgr.Instance().Beacons.FindAll(p => p.Beacon.Area == _area.ID); } }

        public List<VMBeacon.TargetPos_t> Targets
        {
            get
            {
                List<VMBeacon.TargetPos_t> trgs = new List<VMBeacon.TargetPos_t>();
                foreach (VMBeacon beacon in Beacons) trgs.AddRange(beacon.Targets);
               
                return trgs;           
        } }

        public List<object> AllPoint { get { List<object> points = new List<object>(); points.AddRange(Beacons); points.AddRange(Targets); return points; } }

        public VMArea(CArea area)
        {
            _area = area;
            SystemStatus.PropertyChanged += new PropertyChangedEventHandler(OnSystemStatusChagned);
        }

        private void OnSystemStatusChagned(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "LocationInDoorPoint")
            {
                NotifyPropertyChanged("AllPoint");
            }
        }

        private double _adjustscale = 1.0;
        private double _zoomscale = 1.0;
        private Point _positionpoint = new Point(0,0);
        public double Scale{get{return _area.Width <= 0 ||  _area.Height <= 0  ? 1.0 : (_area.Width / _area.Height);}}
        public double AdjustScale { get { return _adjustscale; } set { _adjustscale = value; } }
        public double ZoomScale { get { return _zoomscale; } set { _zoomscale = value; } }
        public double TotalScale { get { return _zoomscale * _adjustscale; } }
        public Point PositionPoint{get{return _positionpoint;} set{_positionpoint = value;}}
        public Size ImageSize{get{return new Size(_area.Width, _area.Height);}}

        //public ICommand DrawPoint { get { return new Command(DrawPointExec); } }
        //public void DrawPointExec(object parameter)
        //{
        //    if (parameter == null && !(parameter is LocationInDoorReports)) return;
        //    LocationInDoorReports reports = parameter as LocationInDoorReports;

        //    _beacon.AddPos.Execute(_target);

        //    _beacon = ResourcesMgr.Instance().Beacons.Find(p => p.TargetList.Contains(_target));
        //    if (_beacon != null)_beacon.RemovePos.Execute(_target);
        //    NotifyPropertyChanged("Beacons");
        //}

        //private 
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



