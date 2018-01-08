using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using System.ComponentModel;
using Dispatcher;

namespace Dispatcher.Modules
{
    public class CElement
    {
        [DefaultValue((long)0), JsonProperty(PropertyName = "id")]
        public long ID;
    }
}
