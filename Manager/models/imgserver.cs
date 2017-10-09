using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Net;

namespace Manager
{
    public class CImgServer
    {
        public string Url = "http://127.0.0.1:8001/images/";
        private string UploadServer = "http://127.0.0.1:8001/upload";
        //public string Url = "http://192.168.2.133:8001/images/";
        //private string UploadServer = "http://192.168.2.133:8001/upload";

        private volatile static CImgServer _instance = null;
        private static readonly object lockHelper = new object();

        public static CImgServer Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new CImgServer();
                }
            }
            return _instance;
        }

        public void Upload(string name, string local)
        {
            if (!System.IO.File.Exists(local)) return;

            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    WebClient wc = new WebClient();
                    wc.Credentials = CredentialCache.DefaultCredentials;
                    wc.Headers.Add("Content-Type", "application/x-www-form-urlencoded");
                    wc.QueryString["fname"] = name;
                    byte[] fileb = wc.UploadFile(new Uri(UploadServer), "POST", local);
                    string res = Encoding.GetEncoding("gb2312").GetString(fileb);
                    Console.WriteLine(res);
                }
                catch (Exception ex)
                {

                }

            })).Start();
        }
    }
}
