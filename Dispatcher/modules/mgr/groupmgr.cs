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
    public class CDepartmentMgr : CResource
    {        
        public CDepartmentMgr()
            : base
                (RequestOpcode.department)
        {
            List.Add(new CDepartment() { Name = "test" });
        }
        
        public override CElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CDepartment>(json);
            }
            catch
            {
                return null;
            }
        }
    }

    public class CDepartment : CElement
    {

        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "gid")]
        public int GroupID { set; get; }
    }   
}
