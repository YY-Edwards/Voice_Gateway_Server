using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace TrboX
{
    
    public enum OPType
    {
        Dispatch,
        ShortMessage,
        Control,
        Position,
        JobTicker,
        Tracker,
    };

    //public enum TargetType
    //{
    //    Radio,
    //    Group,
    //    All,
    //};

    public enum ExecType
    {
        Start = 0,
        Stop = 1,
    }

     [Serializable]
    public class CDispatch
    {
         public ExecType Exec{ set; get; }
         public bool IsEqual(CDispatch dispatch)
         {
             return true;
         }
    }

    [Serializable]
    public class CShortMessage
    {
        public string Message { set; get; }

        public bool IsEqual(CShortMessage msg)
        {
            if (null == msg) return false;

            if (Message == msg.Message)
                return true;

            return false;
        }
    };

    public enum ControlType
    {
        Check = 0,
        Monitor = 1,

        ShutDown = 2,
        StartUp = 3,
        Sleep = 4,
        Week = 5,
    };

    [Serializable]
    public class CControl
    {
        public ControlType Type { set; get; }

        public bool IsEqual(CControl control)
        {
            if (null == control) return false;

            if (Type == control.Type)
                return true;

            return false;
        }
    }

     [Serializable]
    public class CPosition
    {
        private static List<double> normalcyclelist = new List<double>() { 30, 60, 120, 240 };
        private static List<double> csbkcyclelist = new List<double>() { 7.5, 20, 30, 40, 60 };
        private static List<double> csbkenhcyclelist = new List<double>() { 7.5, 15, 30, 60, 120, 240, 480 };

        public static List<double> UpdateCycleList(bool iscsbk, bool isenh)
        {
            if (true == iscsbk)
            {
                if (true == isenh)
                    return csbkenhcyclelist;
                else
                    return csbkcyclelist;
            }
            else
                return normalcyclelist;

        }
        public ExecType Type { set; get; }
        public bool IsCycle { set; get; }
        public double Cycle { set; get; }

        public bool IsCSBK { set; get; }
        public bool IsEnh { set; get; }


        public bool IsEqual(CPosition position)
        {
            if (null == position)return false;

            if((Type == position.Type) 
                &&(IsCycle == position.IsCycle)
                && (Cycle == position.Cycle)
                && (IsCSBK == position.IsCSBK)
                && (IsEnh == position.IsEnh)
                ) return true;

            return false;            
        }
       
    }

     [Serializable]
     public class CJobTicket
     {
            public bool IsEqual(CJobTicket job)
            {
                return true;
            }
     }

     [Serializable]
     public class CTacker
     {
            public bool IsEqual(CTacker tacker)
            {
                return true;
            }
     }

    [Serializable]
    public class COperate
    {
        public OPType Type { set; get; }

        public CMultMember Target { set; get; }
        public object Operate { set; get; }

        //public CDispatch call { set; get; }
        //public CShortMessage message { set; get; }
        //public CControl control { set; get; }
        //public CPosition position { set; get; }

        public COperate()
        {
        }

        public bool IsEqual(COperate operate)
        {
            if ((null == operate)||(Type != operate.Type)) return false;

            if((null == Target) && (null == operate.Target))
            {}
            else if ((null == Target) || (null == operate.Target) || (!Target.IsEqual(operate.Target))) return false;

            if((null == Target) && (null == operate.Target))return true;

            switch(Type)
            {
                case OPType.Dispatch:return (null == Operate) ? false: ((CDispatch)Operate).IsEqual((CDispatch)operate.Operate);                    
                case OPType.ShortMessage: return (null == Operate) ? false: ((CShortMessage)Operate).IsEqual((CShortMessage)operate.Operate);
                case OPType.Control: return (null == Operate) ? false: ((CControl)Operate).IsEqual((CControl)operate.Operate);
                case OPType.Position: return (null == Operate) ? false: ((CPosition)Operate).IsEqual((CPosition)operate.Operate);
                case OPType.JobTicker: return (null == Operate) ? false: ((CJobTicket)Operate).IsEqual((CJobTicket)operate.Operate);
                case OPType.Tracker: return (null == Operate) ? false: ((CTacker)Operate).IsEqual((CTacker)operate.Operate);
                default:return false;
            }
        }


        public string Name
        {
            get {
                switch (Type)
                {
                    case OPType.Dispatch: return "呼叫" + (((null == Target)||("" == Target.Name)) ? "" : ("->" + Target.Name));
                    case OPType.ShortMessage: return "发送短消息" + (((null == Target) || ("" == Target.Name)) ? "" : ("->" + Target.Name));
                    case OPType.Control: return "信令" + (((null == Target) || ("" == Target.Name)) ? "" : ("->" + Target.Name));
                    case OPType.Position: return "位置查询" + (((null == Target) || ("" == Target.Name)) ? "" : ("->" + Target.Name));
                    case OPType.JobTicker: return "发送工单" + (((null == Target) || ("" == Target.Name)) ? "" : ("->" + Target.Name));
                    case OPType.Tracker: return "追踪" + (((null == Target) || ("" == Target.Name)) ? "" : ("->" + Target.Name));
                    default: return "";
                }
            }
        }

        public string Information
        {
            get
            {
                //switch (Type)
                //{
                //    case OPType.Dispatch: return "";
                //    case OPType.ShortMessage: return (null == Operate) ? "" : ((CShortMessage)Operate).Message;
                //    case OPType.Control: return "信令" + ((null == Target) ? "" : ("　" + Target.Name));
                //    case OPType.Position: return "位置查询" + ((null == Target) ? "" : ("　" + Target.Name));
                //    case OPType.JobTicker: return "发送工单" + ((null == Target) ? "" : ("　" + Target.Name));
                //    case OPType.Tracker: return "追踪" + ((null == Target) ? "" : ("　" + Target.Name));
                //    default: return "";
                //}

                return "";
            }
        }

         public string NameInfo
        {
            get {
                return Name + (("" == Information) ? "" : ("," + Information));
            }
        }

        //public COperate(OPType tp, object obj)
        //{
        //    type = tp;
            
        //    call = null;
        //    message = null;
        //    control = null;
        //    position = null;
        //    switch (type)
        //    {
        //        case OPType.Dispatch:
        //            call = obj as CDispatch;
        //            break;
        //        case OPType.ShortMessage:
        //            message = obj as CShortMessage;
        //            break;
        //        case OPType.Control:
        //            control = obj as CControl;
        //            break;
        //        case OPType.Position:
        //            position = obj as CPosition;
        //            break;
        //        case OPType.JobTicker:
        //            break;
        //    }
        //}

        public COperate(OPType type, CMultMember target, object obj)
        {
            Type = type;
            Target = target;
            Operate = obj;           
        }

        public object Exec()
        {
            DataBase.InsertLog("Execute Operate:>>" + Type.ToString() + JsonConvert.SerializeObject(Operate) + "<<" + Target.NameInfo);
            
            if(Type == OPType.Dispatch && ((CDispatch)Operate).Exec == ExecType.Start && TServer.IsInCalled)
            {
                MyWindow.PushMessage(new CustomMessage(DestType.AddEvent, "提示：呼叫失败， 存在一个未完成的呼叫"));
                return false;
            }

            TServer.Call(this);

            return true;
        }
    }
}
