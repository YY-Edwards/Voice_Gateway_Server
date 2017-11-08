using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;

namespace Manager.Models
{
    public class Location : Configuration
    {
        public bool IsEnable{get; set;}
        public double Interval{get; set;}
        public bool IsEnableGpsC{get; set;}
        public NetAddress GpsC{get; set;}
        public Location()
        {
            IsEnable = false;
            Interval = 60;
            IsEnableGpsC = false;
            GpsC = new NetAddress();
        }
    }
}
