using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using System.ComponentModel;
using Sigmar.Logger;

namespace Dispatcher.Modules
{
    public class CElement
    {
        [DefaultValue((int)0), JsonProperty(PropertyName = "id")]
        public int ID;
    }
}
