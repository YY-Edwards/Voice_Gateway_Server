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

namespace Manager
{
    public class CDepartmentMgr : CResource
    {        
        public CDepartmentMgr()
            : base
                (RequestOpcode.department)
        {
            List.Add(new CDepartment() { Name = "test" });
        }
        
        public override CRElement Parse(string json)
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

    [Serializable]
    public class CDepartment : CRElement
    {

        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "gid")]
        public long GroupID { set; get; }

        public CDepartment Copy()
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, this);
            stream.Seek(0, SeekOrigin.Begin);
            return (CDepartment)new BinaryFormatter().Deserialize(stream);
        }
    }
    
}
