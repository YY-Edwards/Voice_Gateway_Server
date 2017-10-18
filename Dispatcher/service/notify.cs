using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Dispatcher.Service
{

    public class CNotice
    {
            public NotifyKey_t Type { set; get; }
            public int Source { set; get; }
            public DateTime Time { set; get; }
            public object Contents { set; get; } 
    }
}
