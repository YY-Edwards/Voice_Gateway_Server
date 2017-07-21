using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Documents;
using System.Windows.Interop;
using System.Threading;

namespace Manager
{
    /// <summary>
    /// Map.xaml 的交互逻辑
    /// </summary>
    public partial class Map : UserControl
    {     
        public Map()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty AreaProperty = DependencyProperty.Register("Area", typeof(CArea), typeof(Map), new PropertyMetadata(new CArea(), delegate(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            Map map = d as Map;
            if(map != null)
            {
                RoutedEventArgs args = new RoutedEventArgs(AreaChangedRoutedEvent, map);
                map.RaiseEvent(args);
            }   
        }));
        public CArea Area
        {
            set
            {
                SetValue(AreaProperty, value);
            }
            get {

                return GetValue(AreaProperty) as CArea;          
            }
        }


        public static readonly RoutedEvent AreaChangedRoutedEvent =
            EventManager.RegisterRoutedEvent(" AreaChanged", RoutingStrategy.Bubble, typeof(EventHandler<RoutedEventArgs>), typeof(Map));

        public event RoutedEventHandler AreaChanged
        {
            add { this.AddHandler(AreaChangedRoutedEvent, value); }
            remove { this.RemoveHandler(AreaChangedRoutedEvent, value); }
        }

        public static readonly DependencyProperty iBeaconsProperty = DependencyProperty.Register("iBeacons", typeof(IEnumerable), typeof(Map), new PropertyMetadata("", delegate(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            Map map = d as Map;
            if (map != null && map.iBeacons != null)
            {
                map.cvs_Locations.Children.Clear();

                map.ClearRadio();

                foreach (CiBeacon ibeacon in map.iBeacons)
                {
                    //for test
                    //map.AddRadio(ibeacon, new CRadio() { RadioID = ibeacon.ID });
                    //map.AddRadio(ibeacon, new CRadio() { RadioID = ibeacon.ID + 1 });
                    //map.AddRadio(ibeacon, new CRadio() { RadioID = ibeacon.ID + 2 });
                    //map.AddRadio(ibeacon, new CRadio() { RadioID = ibeacon.ID + 3 });
                    //end test                    
                    if (double.IsInfinity(ibeacon.X) || double.IsInfinity(ibeacon.Y)) continue;

                    double x = map.cvs_Locations.ActualWidth * ibeacon.X - 50;
                    double y = map.cvs_Locations.ActualHeight * ibeacon.Y - 50;


                    iBeaconPoint po = new iBeaconPoint()
                    {
                        Height = 100,
                        Width = 100,
                        Margin = new Thickness(x, y, 0, 0),
                        iBeacon = ibeacon,
                    };

                    //po.Remove += delegate(object obj, RoutedEventArgs args) { map.OniBeaconRemove(obj, args); };
                    po.PreviewMouseMove += delegate(object obj, MouseEventArgs args) { map.OnPreviewMouseMove(obj, args); };
                    po.QueryContinueDrag += delegate(object obj, QueryContinueDragEventArgs args) { map.OnQueryContinueDrag(obj, args); };                 
                    map.cvs_Locations.Children.Add(po);
                }             
            }
        }));
      
        public IEnumerable iBeacons
        {
            get
            {
                return (IEnumerable)GetValue(iBeaconsProperty);
            }
            set
            {
                SetValue(iBeaconsProperty, value);
            }
        }


        public static readonly RoutedEvent iBeaconsChangedRoutedEvent =
           EventManager.RegisterRoutedEvent("iBeaconsChanged", RoutingStrategy.Bubble, typeof(EventHandler<RoutedEventArgs>), typeof(Map));

        public event RoutedEventHandler iBeaconsChanged
        {
            add { this.AddHandler(iBeaconsChangedRoutedEvent, value); }
            remove { this.RemoveHandler(iBeaconsChangedRoutedEvent, value); }
        }

        public static readonly DependencyProperty IsReadOnlyProperty = DependencyProperty.Register("IsReadOnly", typeof(bool), typeof(Map), new PropertyMetadata(false));

        public bool IsReadOnly
        {
            get
            {
                return (bool)GetValue(IsReadOnlyProperty);
            }
            set
            {
                SetValue(IsReadOnlyProperty, value);
            }
        }


        public Point GetMapPostion(Point pannel)
        {
            CVMMap cvmmap = this.Resources["CVMMap"] as CVMMap;
            return cvmmap.GetMapPostion(pannel);
        }

       
        private AdornerLayer mAdornerLayer = null;
        private DateTime mStartHoverTime = DateTime.MinValue;
        private void OnPreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (IsReadOnly) return;
            iBeaconPoint ibeacon = (iBeaconPoint)sender;

            if (Mouse.LeftButton != MouseButtonState.Pressed)
                return;

            Point pos = e.GetPosition(ibeacon);
            HitTestResult result = VisualTreeHelper.HitTest(ibeacon, pos);
            if (result == null)
                return;

            ibeacon.Visibility = Visibility.Hidden;

            DragDropAdorner adorner = new DragDropAdorner(ibeacon, ibeacon.iBeacon);
            mAdornerLayer = AdornerLayer.GetAdornerLayer(grd_MapPanel);
            mAdornerLayer.Add(adorner);

            DataObject dataObject = new DataObject(ibeacon.iBeacon.Copy());

            System.Windows.DragDrop.DoDragDrop(ibeacon, dataObject, DragDropEffects.Copy);

            mStartHoverTime = DateTime.MinValue;

            mAdornerLayer.Remove(adorner);
            mAdornerLayer = null;

            RoutedEventArgs args = new RoutedEventArgs(iBeaconsChangedRoutedEvent, this);
            this.RaiseEvent(args);
        }
        private void OnQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
            if (IsReadOnly) return;
            mAdornerLayer.Update();                 
        }


        private Dictionary<CiBeacon, List<CRadio>> m_Radios = new Dictionary<CiBeacon, List<CRadio>>();

        public void AddRadio(CiBeacon ibeacon, CRadio radio)
        {
            if(m_Radios.ContainsKey(ibeacon))
            {
                if (m_Radios[ibeacon] == null) m_Radios[ibeacon] = new List<CRadio>();
                int index = m_Radios[ibeacon].FindIndex(p => p.RadioID == radio.RadioID);
                if(index >=0 && index < m_Radios[ibeacon].Count)
                {
                    m_Radios[ibeacon][index] = radio;
                }
                else
                {
                    m_Radios[ibeacon].Add(radio);
                }
            }
            else
            {
                m_Radios.Add(ibeacon, new List<CRadio>());
                m_Radios[ibeacon].Add(radio);
            }

            DrawRadios();
        }

        public void RemoveRadio(CRadio radio)
        {
            if (m_Radios == null) return;
            foreach (var item in m_Radios)
            {
                if (item.Value == null) continue;
                item.Value.RemoveAll(p => p.RadioID == radio.RadioID);
            }

            DrawRadios();
        }

        public void ClearRadio()
        {
            m_Radios.Clear();
        }

        private void DrawRadios()
        {
            this.cvs_Radios.Children.Clear();
            if (m_Radios == null) return;

            foreach(var item in m_Radios)
            {
                if (item.Value == null) continue;
                for(int i  = item.Value.Count - 1; i >=0; i--)
                {
                    double x = this.cvs_Radios.ActualWidth * item.Key.X - 32;
                    double y = this.cvs_Radios.ActualHeight * item.Key.Y - 64;

                    //adjust postion
                    if(i > 0)
                    {
                        int layer = i % 3 == 0 ? (i / 3) : (i/3 + 1);
                        switch(i%3)
                        {
                            case 1://lefttop y -10;x - 5
                                x -= layer * 5;
                                y -= layer * 5;
                                break;
                            case 2: //top y-20
                                y -= layer * 10;
                                break;
                            case 0: //right top y - 10 x + 5
                                x += layer * 5;
                                y -= layer * 5;
                                break;

                        }
                    }


                    Point radiopos = new Point(x, y);


                    RadioPoint pos = new RadioPoint()
                    {
                        Height = 64,
                        Width = 64,
                        Margin = new Thickness(radiopos.X, radiopos.Y, 0, 0),
                        Radio = item.Value[i]   
                    };

                    this.cvs_Radios.Children.Add(pos);
                }
            }
        }


        private void grd_Locations_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (this.iBeacons != null)
            {
                foreach (iBeaconPoint point in cvs_Locations.Children)
                {
                    double x = cvs_Locations.ActualWidth * ((CiBeacon)point.iBeacon).X - 50;
                    double y = cvs_Locations.ActualHeight * ((CiBeacon)point.iBeacon).Y - 50;
                    point.Margin = new Thickness(x, y, 0, 0);
                }
            }

            DrawRadios();            
        }
    }

    public class CMapControl
    {
        public static double s_MaxZoom = 3;
        public static double s_MinZoom = 0.5;

        public Rect MapRect { get { return m_MapRect; } set { m_MapRect = value; } }
        public double Rate { get { return m_Rate; } set { m_Rate = value; } }
        public Point ScalePoint { get { return m_ScalePoint; } set { m_ScalePoint = value; } }

        private Size m_PannelSize;
        private Size m_MapOriginSize;
        private double m_Scale = 1.0;

        private Rect m_MapRect;
        private double m_Rate = 1.0;
        private Point m_ScalePoint = new Point(0,0);
        public void Initialize(Size pannel)
        {
            if (pannel.IsEmpty) return;
            m_PannelSize = pannel;
           
            if (m_PannelSize.Height / m_PannelSize.Width > m_Scale)
            {
                m_MapOriginSize.Width = m_PannelSize.Width;
                m_MapOriginSize.Height = m_PannelSize.Width * m_Scale;
            }
            else
            {
                m_MapOriginSize.Width = m_PannelSize.Height / m_Scale;
                m_MapOriginSize.Height = m_PannelSize.Height;
            }

            m_MapRect = new Rect(new Point(0, 0), m_MapOriginSize);

            Zoom();
        }
        public void SetImage(Size image, Size pannel)
        {
            if (m_PannelSize.IsEmpty) return;
            if (image.IsEmpty) return;

            m_Scale = image.Height / image.Width;
            if (m_PannelSize.Height / m_PannelSize.Width > m_Scale)
            {
                m_MapOriginSize.Width = m_PannelSize.Width;
                m_MapOriginSize.Height = m_PannelSize.Width * m_Scale;
            }
            else
            {
                m_MapOriginSize.Width = m_PannelSize.Height / m_Scale;
                m_MapOriginSize.Height = m_PannelSize.Height;
            }

            m_MapRect = new Rect(new Point(0, 0), m_MapOriginSize);

            m_Rate = 1.0;
            m_ScalePoint = new Point(0, 0);

            Zoom();
        }

       
        public void Zoom(Point scale, double rate)
        {
            m_ScalePoint = scale;
            if (m_Rate == rate) return;
            else
            {
                m_Rate = rate;
            }
            
            Zoom();
        }

        public void Move(Vector mov)
        {
            m_MapRect.X = m_MapRect.X + mov.X;
            m_MapRect.Y = m_MapRect.Y + mov.Y;

            AdjustPosition();
        }
        private void Zoom()
        {
            if (m_MapOriginSize.IsEmpty) return;

            if (m_Rate < s_MinZoom) m_Rate = s_MinZoom;
            if (m_Rate > s_MaxZoom) m_Rate = s_MaxZoom;

            if (MapRect.IsEmpty) m_MapRect = new Rect();
            m_MapRect.X = -(m_ScalePoint.X * m_Rate - m_ScalePoint.X);
            m_MapRect.Y = -(m_ScalePoint.Y * m_Rate - m_ScalePoint.Y);
            m_MapRect.Height = m_MapOriginSize.Height * m_Rate;
            m_MapRect.Width = m_MapOriginSize.Width * m_Rate;

            AdjustPosition();
        }

        private void AdjustPosition()
        {
            if (m_PannelSize.IsEmpty) return;
            if (MapRect.IsEmpty) return;

            if (m_MapRect.X + m_MapRect.Width < m_PannelSize.Width) m_MapRect.X = m_PannelSize.Width - m_MapRect.Width;
            if (m_MapRect.Y + m_MapRect.Height < m_PannelSize.Height) m_MapRect.Y = m_PannelSize.Height - m_MapRect.Height;

            if (m_MapRect.X > 0) m_MapRect.X = 0;
            if (m_MapRect.Y > 0) m_MapRect.Y = 0;
        }

        public Point GetMapPostion(Point pannel)
        {
            if (MapRect.IsEmpty) return default(Point);
            return  new Point((pannel.X - m_MapRect.Left) / m_MapRect.Width, (pannel.Y - m_MapRect.Top) / m_MapRect.Height);
        }
    }

    public class CVMMap : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private CMapControl m_MapCtrl;
        public CVMMap()
        {
            if (m_MapCtrl == null) m_MapCtrl = new CMapControl();
        }

        public Rect MapRect { get {
            return m_MapCtrl.MapRect; } }

        public ICommand Load { get { return new CDelegateCommand(LoadMap); } }
        public ICommand AreaUpdate { get { return new CDelegateCommand(AreaChanged); } }
        public ICommand SizeUpdate { get { return new CDelegateCommand(SizeChanged); } }
        

        public void Scale(object sender, MouseWheelEventArgs e)
        {
            Map map = Util.Lib.Find.ParentObject<Map>(sender as DependencyObject);
            if (map == null) return;
            double delta = (double)e.Delta / 1000;
            m_MapCtrl.Zoom(e.GetPosition(map), m_MapCtrl.Rate + delta);
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("MapRect"));   
        }

        private Point m_StartPoint;
        private bool m_ImageIsDrag = false;
        public void StartMove(object sender, MouseButtonEventArgs e)
        {
            Map map = Util.Lib.Find.ParentObject<Map>(sender as DependencyObject);
            if (map == null) return;
            m_StartPoint = e.GetPosition(map);
            m_MapCtrl.Zoom(m_StartPoint, m_MapCtrl.Rate);
            m_ImageIsDrag = true;
        }
        public void EndMove_Leave(object sender, MouseEventArgs e)
        {
            m_ImageIsDrag = false;
        }
        public void EndMove_UP(object sender, MouseButtonEventArgs e)
        {
            m_ImageIsDrag = false;
        }

        public void Move(object sender, MouseEventArgs e)
        {
            Map map = Util.Lib.Find.ParentObject<Map>(sender as DependencyObject);
            if (map != null && m_ImageIsDrag)
            {
                Point endmove =  e.GetPosition(map);
                Vector move = endmove - m_StartPoint;
                m_MapCtrl.Move(move);
                m_StartPoint = endmove;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("MapRect"));   
            }
        }


        private void LoadMap(object parameter)
        {
            if (parameter == null || !(parameter is Map)) return;
            Map map = parameter as Map;
            m_MapCtrl.Initialize(new Size(map.ActualWidth, map.ActualHeight));
        }

        private BitmapImage m_MapImage = null;
        public BitmapImage MapImage { get { return m_MapImage; } }

        private Visibility m_MapErrVisibe = Visibility.Hidden;
        public Visibility MapErrVisibe { get { return m_MapErrVisibe; } }
        private void AreaChanged(object parameter)
        {
            if (parameter == null || !(parameter is Map)) return;

            Map map = parameter as Map;
           
            try
            {

                try
                {
                    if (System.IO.File.Exists(map.Area.LocalPath))
                    {
                        m_MapImage = new BitmapImage(new Uri(map.Area.LocalPath));
                    }
                    else
                    {
                        m_MapImage = new BitmapImage(new Uri(map.Area.Map));
                    }
                    m_MapErrVisibe = Visibility.Hidden;
                }
                catch
                {
                    m_MapImage = null;
                    m_MapErrVisibe = Visibility.Visible;                  
                }

               

                m_MapCtrl.SetImage(new Size(map.Area.Width, map.Area.Height), new Size(map.ActualWidth, map.ActualHeight));

              
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("MapErrVisibe"));
                    PropertyChanged(this, new PropertyChangedEventArgs("MapRect"));
                    PropertyChanged(this, new PropertyChangedEventArgs("MapImage"));
                }
            }
            catch
            {
            }
        }

        private void SizeChanged(object parameter)
        {
            if (parameter == null || !(parameter is Map)) return;
            Map map = parameter as Map;
            m_MapCtrl.Initialize(new Size(map.ActualWidth, map.ActualHeight));
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("MapRect"));   
        }

        public Point GetMapPostion(Point pannel)
        {
            return m_MapCtrl.GetMapPostion(pannel);
        }
    }
}