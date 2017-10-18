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

using Sigmar.Logger;
namespace Dispatcher.Views
{
    /// <summary>
    /// amap.xaml 的交互逻辑
    /// </summary>
    public partial class Amap : UserControl
    {
        public Amap()
        {
            InitializeComponent();
            InitializedAmap();
            this.Loaded += new RoutedEventHandler(window_loaded);
        }
        private void window_loaded(object sender, RoutedEventArgs e)
        {

            map.WebBrowserJsOperation += new Controls.WebBrowserJsOperationHandler(WebBrowserJsOperation);
            InitializedAmap();

            Log.Info("Amap Is Loaded");
        }

        private bool isInitialized = false;
 
        private void InitializedAmap()
        {           
            VMAmap _amapviewmodule = this.DataContext as VMAmap;
            if (_amapviewmodule != null)
            {
                if (_amapviewmodule.IsInitializeLocation) OnLocationInitialized(_amapviewmodule.Entrance);

                if (isInitialized) return;
                isInitialized = true;

                _amapviewmodule.InitializeLocation += new VMAmap.InitializeLocationHandler(OnLocationInitialized);
                _amapviewmodule.OnDrawPoint += new VMAmap.DrawPointHandler(OnDrawPoint);
                
            }
        }
        private void  OnLocationInitialized(string entrance)
        {
            map.FileUrl = entrance;
        }

        private void OnDrawPoint(DrawLocationReportArgs e)
        {
            double middleLat, middleLon;
            GPSTransform.transform(e.Report.Lat, e.Report.Lon, out middleLat, out middleLon);

            string paramformat = "DisPosPoint({0},{1},{2},{3}, {4}, {5}, '{6}', '{7}',{8},{9},'{10}', {11},'{12}')";
            try
            {
                VMTarget group = ResourcesMgr.Instance().Groups.Find(p => p.Group.GroupID == e.Source.Member.GroupID);
                string param = String.Format(paramformat,
                    e.Source.RadioID,//radioid 
                    e.Source.Member.DeviceType == Modules.Device.DeviceType_t.Handset ? 0: 1, //raido type radio 0. ride 1
                    e.Report.Lon,//long
                    e.Report.Lat,//lat
                    e.Report.Alt,//alt
                    e.Report.Speed,//speed
                    DateTime.Now.ToShortDateString(),
                    DateTime.Now.ToLongTimeString(),
                    middleLon,//middle long
                    middleLat,//middle lat
                    e.Source.Name,
                    group == null ? "" : group.Group.GroupID.ToString(), 
                    group == null ? "" : group.Name
                    );

                map.ExecJs(param);
            }
            catch(Exception ex)
            {
                //WARNING("Drawing Location Point Error", ex);
            }
        }
        private void WebBrowserJsOperation(Controls.WebBrowserJsOperationArgs e)
        {

        }
    }
}
