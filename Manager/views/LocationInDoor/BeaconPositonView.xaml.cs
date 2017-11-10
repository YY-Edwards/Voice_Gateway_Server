using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Manager.Models;
using Manager.ViewModels;
namespace Manager.Views
{
    /// <summary>
    /// UserManagerView.xaml 的交互逻辑
    /// </summary>
    public partial class BeaconPositonView : UserControl
    {
        private AdornerLayer _adornerLayer = null;
        //private DateTime _startHoverTime = DateTime.MinValue;    

        public BeaconPositonView()
        {
            InitializeComponent();
            this._beaconOutAreas.PreviewMouseMove += new MouseEventHandler(OnBeaconOutAreasPreviewMouseMove);
            this._beaconOutAreas.QueryContinueDrag += new QueryContinueDragEventHandler(OnBeaconOutAreasQueryContinueDrag);
            this._beaconOutAreas.Drop += new DragEventHandler(OnBeaconOutAreasDrop);

            this._locationInDoorMap.Drop += new DragEventHandler(OnLocationInDoorMapDrop);
        }

        private void OnBeaconOutAreasPreviewMouseMove(object sender, MouseEventArgs e)
        {
             if (Mouse.LeftButton != MouseButtonState.Pressed)return;

             ListView lst = sender as ListView;
             Point pos = e.GetPosition(lst);
             HitTestResult result = VisualTreeHelper.HitTest(lst, pos);
             if (result == null) return;

      
            ListBoxItem listBoxItem = Util.Lib.Find.ParentObject<ListBoxItem>(result.VisualHit);
            if (listBoxItem == null || listBoxItem.Content != lst.SelectedItem || !(lst.SelectedItem is Beacon))return;

            

            Beacon beacon = listBoxItem.Content as Beacon;
            BeaconPointAdorner adorner = new BeaconPointAdorner(listBoxItem, beacon);

            Main win = Util.Lib.Find.ParentObject<Main>(result.VisualHit);

            _adornerLayer = AdornerLayer.GetAdornerLayer(win.grd_Main);
            _adornerLayer.Add(adorner);

            Beacon beaconBackup = beacon.Copy() as Beacon;

            DataObject dataObject = new DataObject(beaconBackup);

            DragDrop.DoDragDrop(lst, dataObject, DragDropEffects.Copy);


            _adornerLayer.Remove(adorner);
            _adornerLayer = null;

             //if (PropertyChanged != null)
             //{
             //    PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
             //    PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
             //}
        }

        private void OnBeaconOutAreasQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
             _adornerLayer.Update();
        }

        private void OnBeaconOutAreasDrop(object sender, DragEventArgs e)
        {
            try
            {
                BeaconViewModel viewModel = this.DataContext as BeaconViewModel;
                if (viewModel == null) return;

                Beacon beacon = e.Data.GetData(typeof(Beacon)) as Beacon;

                if (!viewModel.BeaconListOutArea.Contains(p => p.ID == beacon.ID))
                {
                    beacon.Area = -1;
                    beacon.X = 0;
                    beacon.Y = 0;
                    beacon.IsValid = false;

                    viewModel.Modify(beacon);
                }
            }
            catch
            {
            }
        }

        private void OnLocationInDoorMapDrop(object sender, DragEventArgs e)
        {

            try
            {
                BeaconViewModel viewModel = this.DataContext as BeaconViewModel;
                if (viewModel == null || viewModel.SelectedArea == null) return;

                MapBox map = (MapBox)sender;

                Point coordinate = map.GetCoordinate(e.GetPosition(map));

                Beacon beacon = e.Data.GetData(typeof(Beacon)) as Beacon;

                beacon.Area = viewModel.SelectedArea.ID;
                beacon.X = coordinate.X;
                beacon.Y = coordinate.Y;
                beacon.IsValid = true;

                viewModel.Modify(beacon);
               
            }
            catch
            {

            }
        }


        //private AdornerLayer mAdornerLayer = null;
        //private DateTime mStartHoverTime = DateTime.MinValue;        
        //public void OnMouseMove(object sender, MouseEventArgs e)
        //{
        //    if (Mouse.LeftButton != MouseButtonState.Pressed)
        //        return;

        //    ListView lst = sender as ListView;

        //    Point pos = e.GetPosition(lst);
        //    HitTestResult result = VisualTreeHelper.HitTest(lst, pos);
        //    if (result == null)
        //        return;

        //    ListBoxItem listBoxItem = Util.Lib.Find.ParentObject<ListBoxItem>(result.VisualHit);
        //    if (listBoxItem == null || listBoxItem.Content != lst.SelectedItem || !(lst.SelectedItem is Beacon))
        //        return;

        //    DragDropAdorner adorner = new DragDropAdorner(listBoxItem, lst.SelectedItem as Beacon);


        //    Main win = Util.Lib.Find.ParentObject<Main>(result.VisualHit);

        //    mAdornerLayer = AdornerLayer.GetAdornerLayer(win.grd_Main);
        //    mAdornerLayer.Add(adorner);

        //    Beacon ibeacon = listBoxItem.Content as Beacon;
        //    DataObject dataObject = new DataObject(ibeacon.Copy());

        //    System.Windows.DragDrop.DoDragDrop(lst, dataObject, DragDropEffects.Copy);

        //    mStartHoverTime = DateTime.MinValue;

        //    mAdornerLayer.Remove(adorner);
        //    mAdornerLayer = null;

        //    if (PropertyChanged != null)
        //    {               
        //        PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
        //        PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
        //    }
        //}

        //public void OnDrag(object sender, QueryContinueDragEventArgs e)
        //{
        //    mAdornerLayer.Update();
        //}


        //public void OniBeaconDrop(object sender, DragEventArgs e)
        //{
        //    try
        //    {                
        //        Beacon ibeacon = e.Data.GetData(typeof(Beacon)) as Beacon;

        //        int index = m_iBeacon.List.FindIndex(p =>
        //        {
        //            return ((Beacon)p).ID == ibeacon.ID;
        //        });

        //        if (index >= 0 && index < m_iBeacon.List.Count)
        //        {
        //            ibeacon.Area = -1;
        //            ibeacon.X = 0;
        //            ibeacon.Y = 0;
        //            ibeacon.IsValid = false;

        //            m_iBeacon.Modify(ibeacon.ID, ibeacon.Copy());

        //            //m_iBeacon.List[index] = ibeacon.Copy();                   
        //        }
        //    }
        //    catch
        //    {
        //    }
        //}

        //public void OnMapDrop(object sender, DragEventArgs e)
        //{
        //    try
        //    {
        //        if (m_SelectedAreaIndex < 0 || m_SelectedAreaIndex >= m_Areas.Count) return;


        //        Map map = (Map)sender; 
        //        Point pos = e.GetPosition(map);
        //        Point posinmap = map.GetMapPostion(pos);

        //        Beacon ibeacon = e.Data.GetData(typeof(Beacon)) as Beacon;


        //        int index = m_iBeacon.List.FindIndex(p =>
        //        {
        //            return ((Beacon)p).ID == ibeacon.ID;
        //        });



        //        if (index >= 0 && index < m_iBeacon.List.Count)
        //        {
        //            ibeacon.Area = ((CArea)m_Areas[m_SelectedAreaIndex]).ID;
        //            ibeacon.X = posinmap.X;
        //            ibeacon.Y =  posinmap.Y;
        //            ibeacon.IsValid = true;

        //            m_iBeacon.Modify(ibeacon.ID, ibeacon.Copy());
        //            //m_iBeacon.List[index] = ibeacon.Copy();                   
        //        }
        //        else
        //        {

        //        }
        //    }
        //    catch
        //    {
        //    }
        //}
    }
}
