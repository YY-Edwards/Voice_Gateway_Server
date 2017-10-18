using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dispatcher.Service;

using Sigmar.Logger;

namespace Dispatcher.Modules
{
    public class CGroup
    {
        public CallStatus_t CallStatus { get; set; }

        public int ID { get; set; }
        public int GroupID { get; set; }
        public string Name { get; set; }       
        public string ByName { get; set; }//for all target

        private bool _isalltarget;
        public bool IsAllTarget { get { return _isalltarget; } }

        public CGroup(bool isalltarget = false)
        {
            _isalltarget = isalltarget;
        }
    }
}
