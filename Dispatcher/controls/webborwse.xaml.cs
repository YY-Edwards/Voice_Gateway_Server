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

using CefSharp;
using CefSharp.Wpf;

using Sigmar.Controls;

namespace Dispatcher.Controls
{


    /// <summary>
    /// webborwse.xaml 的交互逻辑
    /// </summary>
    public partial class WebBrowser : UserControl,IRequestHandler
    {
        private WebView _view;

        public event CefSharp.LoadCompletedEventHandler LoadCompleted;

        public event WebBrowserJsOperationHandler WebBrowserJsOperation;

        public WebBrowser()
        {
            InitializeComponent();
            CEF.Initialize(new Settings { LogSeverity = LogSeverity.Disable, PackLoadingDisabled = true });
            this.Loaded +=delegate{InitializeWebPage();};
        }

        private bool IsInitialized = false;
        private void InitializeWebPage()
        {
            if (IsInitialized) return;
            IsInitialized = true;
            BrowserSettings browserSetting = new BrowserSettings { ApplicationCacheDisabled = true, PageCacheDisabled = true };
                
            _view = new WebView(string.Empty, browserSetting)
            {
                Address = Url,
                RequestHandler = this,
                Background = Brushes.White
            };

            CallbackObjectForJs callback = new CallbackObjectForJs();
            callback.WebBrowserJsOperation += new WebBrowserJsOperationHandler(OnWebBrowserJsOperation);
            _view.RegisterJsObject("callbackObj", callback);

            _view.LoadCompleted += _view_LoadCompleted;

             MainGrid.Children.Insert(0, _view);
        }

        private void OnWebBrowserJsOperation(WebBrowserJsOperationArgs e)
        {
            if (WebBrowserJsOperation != null) WebBrowserJsOperation(e);
        }
        public string Url
        {
            get { return (string)GetValue(UrlProperty); }
            set { SetValue(UrlProperty, value); }
        }
        public static readonly DependencyProperty UrlProperty = DependencyProperty.Register("Url", typeof(string), typeof(WebBrowser), new PropertyMetadata("", delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            WebBrowser web = source as WebBrowser;
            if (web != null)
            {
                web.InitializeWebPage();
                web.View(e.NewValue as string);
            }
        }));

        public string FileUrl
        {
            get { return (string)GetValue(FileUrlProperty); }
            set { SetValue(FileUrlProperty, value); }
        }
        public static readonly DependencyProperty FileUrlProperty = DependencyProperty.Register("FileUrl", typeof(string), typeof(WebBrowser), new PropertyMetadata("", delegate(DependencyObject source, DependencyPropertyChangedEventArgs e)
        {
            WebBrowser web = source as WebBrowser;
            if (web != null)
            {
                web.Url = "file:///" + e.NewValue as string;
                web.InitializeWebPage();
                web.View(web.Url);
            }
        }));
        
           
        private void _view_LoadCompleted(object sender, LoadCompletedEventArgs url)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                _view.Visibility = Visibility.Visible;
                maskLoading.Visibility = Visibility.Collapsed;

                if (null != LoadCompleted) LoadCompleted(this, url);
            }));
        }

        public void ExecJs(string jsfunc)
        {
            _view.ExecuteScript(jsfunc);
        }


        public void View(string url)
        {
            if (_view.IsBrowserInitialized)
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
        public event WebBrowserJsOperationHandler WebBrowserJsOperation;
        public void executeOperation(string opcode, string parameter)
        {
            int i = 0;
            if (WebBrowserJsOperation != null) WebBrowserJsOperation(new WebBrowserJsOperationArgs(opcode, parameter));
        }
    }

    public delegate void WebBrowserJsOperationHandler(WebBrowserJsOperationArgs e);
    public class WebBrowserJsOperationArgs
    {
        public string Opcode;
        public string Parameter;

        public WebBrowserJsOperationArgs(string opcode, string parameter)
        {
            Opcode = opcode;
            Parameter = parameter;
        }
    }
}
