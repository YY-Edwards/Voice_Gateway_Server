using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class COperation
    {
        public TaskType_t Type { set; get; }
        public OperationAgrs Args { set; get; }

        public string Name 
        {
            get
            {
                switch (Type)
                {
                    case TaskType_t.Schedule: return "呼叫";
                    case TaskType_t.ShortMessage: return "短消息";
                    case TaskType_t.Controler: return "信令控制";
                    case TaskType_t.Location: return "位置查询";
                    case TaskType_t.LocationInDoor: return "室内定位";
                    case TaskType_t.JobTicket: return "工单";
                    case TaskType_t.Patrol: return "巡更";
                    default: return "";
                }
            }
        }

        public COperation(TaskType_t type, OperationAgrs args = null)
        {
            Type = type;
            if(args == null)
            {
                switch (Type)
                {
                    case TaskType_t.Schedule: Args = new CallArgs(CallOperatedType_t.Start); break;
                    case TaskType_t.ShortMessage: Args = new ShortMessageArgs(null); break;
                    case TaskType_t.Controler: Args = new ControlArgs(ControlerType_t.Check); break;
                    case TaskType_t.Location: Args = new LocationArgs(LocationType_t.Immediate); break;
                    case TaskType_t.LocationInDoor: Args = new  LocationInDoorArgs(LocationType_t.Immediate); break;
                    default: Args = null;
                        break;
                }
            }
            else
            {
                Args = args;
            }
            
        }

        public bool Equal(COperation dest)
        {
            if (dest == null) return false;
            if (dest.Type != Type) return false;
            else if (Args != null) return Args.Equal(dest.Args);
            else return dest.Args == null;
        }
    }
}
