using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Manager.Models
{
    public class Configuration
    {
        public Configuration()
        {

        }

        public enum LocationQueryType_t
        {
            General = 0,
            CSBK = 1,
            EnhCSBK = 2
        }

        public class NetAddress
        {
            public string Ip { get; set; }
            public int Port { get; set; }

            public NetAddress()
            {

            }

            public NetAddress(string ip, int port)
            {
                Ip = ip;
                Port = port;
            }
        };
    }
}
