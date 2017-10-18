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

using Sigmar.Controls;
using Dispatcher.ViewsModules;
using Dispatcher.Service;

using System.Windows.Resources;

namespace Dispatcher.Views
{
    /// <summary>
    /// positionindoor.xaml 的交互逻辑
    /// </summary>
    public partial class PositionInDoor : UserControl
    {
        public static double s_MaxZoomScale = 3;
        public static double s_MinZoomScale = 1;
        private VMArea _areaviewmodule;
        public PositionInDoor()
        {
            InitializeComponent();
        }

        public Rect MapRect
        {
            get { return (Rect)GetValue(MapRectProperty); }
            set { SetValue(MapRectProperty, value); }
        }
        public static readonly DependencyProperty MapRectProperty =
            DependencyProperty.Register("MapRect", typeof(Rect), typeof(PositionInDoor));

        private void loaded(object sender, RoutedEventArgs e)
        {
            StreamResourceInfo sri = Application.GetResourceStream(new Uri("pack://application:,,,/resource/cursors/hand-open.cur"));
            this.Cursor = new Cursor(sri.Stream);
            InitializeMap();
        }
        private void datacontextchanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            InitializeMap();
        }


        private void InitializeMap()
        {
            _areaviewmodule = this.DataContext as VMArea;
            if (_areaviewmodule == null) return;
            _areaviewmodule.AdjustScale = AdjustScale();
            MapRect = new Rect(_areaviewmodule.PositionPoint.X, _areaviewmodule.PositionPoint.Y, _areaviewmodule.ImageSize.Width * _areaviewmodule.TotalScale, _areaviewmodule.ImageSize.Height * _areaviewmodule.TotalScale);
        }
        private double AdjustScale()
        {

            if( this.ActualWidth == 0 || this.ActualHeight == 0 || _areaviewmodule.ImageSize.Width == 0 ||_areaviewmodule.ImageSize.Height == 0 ) return 1.0;

            double mapscale = this.ActualWidth / this.ActualHeight;
            if (_areaviewmodule.Scale >= mapscale)
            {
                return this.ActualWidth / _areaviewmodule.ImageSize.Width;
            }
            else
            {
                return this.ActualHeight / _areaviewmodule.ImageSize.Height;
            }
           
        }


        private void sizechanged(object sender, SizeChangedEventArgs e)
        {
            InitializeMap();
        }

     
        public void Scale(object sender, MouseWheelEventArgs e)
        {
            Point _zoompoint = e.GetPosition(this);
            Console.WriteLine("Orgin Zoom Scale:" + _areaviewmodule.TotalScale.ToString());

            double _delta = _areaviewmodule.ZoomScale + (double)e.Delta / 1000;

            if (_delta <= s_MinZoomScale) _delta = s_MinZoomScale;
            if (_delta >= s_MaxZoomScale) _delta = s_MaxZoomScale;

            double _totalsacle = _delta * _areaviewmodule.AdjustScale;
            Console.WriteLine("New Zoom Scale:" + _totalsacle.ToString());

            double _sacle = _totalsacle /  _areaviewmodule.TotalScale;
            Console.WriteLine("Scale:" + _sacle.ToString());


            _areaviewmodule.ZoomScale = _delta;

            Console.WriteLine(_areaviewmodule.PositionPoint.ToString());

            if (_delta == 1.0)
            {
                _areaviewmodule.PositionPoint = new Point(0, 0);
            }
            else
            {
                Point _pos = _areaviewmodule.PositionPoint;
                _pos.X -= (_zoompoint.X * _sacle - _zoompoint.X);
                _pos.Y -= (_zoompoint.Y * _sacle - _zoompoint.Y);
                _areaviewmodule.PositionPoint = _pos;
            }


            Console.WriteLine(_areaviewmodule.PositionPoint.ToString());
            MapRect = new Rect(_areaviewmodule.PositionPoint.X, _areaviewmodule.PositionPoint.Y, _areaviewmodule.ImageSize.Width * _areaviewmodule.TotalScale, _areaviewmodule.ImageSize.Height * _areaviewmodule.TotalScale);
            AdjustPosition();
        }


        private Point startpoint;
        private bool imgaeisdrag = false;
        private void StartMove(object sender, MouseButtonEventArgs e)
        {
            startpoint = e.GetPosition(this);
            StreamResourceInfo sri = Application.GetResourceStream(new Uri("pack://application:,,,/resource/cursors/hand-close.cur"));
            this.Cursor =  new Cursor(sri.Stream);
            imgaeisdrag = true;
        }

        private void Move(object sender, MouseEventArgs e)
        {

            if (imgaeisdrag)
            {
                Point endmove = e.GetPosition(this);
                Vector move = endmove - startpoint;

                Rect _rect = MapRect;

                _rect.X += move.X;
                _rect.Y +=  move.Y;
                MapRect = _rect;
                _areaviewmodule.PositionPoint = new Point(MapRect.X, MapRect.Y);

                startpoint = endmove;
                AdjustPosition();
            }  
        }

        private void EndMove_Up(object sender, MouseButtonEventArgs e)
        {
            EndMove();
        }
        private void EndMove_Leave(object sender, MouseEventArgs e)
        {
            EndMove();          
        }

        private void EndMove()
        {
            StreamResourceInfo sri = Application.GetResourceStream(new Uri("pack://application:,,,/resource/cursors/hand-open.cur"));
            this.Cursor = new Cursor(sri.Stream);
            imgaeisdrag = false;
        }
        private void AdjustPosition()
        {
            Rect _rect = MapRect;

            if (_rect.X + _rect.Width < this.ActualWidth) _rect.X = this.ActualWidth - _rect.Width;
            if (_rect.Y + _rect.Height < this.ActualHeight) _rect.Y = this.ActualHeight - _rect.Height;

            if (_rect.X > 0) _rect.X = 0;
            if (_rect.Y > 0) _rect.Y = 0;

            MapRect = _rect;
            _areaviewmodule.PositionPoint = new Point(MapRect.X, MapRect.Y);
        }

        private void grd_Locations_SizeChanged(object sender, SizeChangedEventArgs e)
        {

        }
    }
}
