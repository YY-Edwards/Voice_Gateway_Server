using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Manager.Models
{
    public class BindElement
    {
        public RequestOpcode ResourceOpcode;
        public ResourcesManage.OperateType AssignOperate;
        public ResourcesManage.OperateType DetachOperate;

        public string SoureName;
        public string TargetName;
        

        private Dictionary<long, long> _Assign;
        private Dictionary<long, long> _Detach;

        private LogServer _LogServer{get{return LogServer.Instance();}}

        public BindElement()
       {
           ResourceOpcode = RequestOpcode.None;
           AssignOperate = ResourcesManage.OperateType.assignRadio;
           DetachOperate = ResourcesManage.OperateType.detachRadio;
           SoureName = string.Empty;
           TargetName = string.Empty;

           _Assign = new Dictionary<long, long>();
           _Detach = new Dictionary<long, long>();
       }

        public bool Save()
        {
            if (!Detach()) return false;
            if (!Assign()) return false;
            return true;
        }

        private bool Assign()
        {
            if (_Assign != null && _Assign.Count > 0)
            {
                if (ResourceOpcode == RequestOpcode.None)return false;

                foreach(var assign in _Assign)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", AssignOperate.ToString());
                    param.Add(SoureName, assign.Key);
                    param.Add(TargetName.ToString(), assign.Value);
                    
                   _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);        
                }
                _Assign.Clear();                
            }
            return true;
        }

        private bool Detach()
        {
            if (_Detach != null && _Detach.Count > 0)
            {
                if (ResourceOpcode == RequestOpcode.None) return false;

                foreach (var assign in _Detach)
                {
                    Dictionary<string, object> param = new Dictionary<string, object>();
                    param.Add("operation", DetachOperate.ToString());
                    param.Add(SoureName, assign.Key);
                    param.Add(TargetName, assign.Value);

                   _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);        
                }
                 _Detach.Clear();
            }

            return true;
        }


        public void Assign(long radioID, long targetId)
        {
            if (_Detach != null)
            {
                if (_Detach.ContainsKey(radioID))
                {
                    _Detach.Remove(radioID);
                }
            }

            if (_Assign != null) _Assign = new Dictionary<long, long>();
            if (_Assign.ContainsKey(radioID))
            {
                _Assign[radioID] = targetId;
            }
            else
            {
                _Assign.Add(radioID, targetId);
            }
        }

        public void Detach(long radioID, long targetId)
        {
            if (_Assign != null)
            {
                if (_Assign.ContainsKey(radioID))
                {
                    _Assign.Remove(radioID);
                }
            }

            if (_Detach != null) _Detach = new Dictionary<long, long>();
            if (_Detach.ContainsKey(radioID))
            {
                _Detach[radioID] = targetId;
            }
            else
            {
                _Detach.Add(radioID, targetId);
            }
        }
    }
}
