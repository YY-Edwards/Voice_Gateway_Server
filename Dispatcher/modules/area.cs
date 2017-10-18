using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using Newtonsoft.Json;

namespace Dispatcher.Modules
{
    public class CArea : CElement
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


        private Dictionary<string, string> m_WaitUpload = new Dictionary<string, string>();

        public CArea()
        {
            Name = string.Empty;
            Map = string.Empty;
        }
    }
}
