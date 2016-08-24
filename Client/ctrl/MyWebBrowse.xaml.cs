using CefSharp;
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
using System.Net;

using CefSharp.Wpf;

namespace TrboX
{
    /// <summary>
    /// MyWebBrowse.xaml 的交互逻辑
    /// </summary>
    public partial class MyWebBrowse : UserControl, IRequestHandler
    {
        private WebView _view;

        public MyWebBrowse(string url)
        {
            InitializeComponent();

            CEF.Initialize(new Settings { LogSeverity = LogSeverity.Disable, PackLoadingDisabled = true });

            BrowserSettings browserSetting = new BrowserSettings { ApplicationCacheDisabled = true, PageCacheDisabled = true };

            _view = new WebView(string.Empty, browserSetting)
            {
                Address = url,
                RequestHandler = this,
                Background = Brushes.White
            };

            _view.RegisterJsObject("callbackObj", new CallbackObjectForJs());

            _view.LoadCompleted += _view_LoadCompleted;

            MainGrid.Children.Insert(0, _view);
        }

        private void _view_LoadCompleted(object sender, LoadCompletedEventArgs url)
        {
            Dispatcher.BeginInvoke(new Action(() => 
            {
                _view.Visibility = Visibility.Visible;
                maskLoading.Visibility = Visibility.Collapsed;
            }));
        }

        public void View(string url)
        {
            if(_view.IsBrowserInitialized)
            {
                _view.Visibility = Visibility.Hidden;

                maskLoading.Visibility = Visibility.Visible;

                _view.Load(url);
            }
        }

        #region IRequestHandler
        public bool GetAuthCredentials(IWebBrowser browser, bool isProxy, string host, int port, string realm, string scheme, ref string username, ref string password)
        {
            return false;
        }

        public bool GetDownloadHandler(IWebBrowser browser, string mimeType, string fileName, long contentLength, ref IDownloadHandler handler)
        {
            return true;
        }

        public bool OnBeforeBrowse(IWebBrowser browser, IRequest request, NavigationType naigationvType, bool isRedirect)
        {
            return false;
        }

        public bool OnBeforeResourceLoad(IWebBrowser browser, IRequestResponse requestResponse)
        {
            return false;
        }

        public void OnResourceResponse(IWebBrowser browser, string url, int status, string statusText, string mimeType, WebHeaderCollection headers)
        {
            
        }
        #endregion
    }

    public class CallbackObjectForJs
    {
        public void showMessage(string msg)
        {
            MessageBox.Show(msg);
        }
    }
}
