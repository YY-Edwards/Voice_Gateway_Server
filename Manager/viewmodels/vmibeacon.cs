﻿using System;
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


using System.Runtime.InteropServices;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using Util.Lib;
using System.Windows.Media.Imaging;

using System.Windows.Automation.Peers;

namespace Manager
{
    public class CVMiBeacon:CVMManager, INotifyPropertyChanged
    {

        public CVMiBeacon()
        {
            if (m_iBeacon == null)
            {
                m_iBeacon = new CiBeaconMgr();
                m_iBeacon.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditiBeacon == null) m_EditiBeacon = new CiBeacon();           
        }


        private CiBeaconMgr m_iBeacon;
        public ObservableCollection<CRElement> iBeacons { get { return new ObservableCollection<CRElement>(m_iBeacon.List); } }


        private CiBeacon m_EditiBeacon;
        public CiBeacon EditiBeacon
        {
            get { return m_EditiBeacon; }
            set
            {
                if (value == null)
                {
                    m_EditiBeacon = new CiBeacon();
                    m_iBeacon.IsNew = true;
                }
                else
                {
                    m_EditiBeacon = value;
                    m_iBeacon.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                     PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                     PropertyChanged(this, new PropertyChangedEventArgs("UUID"));
                     PropertyChanged(this, new PropertyChangedEventArgs("Major"));
                     PropertyChanged(this, new PropertyChangedEventArgs("Minor"));
                     PropertyChanged(this, new PropertyChangedEventArgs("TxPower"));
                     PropertyChanged(this, new PropertyChangedEventArgs("RSSI"));
                     PropertyChanged(this, new PropertyChangedEventArgs("TimeStamp"));               
                }
            }
        }

        public string Name { set { m_EditiBeacon.Name = value; } get { if (m_EditiBeacon == null)return ""; return m_EditiBeacon.Name; } }
        public string UUID { set { m_EditiBeacon.UUID = value; } get { if (m_EditiBeacon == null)return ""; return m_EditiBeacon.UUID; } }

        public int Major { set { m_EditiBeacon.Major = value; } get { if (m_EditiBeacon == null)return 0; return m_EditiBeacon.Major; } }
        public int Minor { set { m_EditiBeacon.Minor = value; } get { if (m_EditiBeacon == null)return 0; return m_EditiBeacon.Minor; } }
        public int TxPower { set { m_EditiBeacon.TxPower = value; } get { if (m_EditiBeacon == null)return 0; return m_EditiBeacon.TxPower; } }

        public int RSSI { set { m_EditiBeacon.RSSI = value; } get { if (m_EditiBeacon == null)return 0; return m_EditiBeacon.RSSI; } }
        public int TimeStamp { set { m_EditiBeacon.TimeStamp = value; } get { if (m_EditiBeacon == null)return 0; return m_EditiBeacon.TimeStamp; } }     
       
        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.ibeacon  && PropertyChanged != null)
            {
                
                PropertyChanged(this, new PropertyChangedEventArgs("iBeacons"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                //PropertyChanged(this, new PropertyChangedEventArgs("SelectedAreaIndex"));             
            }
        }


        //command
        public ICommand New { get { return new CDelegateCommand(NewEle); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteEle); } }
        public ICommand Save { get { return new CDelegateCommand(SaveEle); } }
        public ICommand AreasLoad { get { return new CDelegateCommand(OnAreasLoad); } }

        public ICommand iBeaconsChanged { get { return new CDelegateCommand(OnMapiBeaconsChanged); } }
        
        private void NewEle()
        {
            m_iBeacon.IsNew = true;
            m_EditiBeacon = new CiBeacon();
            PropertyChanged(this, new PropertyChangedEventArgs("EditiBeacon"));
            PropertyChanged(this, new PropertyChangedEventArgs("Name"));
            PropertyChanged(this, new PropertyChangedEventArgs("UUID"));
            PropertyChanged(this, new PropertyChangedEventArgs("Major"));
            PropertyChanged(this, new PropertyChangedEventArgs("Minor"));
            PropertyChanged(this, new PropertyChangedEventArgs("TxPower"));
            PropertyChanged(this, new PropertyChangedEventArgs("RSSI"));
            PropertyChanged(this, new PropertyChangedEventArgs("TimeStamp"));  
        }

        private void DeleteEle()
        {
            if (m_EditiBeacon == null) return;
            m_iBeacon.Delete(m_EditiBeacon.ID);
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("iBeacons"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
            }          
        }

        //parameter:password,can not binding on passwordbox
        private void SaveEle(object parameter)
        {
            if (m_EditiBeacon == null) return;
            try
            {
                if (parameter == null ||!(parameter is ListView)) return;

                ListView lst = parameter as ListView;

                if (m_iBeacon.IsNew)
                {
                    m_iBeacon.Add(m_EditiBeacon);
                }
                else
                {
                    m_iBeacon.Modify(m_EditiBeacon.ID, m_EditiBeacon);
                }
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("iBeacons"));
                    PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
                    PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                }
                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_iBeacon.IsNew = false;
        }

        private void OnAreasLoad(object parameter)
        {
            if (parameter == null || !(parameter is ComboBox)) return;
            var dpd = DependencyPropertyDescriptor.FromProperty(ComboBox.ItemsSourceProperty, typeof(ComboBox));
            dpd.AddValueChanged(parameter, (o, e) => {
                AreasChanged(((ComboBox)parameter).ItemsSource);
            });
        }


        private void OnMapiBeaconsChanged()
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
            }
        }
        private void AreasChanged(object itemsource)
        {
            m_Areas = new List<CRElement>(((ObservableCollection<CRElement>)itemsource).ToList());
            m_SelectedAreaIndex = 0;
           
            if (PropertyChanged != null) // Point 2
            {
                //PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                //PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
                PropertyChanged(this, new PropertyChangedEventArgs("SelectedAreaIndex"));
            }           
        }

        private List<CRElement> m_Areas = new List<CRElement>();
      

        private int m_SelectedAreaIndex = 0;
        public int SelectedAreaIndex
        {
            get { return m_SelectedAreaIndex; }
            set
            {
                m_SelectedAreaIndex = value;

                if (m_SelectedAreaIndex >= 0 && PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                }
            }
        }

        public ObservableCollection<CRElement> iBeaconsInArea { get {

            if (m_SelectedAreaIndex < 0 || m_SelectedAreaIndex >= m_Areas.Count) return null;
            List<CRElement> list = m_iBeacon.List.FindAll(p =>
            {
                return ((CiBeacon)p).Area == ((CArea)m_Areas[m_SelectedAreaIndex]).Name;
            });

            return new ObservableCollection<CRElement>(list);
        }
            set { 
                int i = 0; }
        }


        public ObservableCollection<CRElement> iBeaconsOutAreas
        {
            get
            {
                List<CRElement> res = new List<CRElement>(m_iBeacon.List);
                foreach (CArea area in m_Areas)
                {
                    res.RemoveAll(p =>
                    {
                        return ((CiBeacon)p).Area == area.Name;
                    });
                }

                return new ObservableCollection<CRElement>(res);
            }
            set
            {
                int i = 0;
            }
        }


        private AdornerLayer mAdornerLayer = null;
        private DateTime mStartHoverTime = DateTime.MinValue;        
        public void OnMouseMove(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed)
                return;

            ListView lst = sender as ListView;

            Point pos = e.GetPosition(lst);
            HitTestResult result = VisualTreeHelper.HitTest(lst, pos);
            if (result == null)
                return;

            ListBoxItem listBoxItem = Util.Lib.Find.ParentObject<ListBoxItem>(result.VisualHit);
            if (listBoxItem == null || listBoxItem.Content != lst.SelectedItem || !(lst.SelectedItem is CiBeacon))
                return;

            DragDropAdorner adorner = new DragDropAdorner(listBoxItem, lst.SelectedItem as CiBeacon);


            Main win = Util.Lib.Find.ParentObject<Main>(result.VisualHit);

            mAdornerLayer = AdornerLayer.GetAdornerLayer(win.grd_Main);
            mAdornerLayer.Add(adorner);

            CiBeacon ibeacon = listBoxItem.Content as CiBeacon;
            DataObject dataObject = new DataObject(ibeacon.Copy());

            System.Windows.DragDrop.DoDragDrop(lst, dataObject, DragDropEffects.Copy);

            mStartHoverTime = DateTime.MinValue;

            mAdornerLayer.Remove(adorner);
            mAdornerLayer = null;

            if (PropertyChanged != null)
            {               
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsOutAreas"));
            }
        }

        public void OnDrag(object sender, QueryContinueDragEventArgs e)
        {
            mAdornerLayer.Update();
        }


        public void OniBeaconDrop(object sender, DragEventArgs e)
        {
            try
            {                
                CiBeacon ibeacon = e.Data.GetData(typeof(CiBeacon)) as CiBeacon;

                int index = m_iBeacon.List.FindIndex(p =>
                {
                    return ((CiBeacon)p).ID == ibeacon.ID;
                });

                if (index >= 0 && index < m_iBeacon.List.Count)
                {
                    ibeacon.Area = string.Empty;
                    ibeacon.X = 0;
                    ibeacon.Y = 0;
                    ibeacon.IsValid = false;

                    m_iBeacon.Modify(ibeacon.ID, ibeacon.Copy());

                    //m_iBeacon.List[index] = ibeacon.Copy();                   
                }
            }
            catch
            {
            }
        }

        public void OnMapDrop(object sender, DragEventArgs e)
        {
            try
            {
                if (m_SelectedAreaIndex < 0 || m_SelectedAreaIndex >= m_Areas.Count) return;


                Map map = (Map)sender; 
                Point pos = e.GetPosition(map);
                Point posinmap = map.GetMapPostion(pos);

                CiBeacon ibeacon = e.Data.GetData(typeof(CiBeacon)) as CiBeacon;

               
                int index = m_iBeacon.List.FindIndex(p =>
                {
                    return ((CiBeacon)p).ID == ibeacon.ID;
                });



                if (index >= 0 && index < m_iBeacon.List.Count)
                {
                    ibeacon.Area = ((CArea)m_Areas[m_SelectedAreaIndex]).Name;
                    ibeacon.X = posinmap.X;
                    ibeacon.Y =  posinmap.Y;
                    ibeacon.IsValid = true;

                    m_iBeacon.Modify(ibeacon.ID, ibeacon.Copy());
                    //m_iBeacon.List[index] = ibeacon.Copy();                   
                }
                else
                {

                }
            }
            catch
            {
            }
        }


        public void UpdateiBeacon()
        {
            if (PropertyChanged != null) // Point 2
            {               
                PropertyChanged(this, new PropertyChangedEventArgs("iBeaconsInArea"));
            }
        }
        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_iBeacon.Save();
        }

        public void Get()
        {
            m_iBeacon.Query();
        }

    }
}
