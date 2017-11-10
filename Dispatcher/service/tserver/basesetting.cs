using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;
using System.ComponentModel;

namespace Dispatcher.Service
{
   public class CBaseSetting:CConfiguration
   {
       public NetAddress TSvr { get; set; }
       public NetAddress LogSvr { get; set; }
       public bool IsSaveCallLog { get; set; }
       public bool IsSaveMsgLog { get; set; }
       public bool IsSavePositionLog { get; set; }
       public bool IsSaveControlLog { get; set; }
       public bool IsSaveJobLog { get; set; }
       public bool IsSaveTrackerLog { get; set; }
       public bool IsSaveLocationInDoorLog { get; set; }
       public string LogPath { get; set; }

        public CBaseSetting()
            : base(
            SettingType.Base,
            RequestOpcode.setBaseSetting,
            RequestOpcode.getBaseSetting)
        {
            InitializeValue();
        }

        public override object Parse(string json)
        {
            try
            {
                CBaseSetting tserver  = JsonConvert.DeserializeObject<CBaseSetting>(json);
                TSvr = tserver.TSvr ?? new NetAddress()
                {
                    Ip = "127.0.0.1",
                    Port = 9000
                };
                LogSvr = tserver.LogSvr ?? new NetAddress()
                {
                    Ip = "127.0.0.1",
                    Port = 9003
                };

                IsSaveCallLog = tserver.IsSaveCallLog;
                IsSaveMsgLog = tserver.IsSaveMsgLog;
                IsSavePositionLog = tserver.IsSavePositionLog;

                IsSaveControlLog = tserver.IsSaveControlLog;
                IsSaveJobLog = tserver.IsSaveJobLog;
                IsSaveTrackerLog = tserver.IsSaveTrackerLog;
                IsSaveLocationInDoorLog = tserver.IsSaveLocationInDoorLog;
                LogPath = tserver.LogPath;

                return this;
            }
            catch
            {
                InitializeValue();
                return this;
            }

        }

       private void InitializeValue()
       {
           TSvr = new NetAddress()
           {
               //Ip = Utility.TServerHost,
               //Port = Utility.TServerPort,
           };

           LogSvr = new NetAddress()
           {
               //Ip = Utility.LogServerHost,
               //Port = Utility.LogServerPort,
           };

           IsSaveCallLog = true;
           IsSaveMsgLog = true;
           IsSavePositionLog = true;

           IsSaveControlLog = false;
           IsSaveJobLog = false;
           IsSaveTrackerLog = false;
           IsSaveLocationInDoorLog = true;

           LogPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments) + "\\Trbox3.0\\Log\\";
       }
    }
}
