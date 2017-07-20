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
using System.Windows.Controls.Primitives;

namespace Manager
{
    /// <summary>
    /// 按照步骤 1a 或 1b 操作，然后执行步骤 2 以在 XAML 文件中使用此自定义控件。
    ///
    /// 步骤 1a) 在当前项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:LocationInDoor.View"
    ///
    ///
    /// 步骤 1b) 在其他项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根 
    /// 元素中: 
    ///
    ///     xmlns:MyNamespace="clr-namespace:LocationInDoor.View;assembly=LocationInDoor.View"
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
    ///     <MyNamespace:iBeaconPoint/>
    ///
    /// </summary>
    public class iBeaconPoint : Control
    {
        static iBeaconPoint()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(iBeaconPoint), new FrameworkPropertyMetadata(typeof(iBeaconPoint)));
        }

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;
            MenuItem muItem_Delete = (MenuItem)baseTemplate.FindName("muItem_Delete", this);

            if (muItem_Delete != null)
            {
                muItem_Delete.Click += delegate(object sender, RoutedEventArgs e)
                {
                    RoutedEventArgs newEventArgs = new RoutedEventArgs(RemoveRoutedEvent);
                    RaiseEvent(newEventArgs);
                };
            }

        }


        public static readonly DependencyProperty ImageProperty = DependencyProperty.Register("iBeacon", typeof(CiBeacon), typeof(iBeaconPoint), new UIPropertyMetadata(null));
        public CiBeacon iBeacon { set { SetValue(ImageProperty, value); } get { return GetValue(ImageProperty) as CiBeacon; } }


        public static readonly RoutedEvent RemoveRoutedEvent = EventManager.RegisterRoutedEvent("Remove", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(iBeaconPoint));

        public event RoutedEventHandler Remove
        {
            add { AddHandler(RemoveRoutedEvent, value); }
            remove { RemoveHandler(RemoveRoutedEvent, value); }
        }

    }
}
