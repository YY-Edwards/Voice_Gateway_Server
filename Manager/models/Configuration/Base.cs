﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.ComponentModel;

namespace Manager.Models
{
   public class Base:Configuration
   {
        public NetAddress TSvr { get; set; }
        public NetAddress LogSvr{ get; set; }
        public bool IsSaveCallLog { get; set; }
        public bool IsSaveMsgLog { get; set; }
        public bool IsSavePositionLog { get; set; }
        public bool IsSaveControlLog { get; set; }
        public bool IsSaveJobLog { get; set; }
        public bool IsSaveTrackerLog { get; set; }
        public bool IsSaveLocationInDoorLog { get; set; }
        public string LogPath { get; set; }

        public Base()          
        {
            TSvr = new NetAddress()
            {
                Ip = Utility.TServerHost,
                Port = Utility.TServerPort,
            };

            LogSvr = new NetAddress()
            {
                Ip = Utility.LogServerHost,
                Port = Utility.LogServerPort,
            };

            IsSaveCallLog = true;
            IsSaveMsgLog = true;
            IsSavePositionLog = true;

            IsSaveControlLog = false;
            IsSaveJobLog = false;
            IsSaveTrackerLog = false;
            IsSaveLocationInDoorLog = true;

            LogPath = App.runTimeDirectory;
        }
    }
}
