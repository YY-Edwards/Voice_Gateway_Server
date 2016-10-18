﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Media;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Controls;
using System.Windows.Input;

using Microsoft.Win32;

namespace TrboX
{
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

    public partial  class MyWindow : Window
    {
        private const int WM_SYSCOMMAND = 0x112;
        public const int WM_LBUTTONUP = 0x0202;
        private HwndSource hs;
        IntPtr retInt = IntPtr.Zero;
        public double relativeClip = 14;

        public string SubTitle { set { SetSubTitle(value); } get { return SubTitle; } }

        public MINMAXINFO m_mmi;
        public MINMAXINFO MinMaxInfo { get{return m_mmi;} }

       //public string SubTitle { set;}
        public MyWindow():base()
        {
           // InitializeComponent();
            this.Loaded += delegate
            {
                InitializeEvent();
            };

            this.SourceInitialized += new EventHandler(MyWindow_SourceInitialized);
        }
 
        private void MyWindow_SourceInitialized(object sender, EventArgs e)
        {
            hs = PresentationSource.FromVisual((Visual)sender) as HwndSource;
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
                case 0x0018 :/*WM_SHOWWINDOW*/
                    if (1 == (int)wParam) //Show
                    {
                        OnMyWindow_Show();
                    }
                    else
                    {
                        OnMyWindow_Hide();
                    }
                    break;
                case 0x0201://WM_LBUTTON_DWON
                case 0x0204://WM_RBUTTON_DWON
                    OnMouseL_R_Prssed();

                    break;
                case 0x0202://WM_LBUTTON_UP
                case 0x0205://WM_RBUTTON_UP
                    OnMouseL_Released();
                                           
                    break;

                case 0x03 ://WM_MOVE = 0x03

                    break;
                default: break;
            }
            return (System.IntPtr)0;

        }
        #region

        public virtual void OnMouseL_R_Prssed()
        { 
        
        }
        public virtual void OnMouseL_Released()
        { }
        public virtual void OnMyWindow_Show()
        {
        }
        public virtual void OnMyWindow_Hide()
        {
        }

        private void WmGetMinMaxInfo(System.IntPtr hwnd, System.IntPtr lParam)
        {
            m_mmi = (MINMAXINFO)Marshal.PtrToStructure(lParam, typeof(MINMAXINFO));

            // Adjust the maximized size and position to fit the work area of the correct monitor
            int MONITOR_DEFAULTTONEAREST = 0x00000002;

            System.IntPtr monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (monitor != System.IntPtr.Zero)
            {
                MONITORINFO monitorInfo = new MONITORINFO();
                GetMonitorInfo(monitor, monitorInfo);

                RECT rcWorkArea = monitorInfo.rcWork;
                RECT rcMonitorArea = monitorInfo.rcMonitor;
                m_mmi.ptMaxPosition.x = Math.Abs(rcWorkArea.left - rcMonitorArea.left);
                m_mmi.ptMaxPosition.y = Math.Abs(rcWorkArea.top - rcMonitorArea.top);
                m_mmi.ptMaxSize.x = Math.Abs(rcWorkArea.right - rcWorkArea.left);
                m_mmi.ptMaxSize.y = Math.Abs(rcWorkArea.bottom - rcWorkArea.top);

                m_mmi.ptMinTrackSize.x = (int)MinWidth;
                m_mmi.ptMinTrackSize.y = (int)MinHeight; 
            }

            Marshal.StructureToPtr(m_mmi, lParam, true);
        }



        [StructLayout(LayoutKind.Sequential, Pack = 0)]
        public struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
            public static readonly RECT Empty = new RECT();
            public int Width
            {
                get { return Math.Abs(right - left); }  // Abs needed for BIDI OS
            }
            public int Height
            {
                get { return bottom - top; }
            }
            public RECT(int left, int top, int right, int bottom)
            {
                this.left = left;
                this.top = top;
                this.right = right;
                this.bottom = bottom;
            }

            public RECT(RECT rcSrc)
            {
                this.left = rcSrc.left;
                this.top = rcSrc.top;
                this.right = rcSrc.right;
                this.bottom = rcSrc.bottom;
            }
        }
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public class MONITORINFO
        {
            public int cbSize = Marshal.SizeOf(typeof(MONITORINFO));        
            public RECT rcMonitor = new RECT();       
            public RECT rcWork = new RECT();
            public int dwFlags = 0;
        }

        [DllImport("user32")]
        internal static extern bool GetMonitorInfo(IntPtr hMonitor, MONITORINFO lpmi);

        [DllImport("User32")]

        internal static extern IntPtr MonitorFromWindow(IntPtr handle, int flags);
        #endregion

        #region

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
        #endregion

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        private void ResizeWindow(ResizeDirection direction)
        {
            SendMessage(hs.Handle, WM_SYSCOMMAND, (IntPtr)(61440 + direction), IntPtr.Zero);
        }

        public virtual void OnMyWindow_Btn_Close_Click()
        {
            this.Close();
        }
        public virtual void OnMyWindow_Btn_Max_Click()
        {
            this.WindowState = WindowState.Maximized;
        }
        public virtual void OnMyWindow_Btn_Min_Click()
        {
            this.WindowState = WindowState.Minimized;
        }
        public virtual void OnMyWindow_Btn_Restore_Click()
        {
            this.WindowState = WindowState.Normal;
        }

        public string  SetSubTitle(string title)
        {
            ControlTemplate baseWindowTemplate = this.Template;
            Label lab_SubTitle = (Label)baseWindowTemplate.FindName("lab_SubTitle", this);
            if (null == lab_SubTitle) return "";
            lab_SubTitle.Content = title;
            return title;
        }
       
        private void InitializeEvent()
        {     
            ControlTemplate baseWindowTemplate = this.Template; //(ControlTemplate)App.Current.Resources["MyWindowTemplate"];
            Border borderClip = (Border)baseWindowTemplate.FindName("bdr_win", this);

            if (null != borderClip)
            {
                borderClip.MouseMove += delegate
                {
                    DisplayResizeCursor(null, null);
                };
                borderClip.PreviewMouseDown += delegate
                {
                    Resize(null, null);
                };
                this.PreviewMouseMove += delegate
                {
                    ResetCursor(null, null);
                };
            }

            Image img_LoginWin = (Image)baseWindowTemplate.FindName("img_LoginWin", this);
            if (null != img_LoginWin)
           {
               img_LoginWin.PreviewMouseLeftButtonDown += delegate
               {
                   this.DragMove();
               };
           }

            

            Button btn_Header = (Button)baseWindowTemplate.FindName("btn_Header", this);

            if (null != btn_Header)
            {
                btn_Header.PreviewMouseLeftButtonDown += delegate
                {
                    this.DragMove();
                };



                btn_Header.PreviewMouseDoubleClick += delegate
                {
                    if (this.WindowState == WindowState.Maximized)
                    {
                        this.WindowState = WindowState.Normal;
                    }
                    else
                    {
                        this.WindowState = WindowState.Maximized;
                    }

                };
            }


            Image img_SysClose = (Image)baseWindowTemplate.FindName("img_SysClose", this);

            if (null != img_SysClose)
            {
                img_SysClose.PreviewMouseLeftButtonUp += delegate
                {
                    OnMyWindow_Btn_Close_Click();
                };
            }

            Image img_SysMin = (Image)baseWindowTemplate.FindName("img_SysMin", this);
            if (null != img_SysMin)
            {
                img_SysMin.PreviewMouseLeftButtonUp += delegate
                {
                    OnMyWindow_Btn_Min_Click();
                };
            }

            Image img_SysMax = (Image)baseWindowTemplate.FindName("img_SysMax", this);
            Image img_SysRestore = (Image)baseWindowTemplate.FindName("img_SysRestore", this);

            if ((null != img_SysMax) && (null != img_SysRestore))
            {
                img_SysMax.PreviewMouseLeftButtonUp += delegate
                {
                    OnMyWindow_Btn_Max_Click();
                };

                img_SysRestore.PreviewMouseLeftButtonUp += delegate
                {
                    OnMyWindow_Btn_Restore_Click();
                };
            }

            this.SizeChanged += delegate
            {
                if (this.WindowState == WindowState.Maximized)
                {
                    img_SysMax.Visibility = Visibility.Collapsed;
                    img_SysRestore.Visibility = Visibility.Visible;

                    if (null != borderClip) borderClip.Margin = new Thickness(0);
                    relativeClip = -1;

                }
                else
                {
                    img_SysMax.Visibility = Visibility.Visible;
                    img_SysRestore.Visibility = Visibility.Collapsed;
                    if (null != borderClip) borderClip.Margin = new Thickness(10);
                    relativeClip = 14;
                }
            };
        }

        public new void DragMove()
        {
            if (this.WindowState == WindowState.Normal)
            {
                SendMessage(hs.Handle, WM_SYSCOMMAND, (IntPtr)0xf012, IntPtr.Zero);
                SendMessage(hs.Handle, WM_LBUTTONUP, IntPtr.Zero, IntPtr.Zero);
            }
        }

        private void DisplayResizeCursor(object sender, MouseEventArgs e)
        {
            Point pos = Mouse.GetPosition(this);
            double x = pos.X;
            double y = pos.Y;
            double w = this.ActualWidth;  //注意这个地方使用ActualWidth,才能够实时显示宽度变化
            double h = this.ActualHeight;

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

        private void ResetCursor(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed)
            {
                this.Cursor = Cursors.Arrow;
            }
        }

        private void Resize(object sender, MouseButtonEventArgs e)
        {
            Point pos = Mouse.GetPosition(this);
            double x = pos.X;
            double y = pos.Y;
            double w = this.ActualWidth;
            double h = this.ActualHeight;

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
    }
}
