using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Sigmar;

namespace Dispatcher.Modules
{
    public class Mancar:Device
    {
        
        public Mancar()
        {
            HasDevice = false;
            ManCarType = ManCarType_t.Staff;
        }
        public ManCarType_t ManCarType;

        public long ManCarID { get; set; }
        public string ManCarName { set; get; }
        public Mancar SetMancarType(ManCarType_t type, bool hasdevice = true)
        {
            HasMancar = true;
            ManCarType = type;           
            HasDevice = hasdevice;
           
            return this;
        }

        public enum ManCarType_t
        {
            Staff,
            Vehicle,
        }
    }
}
