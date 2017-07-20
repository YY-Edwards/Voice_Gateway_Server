using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using Util.Lib;
using System.Windows.Media.Imaging;
using System.Windows.Automation.Peers;

namespace Manager
{
    public class DragDropAdorner : Adorner
    {
        private CiBeacon m_iBeacon = null;

        public DragDropAdorner(UIElement parent, CiBeacon ibeacon)
            : base(parent)
        {
            IsHitTestVisible = false; // Seems Adorner is hit test visible?
            m_iBeacon = ibeacon;
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);

            if (m_iBeacon != null)
            {
                Win32.POINT screenPos = new Win32.POINT();
                if (Win32.GetCursorPos(ref screenPos))
                {
                    Point pos = PointFromScreen(new Point(screenPos.X, screenPos.Y));

                    Rect rect = new Rect(pos.X - 7.5, pos.Y - 7.5, 120, 20);

                    DockPanel dock = new DockPanel()
                    {
                        Background = new SolidColorBrush(Color.FromArgb(0, 0xFF, 0xFF, 0xFF)),
                        Height = 20,
                        Width = 120,
                        LastChildFill = false
                    };


                    Image img = new Image()
                    {
                        Source = new BitmapImage(new Uri("pack://application:,,,/views/images/bluetooth.png")),
                        Height = 18,
                        Width = 18,
                        VerticalAlignment = System.Windows.VerticalAlignment.Center,
                        Margin = new Thickness(0, 0, 5, 0)
                    };

                    dock.Children.Add(img);

                    TextBlock lab = new TextBlock
                    {
                        Text = m_iBeacon.NameStr,

                        VerticalAlignment = System.Windows.VerticalAlignment.Center,
                    };
                    dock.Children.Add(lab);

                    SolidColorBrush renderBrush = new SolidColorBrush(Colors.Green);

                    drawingContext.DrawRectangle(new VisualBrush(dock), new Pen(Brushes.Transparent, 0), rect);

                    base.OnRender(drawingContext);
                }
            }
        }
    }

    public static class Win32
    {
        public struct POINT { public Int32 X; public Int32 Y; }

        [DllImport("user32.dll")]
        public static extern bool GetCursorPos(ref POINT point);
    }
}
