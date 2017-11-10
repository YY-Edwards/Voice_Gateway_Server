using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Manager.Views
{
    /// <summary>
    /// 按照步骤 1a 或 1b 操作，然后执行步骤 2 以在 XAML 文件中使用此自定义控件。
    ///
    /// 步骤 1a) 在当前项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根
    /// 元素中:
    ///
    ///     xmlns:MyNamespace="clr-namespace:Manager.Views"
    ///
    ///
    /// 步骤 1b) 在其他项目中存在的 XAML 文件中使用该自定义控件。
    /// 将此 XmlNamespace 特性添加到要使用该特性的标记文件的根
    /// 元素中:
    ///
    ///     xmlns:MyNamespace="clr-namespace:Manager.Views;assembly=Manager.Views"
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
    ///     <MyNamespace:SettingControl/>
    ///
    /// </summary>
    public class ManageView : UserControl
    {
        static ManageView()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ManageView), new FrameworkPropertyMetadata(typeof(ManageView)));
        }

        public string Title { set { SetValue(TitleProperty, value); } get { return GetValue(TitleProperty) as string; } }

        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(string), typeof(ManageView), new UIPropertyMetadata(null));

        private ContentPresenter container = null;
        private ScrollViewer scrollViewer = null;
        private int currentPart = 0;
        private int willSetPart = 0;

        public ManageView()
        {
            this.Loaded += new RoutedEventHandler(OnManageViewLoaded);
        }

        private void OnManageViewLoaded(object sender, RoutedEventArgs e)
        {
            if(currentPart != willSetPart)
            {
                ScorllToPart(willSetPart);
            }
        }
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            ControlTemplate baseTemplate = this.Template;

            container = (ContentPresenter)baseTemplate.FindName("container", this);
            scrollViewer = (ScrollViewer)baseTemplate.FindName("scrollViewer", this);
        }

        public void ScorllToPart(int part)
        {
            try
            {
                willSetPart = part;
                if (willSetPart == currentPart) return;
                              
                if (container == null || scrollViewer == null) return;
                if (container.Content is DockPanel)
                {
                    UIElement targetUIElement = (this.container.Content as DockPanel).Children[part];
                    PosInScrView(this.scrollViewer, targetUIElement as FrameworkElement, -52);
                    currentPart = part;
                }
            }
            catch
            {
            }
        }

        private void PosInScrView(ScrollViewer scr, FrameworkElement element, double offset)
        {
            GeneralTransform transform = element.TransformToVisual(scr);
            double pos = transform.Transform(new Point(element.Margin.Left, element.Margin.Top)).Y;
            scr.ScrollToVerticalOffset(scr.ContentVerticalOffset + pos + offset);
        }

        protected RadioButton FindCheckedRadioButton(UIElementCollection elements, string groupName)
        {
            foreach (var element in elements)
            {
                if (element is RadioButton && (element as RadioButton).GroupName == groupName && (bool)(element as RadioButton).IsChecked) return element as RadioButton;
                else if (element is Panel && (element as Panel).Children != null) return FindCheckedRadioButton((element as Panel).Children, groupName);
                else continue;
            }

            return null;
        }
    }
}