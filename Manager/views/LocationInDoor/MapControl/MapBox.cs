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

using System.Windows.Resources;
using System.ComponentModel;

namespace Manager.Views
{
    /// <summary>
    /// 按照步骤 1a 或 1b 操作，然后执行步骤 2 以在 XAML 文件中使用此自定义控件。
    ///
    /// 步骤 1a) 在当前项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:Manager.Views.LocationInDoor"
    ///
    ///
    /// 步骤 1b) 在其他项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:Manager.Views.LocationInDoor;assembly=Manager.Views.LocationInDoor"
    ///
    /// 您还需要添加一个从 XAML 文件所在的项目到此项目的项目引用，
    /// 并重新生成以避免编译错误: 
    ///
    ///     在解决方案资源管理器中右击目标项目，然后依次单击
    ///     “添加引用”->“项目”->[浏览查找并选择此项目]
    ///
    ///
    /// 步骤 2)
    /// 继续操作并在 XAML 文件中使用控件。
    ///
    ///     <MyNamespace:MapBox/>
    ///
    /// </summary>
    public class MapBox : UserControl
    {      
        private Grid _layerContainer;

        static MapBox()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(MapBox), new FrameworkPropertyMetadata(typeof(MapBox)));
        }

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;

            _layerContainer = (Grid)baseTemplate.FindName("_layerContainer", this);

            if (_layerContainer != null)
            {
                _layerContainer.PreviewMouseWheel += new MouseWheelEventHandler(OnLayerContainerPreviewMouseWheel);
                _layerContainer.PreviewMouseDown += new MouseButtonEventHandler(OnLayerContainerPreviewMouseDown);
                _layerContainer.PreviewMouseUp += new MouseButtonEventHandler(OnLayerContainerPreviewMouseUp);
                _layerContainer.MouseLeave += new MouseEventHandler(OnLayerContainerMouseLeave);
                _layerContainer.PreviewMouseMove += new MouseEventHandler(OnLayerContainerPreviewMouseMove);
            }

            this.ShowLayers();
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);
            DrawMap();
        }


        public List<MapLayer> Layers
        {
            get { return (List<MapLayer>)GetValue(LayersProperty); }
            set
            {
                SetValue(LayersProperty, value);
            }
        }

        public static readonly DependencyProperty LayersProperty = DependencyProperty.Register("Items", typeof(List<MapLayer>), typeof(MapBox), new PropertyMetadata(new List<MapLayer>(), new PropertyChangedCallback(OnItemsChangedCallback)));

        private static void OnItemsChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MapBox _map = d as MapBox;
            if (_map != null) _map.ShowLayers();           
        }

        public void ShowLayers()
        {
            if (_layerContainer != null)
            {
                if (Layers != null)
                {                  
                    _layerContainer.Children.Clear();
                    foreach (MapLayer _layer in Layers)
                    {
                        if(this.Name == _layer.MapName && !_layerContainer.Children.Contains(_layer))
                        {
                            _layerContainer.Children.Add(_layer);
                        }
                    }
                }
            }
        }



        private double _baseZoom = 1.0;

        public double MinZoom
        {
            get { return (double)GetValue(MinZoomProperty); }
            set { SetValue(MinZoomProperty, value); }
        }
        public static readonly DependencyProperty MinZoomProperty = DependencyProperty.Register("MinZoom", typeof(double), typeof(MapBox), new PropertyMetadata(1.0));

        public double MaxZoom
        {
            get { return (double)GetValue(MaxZoomProperty); }
            set { SetValue(MaxZoomProperty, value); }
        }
        public static readonly DependencyProperty MaxZoomProperty = DependencyProperty.Register("MaxZoom", typeof(double), typeof(MapBox), new PropertyMetadata(3.0));


        public double Zoom{
            get { return (double)GetValue(ZoomProperty); }
            set { SetValue(ZoomProperty, value); }
        }
        public static readonly DependencyProperty ZoomProperty = DependencyProperty.Register("Zoom", typeof(double), typeof(MapBox), new PropertyMetadata(1.0, new PropertyChangedCallback(OnZoomChangedCallback)));

        private static void OnZoomChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MapBox _map = d as MapBox;
            if (_map != null) _map.DrawMap();
        }

        public Point  ViewPosition
        {
            get { return (Point)GetValue(ViewPositionProperty); }
            set { SetValue(ViewPositionProperty, value); }
        }
        public static readonly DependencyProperty ViewPositionProperty = DependencyProperty.Register("ViewPosition", typeof(Point), typeof(MapBox),new PropertyMetadata(new Point(0,0), new PropertyChangedCallback(OnPositionChangedCallback)));

        private static void OnPositionChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MapBox _map = d as MapBox;
            if (_map != null) _map.DrawMap();
        }
        public Size ViewSize
        {
            get { return (Size)GetValue(ViewSizeProperty); }
            set { SetValue(ViewSizeProperty, value); }
        }
        public static readonly DependencyProperty ViewSizeProperty = DependencyProperty.Register("ViewSize", typeof(Size), typeof(MapBox));


        public void DrawMap()
        {
            if (this.ActualWidth <= 0 || this.ActualHeight <= 0 || ViewSize.Width <= 0 || ViewSize.Height <= 0)
            {
                MapRect = new Rect(new Point(0, 0), ViewSize);
            }
            else
            {

            }
            double mapscale = this.ActualWidth / this.ActualHeight;
            double imgscale = ViewSize.Width / ViewSize.Height;

            _baseZoom = mapscale > imgscale ? this.ActualHeight / ViewSize.Height : this.ActualWidth / ViewSize.Width;
            //ViewPosition = new Point(0, 0);
            MapRect = new Rect(ViewPosition.X, ViewPosition.Y, ViewSize.Width * _baseZoom * Zoom, ViewSize.Height * _baseZoom * Zoom);
        }


        private void OnLayerContainerPreviewMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (Zoom >= MaxZoom && e.Delta >= 0) return;
            if (Zoom <= MinZoom && e.Delta <= 0) return;


            Point _zoomPoint = e.GetPosition(this);

            double delta = (double)e.Delta / 1000 * Zoom;
            Zoom += delta;

            if (Zoom > MaxZoom)
            {
                delta = Zoom - MaxZoom;
                Zoom = MaxZoom;
            }
            else if (Zoom < MinZoom)
            {
                delta = MinZoom - Zoom;
                Zoom = MinZoom;
            }

            ViewPosition = new Point(MapRect.X - _zoomPoint.X * delta, MapRect.Y - _zoomPoint.Y * delta);

            MapRect = new Rect(ViewPosition.X, ViewPosition.Y, ViewSize.Width * _baseZoom * Zoom, ViewSize.Height * _baseZoom * Zoom);

            BoundaryAdjust();
        }

        //Move

        private Point _startPoint;
        private bool _isDrag = false;
        private void OnLayerContainerPreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            _startPoint = e.GetPosition(this);
            StreamResourceInfo sri = Application.GetResourceStream(new Uri("pack://application:,,,/Views/LocationInDoor/MapControl/Cursors/hand-close.cur"));
            this.Cursor = new Cursor(sri.Stream);
            _isDrag = true;
        }

        private void OnLayerContainerPreviewMouseUp(object sender, MouseButtonEventArgs e)
        {
            EndMove();
        }

        private void OnLayerContainerMouseLeave(object sender, MouseEventArgs e)
        {
            EndMove();
        }
        private void OnLayerContainerPreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (_isDrag)
            {
                Point endmove = e.GetPosition(this);
                Vector move = endmove - _startPoint;

                Rect _rect = MapRect;

                _rect.X += move.X;
                _rect.Y += move.Y;

                ViewPosition = new Point(_rect.X, _rect.Y);

                MapRect = _rect;

                _startPoint = endmove;
                BoundaryAdjust();
            }
        }


        private void EndMove()
        {
            StreamResourceInfo sri = Application.GetResourceStream(new Uri("pack://application:,,,/Views/LocationInDoor/MapControl/Cursors/hand-open.cur"));
            this.Cursor = new Cursor(sri.Stream);
            _isDrag = false;
        }





        private void BoundaryAdjust()
        {
           
            Rect _rect = MapRect;

            if (_rect.X + _rect.Width < this.ActualWidth) _rect.X = this.ActualWidth - _rect.Width;
            if (_rect.Y + _rect.Height < this.ActualHeight) _rect.Y = this.ActualHeight - _rect.Height;

            if (_rect.X > 0) _rect.X = 0;
            if (_rect.Y > 0) _rect.Y = 0;

            ViewPosition = new Point(_rect.X, _rect.Y);

            MapRect = _rect;
        }

        public Rect MapRect
        {
            get { return (Rect)GetValue(MapRectProperty); }
            set { SetValue(MapRectProperty, value); }
        }
        public static readonly DependencyProperty MapRectProperty =DependencyProperty.Register("MapRect", typeof(Rect), typeof(MapBox));


        public Point GetCoordinate(Point pointInMap)
        {

            if (MapRect.Width == 0 || MapRect.Height == 0) return new Point(0,0);
            return new Point((pointInMap.X - MapRect.Left) / MapRect.Width, (pointInMap.Y - MapRect.Top) / MapRect.Height);

        }      

    }
}
