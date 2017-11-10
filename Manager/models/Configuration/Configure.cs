using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;

namespace Manager.Models
{
    public class Configure
    {
        public event Action<object, object> ReceivedConfiguration;
        public event Action<object> Timeout;

        private TServer _TServer{get{return TServer.Instance();}}
        
        public RequestOpcode ReadOpcode;
        public RequestOpcode SaveOpcode;
        public Configure()
        {           
            ReadOpcode = RequestOpcode.None;
            SaveOpcode = RequestOpcode.None;

            _TServer.Timeout += new Action<object, Server.Request>(OnTServerTimeout);
        }

        private void OnTServerTimeout(object sender, Server.Request request)
        {
            if (Timeout != null) Timeout(this);
        }
        public T Read<T>()
            where T:class, new()
        {
            if (_TServer == null || ReadOpcode == RequestOpcode.None) return null;
            Server.Response reponse = _TServer.SendRequest<Server.Response>(ReadOpcode, RequestType.radio);
            if(reponse != null && reponse.status.ToLower() == "success" )
            {
                try
                {
                    string contents = JsonConvert.SerializeObject(reponse.contents);
                    T configuration = JsonConvert.DeserializeObject<T>(contents);

                    if (ReceivedConfiguration != null) ReceivedConfiguration(this, configuration);

                    return configuration;
                }
                catch
                {

                }
            }
            
            return null;
        }

        public bool Save(object configuration)
        {
            if (_TServer == null || SaveOpcode == RequestOpcode.None) return false;
            Server.Response reponse = _TServer.SendRequest<Server.Response>(SaveOpcode, RequestType.radio, configuration);

            if(reponse != null && reponse.status.ToLower() == "success" )
            {
                return true;
            }

            return false;
        }  
    }
}
