using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Runtime.InteropServices;
using Microsoft.Win32;

using System.Windows.Interop;
namespace TrboX
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    /// 
    public partial class MainWindow : Window
    {
        public enum ResizeDirection
        {
            Left = 1,
            Right = 2,
            Top = 3,
            TopLeft = 4,
            TopRight = 5,
            Bottom = 6,
            BottomLeft = 7,
            BottomRight = 8,
        }


        double relativeClip = 4;

        public MainWindow()
        {
            InitializeComponent();
            this.SourceInitialized += new EventHandler(WSInitialized);          
        }

        HwndSource hs;
        void WSInitialized(object sender, EventArgs e)
        {
            hs = PresentationSource.FromVisual(this) as HwndSource;
            hs.AddHook(new HwndSourceHook(WndProc));
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0024:/* WM_GETMINMAXINFO */
                    WmGetMinMaxInfo(hwnd, lParam);
                    handled = true;
                    break;
                default: break;
            }
            return (System.IntPtr)0;
        }


        private void ResetCursor(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed)
            {
                this.Cursor = Cursors.Arrow;
            }
        }

      





        private GridLength NavLength, InfoLength, LogLength;

        private void chk_DispNav_Checked(object sender, RoutedEventArgs e)
        {
            //if (null != col_Nav)
            //{
            //    if((null == NavLength) || (true == NavLength.IsAuto))NavLength = new GridLength(200);
            //    col_Nav.Width = NavLength;
            //    col_Nav.MinWidth = 40;
            //}

            //if(null != gsp_Nav)
            //{
            //    gsp_Nav.Width = 5;
            //}
        }

        private void chk_DispNav_Unchecked(object sender, RoutedEventArgs e)
        {
            //if (null != col_Nav)
            //{
            //    NavLength = col_Nav.Width;
            //    GridLength height = new GridLength(0);
            //    col_Nav.Width = height;
            //    col_Nav.MinWidth = 0;
            //}

            //if (null != gsp_Nav)
            //{
            //    gsp_Nav.Width = 0;
            //}
        }

        private void btn_DispInfo_Checked(object sender, RoutedEventArgs e)
        {
            //if (null != col_Info)
            //{
            //    if ((null == InfoLength) || (true == InfoLength.IsAuto)) InfoLength = new GridLength(180);
            //    col_Info.Width = InfoLength;
            //    col_Info.MinWidth = 40;
            //}
            //if (null != gsp_Info)
            //{
            //    gsp_Info.Width = 5;
            //}
        }

        private void btn_DispInfo_Unchecked(object sender, RoutedEventArgs e)
        {
            //if (null != col_Info)
            //{
            //    InfoLength = col_Info.Width;
            //    GridLength height = new GridLength(0);
            //    col_Info.Width = height;
            //    col_Info.MinWidth = 0;
            //}

            //if (null != gsp_Info)
            //{
            //    gsp_Info.Width = 0;
            //}
        }
        private void chk_DispLog_Checked(object sender, RoutedEventArgs e)
        {
            //if (null != row_Log)
            //{
            //    if ((null == LogLength) || (true == LogLength.IsAuto)) LogLength = new GridLength(140);                
            //    row_Log.Height = LogLength;
            //    row_Log.MinHeight = 40;
            //}
            //if (null != gsp_Log)
            //{
            //    gsp_Log.Height = 5;
            //}
        }
        private void chk_DispLog_Unchecked(object sender, RoutedEventArgs e)
        {
            //if (null != row_Log)
            //{
            //    LogLength = row_Log.Height;
            //    GridLength height = new GridLength(0);
            //    row_Log.Height = height;
            //    row_Log.MinHeight = 0;
            //}

            //if (null != gsp_Log)
            //{
            //    gsp_Log.Height = 0;
            //}
        }




        private void OnRxMessage(int type, int value, IntPtr dat, IntPtr rev)
        {
            byte[] dat_array = new byte[512];
            byte[] rev_array = new byte[512];           
        }

        private void DisplayResizeCursor(object sender, MouseEventArgs e)
        {
            Border clickBorder = sender as Border;

            Point pos = Mouse.GetPosition(this);
            double x = pos.X;
            double y = pos.Y;
            double w = this.Width;
            double h = this.Height;

            if (x <= relativeClip & y <= relativeClip) // left top
            {
                this.Cursor = Cursors.SizeNWSE;
            }
            if (x >= w - relativeClip & y <= relativeClip) //right top
            {
                this.Cursor = Cursors.SizeNESW;
            }

            if (x >= w - relativeClip & y >= h - relativeClip) //bottom right
            {
                this.Cursor = Cursors.SizeNWSE;
            }

            if (x <= relativeClip & y >= h - relativeClip)  // bottom left
            {
                this.Cursor = Cursors.SizeNESW;
            }

            if ((x >= relativeClip & x <= w - relativeClip) & y <= relativeClip) //top
            {
                this.Cursor = Cursors.SizeNS;
            }

            if (x >= w - relativeClip & (y >= relativeClip & y <= h - relativeClip)) //right
            {
                this.Cursor = Cursors.SizeWE;
            }

            if ((x >= relativeClip & x <= w - relativeClip) & y > h - relativeClip) //bottom
            {
                this.Cursor = Cursors.SizeNS;
            }

            if (x <= relativeClip & (y <= h - relativeClip & y >= relativeClip)) //left
            {
                this.Cursor = Cursors.SizeWE;
            }
        }

       
        private void Resize(object sender, MouseButtonEventArgs e)
        {
            Border clickedBorder = sender as Border;

            Point pos = Mouse.GetPosition(this);
            double x = pos.X;
            double y = pos.Y;
            double w = this.Width;
            double h = this.Height;

            if (x <= relativeClip & y <= relativeClip) // left top
            {
                this.Cursor = Cursors.SizeNWSE;
                ResizeWindow(ResizeDirection.TopLeft);
            }
            if (x >= w - relativeClip & y <= relativeClip) //right top
            {
                this.Cursor = Cursors.SizeNESW;
                ResizeWindow(ResizeDirection.TopRight);
            }

            if (x >= w - relativeClip & y >= h - relativeClip) //bottom right
            {
                this.Cursor = Cursors.SizeNWSE;
                ResizeWindow(ResizeDirection.BottomRight);
            }

            if (x <= relativeClip & y >= h - relativeClip)  // bottom left
            {
                this.Cursor = Cursors.SizeNESW;
                ResizeWindow(ResizeDirection.BottomLeft);
            }

            if ((x >= relativeClip & x <= w - relativeClip) & y <= relativeClip) //top
            {
                this.Cursor = Cursors.SizeNS;
                ResizeWindow(ResizeDirection.Top);
            }

            if (x >= w - relativeClip & (y >= relativeClip & y <= h - relativeClip)) //right
            {
                this.Cursor = Cursors.SizeWE;
                ResizeWindow(ResizeDirection.Right);
            }

            if ((x >= relativeClip & x <= w - relativeClip) & y > h - relativeClip) //bottom
            {
                this.Cursor = Cursors.SizeNS;
                ResizeWindow(ResizeDirection.Bottom);
            }

            if (x <= relativeClip & (y <= h - relativeClip & y >= relativeClip)) //left
            {
                this.Cursor = Cursors.SizeWE;
                ResizeWindow(ResizeDirection.Left);
            }
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        private void ResizeWindow(ResizeDirection direction)
        {
            SendMessage(hs.Handle, 0x112 /* WM_SYSCOMMAND*/, (IntPtr)(61440 + direction), IntPtr.Zero);
        }


        #region 这一部分用于最大化时不遮蔽任务栏
        private static void WmGetMinMaxInfo(System.IntPtr hwnd, System.IntPtr lParam)
        {

            MINMAXINFO mmi = (MINMAXINFO)Marshal.PtrToStructure(lParam, typeof(MINMAXINFO));

            // Adjust the maximized size and position to fit the work area of the correct monitor
            int MONITOR_DEFAULTTONEAREST = 0x00000002;
            System.IntPtr monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

            if (monitor != System.IntPtr.Zero)
            {

                MONITORINFO monitorInfo = new MONITORINFO();
                GetMonitorInfo(monitor, monitorInfo);
                RECT rcWorkArea = monitorInfo.rcWork;
                RECT rcMonitorArea = monitorInfo.rcMonitor;
                mmi.ptMaxPosition.x = Math.Abs(rcWorkArea.left - rcMonitorArea.left);
                mmi.ptMaxPosition.y = Math.Abs(rcWorkArea.top - rcMonitorArea.top);
                mmi.ptMaxSize.x = Math.Abs(rcWorkArea.right - rcWorkArea.left);
                mmi.ptMaxSize.y = Math.Abs(rcWorkArea.bottom - rcWorkArea.top);
            }

            Marshal.StructureToPtr(mmi, lParam, true);
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct POINT
        {
            public int x;
            public int y;
            public POINT(int x, int y)
            {
                this.x = x;
                this.y = y;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MINMAXINFO
        {
            public POINT ptReserved;
            public POINT ptMaxSize;
            public POINT ptMaxPosition;
            public POINT ptMinTrackSize;
            public POINT ptMaxTrackSize;
        };

        [StructLayout(LayoutKind.Sequential, Pack = 0)]
        public struct RECT
        {
            /// <summary> Win32 </summary>
            public int left;
            /// <summary> Win32 </summary>
            public int top;
            /// <summary> Win32 </summary>
            public int right;
            /// <summary> Win32 </summary>
            public int bottom;

            /// <summary> Win32 </summary>
            public static readonly RECT Empty = new RECT();

            /// <summary> Win32 </summary>
            public int Width
            {
                get { return Math.Abs(right - left); }  // Abs needed for BIDI OS
            }
            /// <summary> Win32 </summary>
            public int Height
            {
                get { return bottom - top; }
            }

            /// <summary> Win32 </summary>
            public RECT(int left, int top, int right, int bottom)
            {
                this.left = left;
                this.top = top;
                this.right = right;
                this.bottom = bottom;
            }


            /// <summary> Win32 </summary>
            public RECT(RECT rcSrc)
            {
                this.left = rcSrc.left;
                this.top = rcSrc.top;
                this.right = rcSrc.right;
                this.bottom = rcSrc.bottom;
            }

            /// <summary> Win32 </summary>
            public bool IsEmpty
            {
                get
                {
                    // BUGBUG : On Bidi OS (hebrew arabic) left > right
                    return left >= right || top >= bottom;
                }
            }

            /// <summary> Return a user friendly representation of this struct </summary>
            public override string ToString()
            {
                if (this == RECT.Empty) { return "RECT {Empty}"; }
                return "RECT { left : " + left + " / top : " + top + " / right : " + right + " / bottom : " + bottom + " }";
            }

            /// <summary> Determine if 2 RECT are equal (deep compare) </summary>
            public override bool Equals(object obj)
            {
                if (!(obj is Rect)) { return false; }
                return (this == (RECT)obj);
            }

            /// <summary>Return the HashCode for this struct (not garanteed to be unique)</summary>
            public override int GetHashCode()
            {
                return left.GetHashCode() + top.GetHashCode() + right.GetHashCode() + bottom.GetHashCode();
            }


            /// <summary> Determine if 2 RECT are equal (deep compare)</summary>
            public static bool operator ==(RECT rect1, RECT rect2)
            {
                return (rect1.left == rect2.left && rect1.top == rect2.top && rect1.right == rect2.right && rect1.bottom == rect2.bottom);
            }

            /// <summary> Determine if 2 RECT are different(deep compare)</summary>
            public static bool operator !=(RECT rect1, RECT rect2)
            {
                return !(rect1 == rect2);
            }
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public class MONITORINFO
        {
            /// <summary>
            /// </summary>            
            public int cbSize = Marshal.SizeOf(typeof(MONITORINFO));

            /// <summary>
            /// </summary>            
            public RECT rcMonitor = new RECT();

            /// <summary>
            /// </summary>            
            public RECT rcWork = new RECT();

            /// <summary>
            /// </summary>            
            public int dwFlags = 0;
        }

        [DllImport("user32")]
        internal static extern bool GetMonitorInfo(IntPtr hMonitor, MONITORINFO lpmi);

        [DllImport("User32")]
        internal static extern IntPtr MonitorFromWindow(IntPtr handle, int flags);
        #endregion



        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }
        private void lab_Title_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            bdr_Header_PreviewMouseLeftButtonDown(sender, e);
        }

        private void bdr_Header_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            //var element = (FrameworkElement)sender;
            if (e.ClickCount == 1)
            {
                DragMove();
            }
            if (e.ClickCount > 1)
            {
                bdr_Header_DoubleClick(sender, e);
            }
        }

        private void bdr_Header_DoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (WindowState == WindowState.Maximized)
            {
                WindowState = WindowState.Normal;
            }
            else
            {
                WindowState = WindowState.Maximized;
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }
        private void btn_SysMin_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void btn_SysMax_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Maximized;
        }
        private void btn_SysReStore_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Normal;
        }

        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (WindowState == WindowState.Maximized)
            {
                btn_SysReStore.Visibility = Visibility.Visible;
                btn_SysMax.Visibility = Visibility.Collapsed;
            }
            else
            {
                btn_SysReStore.Visibility = Visibility.Collapsed;
                btn_SysMax.Visibility = Visibility.Visible;
            }
        }

        private void btn_SysClose_Click(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }
    }
}
