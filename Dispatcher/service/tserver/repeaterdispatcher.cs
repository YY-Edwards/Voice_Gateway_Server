using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Dispatcher.Service
{
    public class RepeaterDispatcher : CDispatcher
    {

        private volatile static RepeaterDispatcher _instance = null;
        private static readonly object lockHelper = new object();

        public static RepeaterDispatcher Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new RepeaterDispatcher();
                }
            }
            return _instance;
        }



        public RepeaterDispatcher()
            : base(RequestType.wl)
        {

        }

        public override void CustomRequest(RequestOpcode call, string parameter)
        {
            switch (call)
            {
                case RequestOpcode.wlCall:
                    OnCallRequest(parameter);
                    break;                    
                default:
                    break;

            }
        }

        private void  OnCallRequest(string parameter)
        {

            if (parameter == null || parameter == "") return;
            try
            {
                RepeaterCallParameter res = JsonConvert.DeserializeObject<RepeaterCallParameter>(parameter);
                if (res != null)
                {

                    CallRequestArgs args =  new CallRequestArgs(res.operate)
                    {
                        Type = res.type,
                        Target = res.target,
                        Source = res.source,
                        IsCurrent = res.isCurrent,
                    };

                    OnCallRequest(args);
                }

            }
            catch (Exception ex)
            {
                //WARNING(ex);
            }
        }

        public override Session GetCallParameter(ExecType_t exec, TargetMode_t type, int target)
        {
            return new RepeaterCallParameter()
            {
                operate = exec,
                type = type,
                target = target,

                TargetMode = type,
                TargetId = target
            };
        }

        public override CallResponseArgs CreateCallResponse(string parameter)
        {
            if (parameter == null || parameter == "") return null;
            try
            {
                RepeaterCallResponse res = JsonConvert.DeserializeObject<RepeaterCallResponse>(parameter);
                if (res != null)
                {
                    return new CallResponseArgs(res.operate, res.status)
                    {
                        Type = res.type,
                        Target = res.target,
                        Source = res.source,
                        IsCurrent = res.isCurrent
                    };
                }
                return null;

            }
            catch (Exception ex)
            {
                //WARNING(ex);
                return null;
            }
        }

        public override void PlayVoice(int target)
        {
            RequestOpcode opcode = RequestOpcode.wlPlay;
            var param = new WirelanPlayParameter() { 
                target = target,

                //-1表示播放个呼，-2表示表示播放全呼，其余为组ID

                TargetMode = target == -1 ? TargetMode_t.Private : (target == -2 ? TargetMode_t.All : TargetMode_t.Group),
                TargetId = target,
            };
            Request(opcode, param);  
        }

        public override void Control(int target, ControlerType_t type)
        {
            return;
        }

    }
    public class RepeaterCallParameter:Session
    {
        public ExecType_t operate;
        public TargetMode_t type;
        public int source;
        public int target;
        public bool isCurrent;
    }

    public class RepeaterCallResponse
    {
        public int status;
        public ExecType_t operate;
        public TargetMode_t type;
        public int source;
        public int target;
        public bool isCurrent;
    }

    public class WirelanPlayParameter : Session
    {
        public int target;
    }
}
