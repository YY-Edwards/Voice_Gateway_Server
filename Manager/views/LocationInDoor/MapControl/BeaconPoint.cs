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
    public class BeaconPoint : MapPoint
    {        
        static BeaconPoint()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(BeaconPoint), new FrameworkPropertyMetadata(typeof(BeaconPoint)));
        }

        public BeaconPoint()
        {
            this.PreviewMouseMove += new MouseEventHandler(OnBeaconPointPreviewMouseMove);
            this.QueryContinueDrag += new QueryContinueDragEventHandler(OnBeaconPointQueryContinueDrag);
        }

       protected override Point GetCenter()
       {
           return new Point(50, 50);
       }

        private Beacon _beacon { get { return base.Content as Beacon; } }

        public Beacon Beacon
        {
            get { return base.Content as Beacon; }
            set
            {
                base.Content = value;
            }
        }

        private AdornerLayer _adornerLayer = null;
        private DateTime _startHoverTime = DateTime.MinValue;

        private void OnBeaconPointPreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed) return;

            BeaconPoint point = (BeaconPoint)sender;

            Point pos = e.GetPosition(point);
            HitTestResult result = VisualTreeHelper.HitTest(point, pos);
            if (result == null) return;

            point.Visibility = Visibility.Hidden;
            BeaconPointAdorner adorner = new BeaconPointAdorner(point, point._beacon);

            
            MapBox map = Util.Lib.Find.ParentObject<MapBox>(result.VisualHit);
            _adornerLayer = AdornerLayer.GetAdornerLayer(map);
            _adornerLayer.Add(adorner);

            DataObject dataObject = new DataObject(point._beacon.Copy());
          
            DragDrop.DoDragDrop(point, dataObject, DragDropEffects.Copy);

            _adornerLayer.Remove(adorner);
            _adornerLayer = null;
        }

        private void OnBeaconPointQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
            _adornerLayer.Update();
        }
    }
}
