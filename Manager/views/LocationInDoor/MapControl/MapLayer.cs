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
using System.ComponentModel;

using Manager.Models;

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
    public class MapLayer : UserControl
    {       
        static MapLayer()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(MapLayer), new FrameworkPropertyMetadata(typeof(MapLayer)));
        }

        public string MapName
        {
            get { return (string)GetValue(MapNameProperty); }
            set
            {
                SetValue(MapNameProperty, value);
            }
        }

        public static readonly DependencyProperty MapNameProperty =
           DependencyProperty.Register("MapName", typeof(string), typeof(MapLayer), new PropertyMetadata(""));



        public Grid _pointsContainer;
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;

            _pointsContainer = (Grid)baseTemplate.FindName("_pointsContainer", this);
        }



        protected override void OnRender(DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);
            DrawPoints();
        }

        public Type PointType
        {
            get { return (Type)GetValue(PointTypeProperty); }
            set
            {
                SetValue(PointTypeProperty, value);
            }
        }

        public static readonly DependencyProperty PointTypeProperty =
           DependencyProperty.Register("PointType", typeof(Type), typeof(MapLayer), new PropertyMetadata(typeof(string)));
       


        public object Content
        {
            get { return GetValue(ContentProperty); }
            set
            {
                SetValue(ContentProperty, value);
            }
        }

        public static readonly DependencyProperty ContentProperty =
           DependencyProperty.Register("Content", typeof(object), typeof(MapLayer), new PropertyMetadata(null));


        public ICollectionView Points
        {
            get { return (ICollectionView)GetValue(PointsProperty); }
            set
            {
                SetValue(PointsProperty, value);
            }
        }

        public static readonly DependencyProperty PointsProperty =
           DependencyProperty.Register("Points", typeof(ICollectionView), typeof(MapLayer), new PropertyMetadata(null, new PropertyChangedCallback(OnPointsChangedCallback)));

        private static void OnPointsChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MapLayer _layer = d as MapLayer;
            if (_layer != null) _layer.DrawPoints();
        }


        public void DrawPoints()
        {
            if (_pointsContainer != null)
            {
                _pointsContainer.Children.Clear();
                if (Points != null)
                {
                    foreach (ILocationable point in Points)
                    {

                        MapPoint PointUI = System.Activator.CreateInstance(PointType) as MapPoint;

                        PointUI.Content = point;
                        PointUI.SetPosition(point.GetLeft() * this.ActualWidth, point.GetTop() * this.ActualHeight);
                        //SetPosition
                        //PointUI.Margin = new Thickness(point.GetLeft() * this.ActualWidth, point.GetTop() * this.ActualHeight,0,0);
                        // PointUI


                        _pointsContainer.Children.Add(PointUI);
                        //_pointsContainer.Children.Add(AN);
                    }
                }
            }
        }     
    }
}
