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
using Dispatcher.Service;

namespace Dispatcher.Modules
{
    public class CAreaMgr : CResource
    {

        public CAreaMgr()
            : base
                (RequestOpcode.area)
        {
          
        }
        
        public override CElement Parse(string json)
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


        public void UploadMap()
        {
            if (UpdateList != null)
            {
                foreach(CArea area in UpdateList)
                {                   
                    CImgServer.Instance().Upload(area.Map, area.LocalPath);
                }
               
            }
            if (AddList != null)
            {
                foreach (CArea area in AddList)
                {
                    CImgServer.Instance().Upload(area.Map, area.LocalPath);
                }
            }
        }
    } 
}
