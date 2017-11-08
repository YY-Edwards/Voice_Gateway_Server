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
    public class Area : RElement
    {

        [JsonProperty(PropertyName = "name")]
        public string Name { set; get; }

        [JsonProperty(PropertyName = "map")]
        public string Map { set; get; }
       

        [JsonProperty(PropertyName = "width")]
        public string MapWidth { set; get; }

        [JsonProperty(PropertyName = "height")]
        public string MapHeight { set; get; }

        [JsonIgnore]
        public string MapName { set; get; }

        [JsonIgnore]
        public double Width
        {
            get
            {
                try
                {
                   return double.Parse(MapWidth);
                }
                catch
                {
                    return 100;
                }
            }
            set
            {
                MapWidth = value.ToString();
            }

        }

        [JsonIgnore]
        public double Height
        {
            get
            {
                try
                {
                    return double.Parse(MapHeight);
                }
                catch
                {
                    return 100;
                }
            }
            set
            {
                MapHeight = value.ToString();
            }
        }

        [JsonIgnore]
        public bool IsLocal { set; get; }

        [JsonIgnore]
        public string LocalPath { set; get; }

        public Area()
        {
            Name = string.Empty;
            Map = string.Empty;

            IsLocal = false;
            LocalPath = string.Empty;

            Height = 0;
            Height = 0;
        }

       
    }
    
}
