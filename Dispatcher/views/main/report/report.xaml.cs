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
using System.Windows.Interop;
using System.Runtime.InteropServices;

using Sigmar.Controls;
using Sigmar.Extension;
using Dispatcher.ViewsModules;
using Sigmar.Logger;

namespace Dispatcher.Views
{
    /// <summary>
    /// report.xaml 的交互逻辑
    /// </summary>
    public partial class Report : UserControl
    {
        public Report()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(report_loaded);
        }

        private void report_loaded(object sender, RoutedEventArgs e)
        {
           HwndSource hs = PresentationSource.FromVisual(this) as HwndSource;
           if (hs != null) hs.AddHook(new HwndSourceHook(WndProc));

           Log.Info("Report is Loaded");
        }
        
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0201://WM_LBUTTON_DWON
                case 0x0204://WM_RBUTTON_DWON
                    OnMousePrssed((int)wParam, (int)lParam);               
                    break;

                default: break;
            }
            return (System.IntPtr)0;
        }
        public void OnMousePrssed(int w, int p)
        {
            try
            {

                ViewsModules.VMReport vm = txt_CurrentPage.DataContext as ViewsModules.VMReport;
                if (vm == null) return;
                int page = txt_CurrentPage.Text.ToInt();
                if(page <= 0)return;
                //Point pp = Mouse.GetPosition(e.Source as FrameworkElement);//WPF方法

                Window window = Window.GetWindow(txt_CurrentPage);
                Point point = txt_CurrentPage.TransformToAncestor(window).Transform(new Point(0, 0));

                Console.Write("MouseX" + (p & 0xffff).ToString() + "  Y:" + ((p >> 16) & 0xffff).ToString() + "\r\n");
                Console.Write("ControlX" + point.X.ToString() + "  Y:" + point.Y.ToString() + "\r\n");

                double mouseX = (p & 0xffff);
                double mouseY = ((p >> 16) & 0xffff);


                if (mouseX < point.X || mouseX > point.X + txt_CurrentPage.ActualWidth) vm.PageChanged.Execute(page);
                if (mouseY < point.Y || mouseY > point.Y + txt_CurrentPage.ActualHeight) vm.PageChanged.Execute(page);
            }
            catch
            {

            }
        }
    }
}
