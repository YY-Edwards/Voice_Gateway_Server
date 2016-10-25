using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    public class TcpRequset
    {
       public  string call { set; get; }
       public Int64 callId { set; get; }
       public object param { set; get; }
    }
    public class RequsetSimpleStru
    {
        public string call { set; get; }
        public Int64 callId { set; get; }
    }

    public class TcpResponse
    {
        public string status { set; get; }
        public string statusText { set; get; }
        public int errCode { set; get; }
        public Int64 callId { set; get; }
        public object contents { set; get; }

        public bool IsSuccess
        {
            get {
                if ("success" == status) return true;
                else return false;
            }
        }
    }

    class JsonParse
    {
        private static Int64 PackageNumber = 0;
        public static Int64 CallID
        {
            get { return PackageNumber; }
        }

        public static string Req2Json(RequestType request, object obj)
        {
            PackageNumber++;
            if (null == obj)
            {
                RequsetSimpleStru req = new RequsetSimpleStru() { call = request.ToString(), callId = PackageNumber};               
                return JsonConvert.SerializeObject(req) + "\r\n";
            }
            else
            {
                TcpRequset req = new TcpRequset() { call = request.ToString(), callId = PackageNumber, param = obj };
                return JsonConvert.SerializeObject(req) + "\r\n";
            }
        }

        public static object Json2Rep(string str)
        {
            TcpResponse res = JsonConvert.DeserializeObject<TcpResponse>(str) as TcpResponse;
            return res;
        }
    }
}
