using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Net;
using System.IO;

//using Sigmar.Logger;
namespace Manager.Models
{
    public class HttpServer
    {
        #region Singleton

        private volatile static HttpServer _instance = null;
        private static readonly object singletonLockHelper = new object();

        public static HttpServer Instance()
        {
            if (_instance == null)
            {
                lock (singletonLockHelper)
                {
                    if (_instance == null)
                        _instance = new HttpServer();
                }
            }

            return _instance;
        }

        #endregion Singleton

        public string ImagesUrl { get { return string.Format("http://{0}:{1}/images/", Utility.HttpServerHost,Utility.HttpServerPort );} }      
        private string UploadServer { get { return string.Format("http://{0}:{1}/upload/", Utility.HttpServerHost,Utility.HttpServerPort );} }


        private int Upload_Request(string address, string fileNamePath, string saveName)
        {
            int returnValue = 0;

            FileStream fs = new FileStream(fileNamePath, FileMode.Open, FileAccess.Read);
            BinaryReader r = new BinaryReader(fs);
            
            string strBoundary = "----------" + DateTime.Now.Ticks.ToString("x");
            byte[] boundaryBytes = Encoding.ASCII.GetBytes("\r\n--" + strBoundary + "\r\n");

            StringBuilder sb = new StringBuilder();
            sb.Append("--");
            sb.Append(strBoundary);
            sb.Append("\r\n");
            sb.Append("Content-Disposition: form-data; name=\"");
            sb.Append("file");
            sb.Append("\"; filename=\"");
            sb.Append(saveName);
            sb.Append("\"");
            sb.Append("\r\n");
            sb.Append("Content-Type: ");
            sb.Append("application/octet-stream");
            sb.Append("\r\n");
            sb.Append("\r\n");
            string strPostHeader = sb.ToString();
            byte[] postHeaderBytes = Encoding.UTF8.GetBytes(strPostHeader);


            HttpWebRequest httpReq = (HttpWebRequest)WebRequest.Create(new Uri(address));
            httpReq.Method = "POST";

  
            httpReq.AllowWriteStreamBuffering = false;
  
            httpReq.Timeout = 300000;
            httpReq.ContentType = "multipart/form-data; boundary=" + strBoundary;
            long length = fs.Length + postHeaderBytes.Length + boundaryBytes.Length;
            long fileLength = fs.Length;
            httpReq.ContentLength = length;
            try
            {

                //每次上传4k   
                int bufferLength = 4096;
                byte[] buffer = new byte[bufferLength];


                //已上传的字节数   
                long offset = 0;


                //开始上传时间   
                DateTime startTime = DateTime.Now;
                int size = r.Read(buffer, 0, bufferLength);
                Stream postStream = httpReq.GetRequestStream();


                //发送请求头部消息   
                postStream.Write(postHeaderBytes, 0, postHeaderBytes.Length);
                while (size > 0)
                {
                    postStream.Write(buffer, 0, size);
                    offset += size;
                    //TimeSpan span = DateTime.Now - startTime;
                    //double second = span.TotalSeconds;
                   //Console.WriteLine("已用时：" + second.ToString("F2") + "秒");
                   // if (second > 0.001)
                   // {
                   //      Console.WriteLine(" 平均速度：" + (offset / 1024 / second).ToString("0.00") + "KB/秒");
                   // }
                   // else
                   // {
                   //     Console.WriteLine(" 正在连接…");
                   // }
                   // Console.WriteLine("已上传：" + (offset * 100.0 / length).ToString("F2") + "%");
                   // Console.WriteLine(((offset / 1048576.0).ToString("F2") + "M/" + (fileLength / 1048576.0).ToString("F2") + "M"));
                   // //Application.DoEvents();
                    size = r.Read(buffer, 0, bufferLength);
                }
                //添加尾部的时间戳   
                postStream.Write(boundaryBytes, 0, boundaryBytes.Length);
                postStream.Close();


                //获取服务器端的响应   
                WebResponse webRespon = httpReq.GetResponse();
                Stream s = webRespon.GetResponseStream();
                StreamReader sr = new StreamReader(s);


                //读取服务器端返回的消息   
                String sReturnString = sr.ReadToEnd();//sr.ReadLine();  
                s.Close();
                sr.Close();
                if (sReturnString == "Success")
                {
                    returnValue = 1;
                }
                else if (sReturnString == "Error")
                {
                    returnValue = 0;
                }


            }
            catch
            {
                returnValue = 0;
            }
            finally
            {
                fs.Close();
                r.Close();
            }


            return returnValue;
        }  

        public void Upload(string name, string local)
        {
            if (!System.IO.File.Exists(local)) return;

            new Task(() => {
                Upload_Request(UploadServer, local, name);           
            }).Start();
        }
    }
}
