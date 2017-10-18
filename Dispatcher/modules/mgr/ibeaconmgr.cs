using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using Dispatcher.Service;
namespace Dispatcher.Modules
{
    public class CBeaconMgr : CResource
    {

        public CBeaconMgr()
            : base
                (RequestOpcode.ibeacon)
        {
            List.Add(new CBeacon() { Name = "test" });
        }
        
        public override CElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CBeacon>(json);
            }
            catch
            {
                return null;
            }
        }
    }
}
