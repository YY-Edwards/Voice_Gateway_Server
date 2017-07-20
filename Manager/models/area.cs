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
    public class CAreaMgr : CResource
    {



        public CAreaMgr()
            : base
                (RequestOpcode.area)
        {
            List.Add(new CDepartment() { Name = "test" });
        }
        
        public override CRElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CArea>(json);
            }
            catch
            {
                return null;
            }
        }

        public void UpLoadMap()
        {
            foreach (CArea area in List)
            {
                if(System.IO.File.Exists(area.LocalPath))
                {
                    //upload area.LocalPath
                }
            }
        }
    }

    [Serializable]
    public class CArea : CRElement
    {

        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "map")]
        public string Map { set; get; }

        [JsonProperty(PropertyName = "width")]
        public string MapWidth { set { Width = double.Parse(value); } get { return Width.ToString(); } }


        [JsonProperty(PropertyName = "height")]
        public string MapHeight { set { Height = double.Parse(value); } get { return Height.ToString(); } }

         [JsonIgnore]
        public double Width { set; get; }

        [JsonIgnore]
         public double Height { set; get; }


        [JsonIgnore]
        public string LocalPath { set; get; }

        public CArea()
        {
            Name = string.Empty;
            Map = string.Empty;
        }

        public CDepartment Copy()
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, this);
            stream.Seek(0, SeekOrigin.Begin);
            return (CDepartment)new BinaryFormatter().Deserialize(stream);
        }
    }
    
}
