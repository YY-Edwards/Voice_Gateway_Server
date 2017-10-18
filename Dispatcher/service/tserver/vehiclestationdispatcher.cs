using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Dispatcher.Service
{
    public class VehicleStationDispatcher:CDispatcher
    {
        private volatile static VehicleStationDispatcher _instance = null;
        private static readonly object lockHelper = new object();

        public static VehicleStationDispatcher Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new VehicleStationDispatcher();
                }
            }
            return _instance;
        }



        public VehicleStationDispatcher()
            : base(RequestType.radio)
        {

        }

    }
}
