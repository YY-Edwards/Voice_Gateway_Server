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

namespace Manager.Models
{
    public class Group : RElement
    {        
        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "gid")]
        public int GroupID { set; get; }

        public Group()
        {
            Name = string.Empty; ;
            GroupID =  0;
        }
    }   
}
