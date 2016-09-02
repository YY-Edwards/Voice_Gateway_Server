using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    
    public enum OPType
    {
        Dispatch,
        ShortMessage,
        Control,
        Position,
        JobTicker,
    };

    public enum TargetType
    {
        Radio,
        Group,
        All,
    };

     [Serializable]
    public class CDispatch
    {
        public CRelationShipObj target { set; get; }
        public TargetType targettype { set; get; }
    }

    [Serializable]
    public class CShortMessage
    {
        public CRelationShipObj target { set; get; }
        public TargetType targettype { set; get; }
        public string message { set; get; }
    };

    public enum ControlType
    {
        Check,
        Monitor,

        ShutDown,
        StartUp,
        Sleep,
        Week,
    };

    [Serializable]
    public class CControl
    {
        public CRelationShipObj target { set; get; }
        public TargetType targettype { set; get; }
        public ControlType type { set; get; }
    }

    public enum PositionType
    {
        Position,
        StartCycle,
        StopCycle
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
                if (true != isenh)
                    return csbkenhcyclelist;
                else
                    return csbkcyclelist;
            }
            else
                return normalcyclelist;

        }

        public CRelationShipObj target { set; get; }
        public TargetType targettype { set; get; }
        public CPosition type { set; get; }
        public bool iscycle { set; get; }
        public double cycle { set; get; }

        public bool iscsbk { set; get; }
        public bool isenh { set; get; }
       
    }

     [Serializable]
    public class COperate
    {
        public OPType type { set; get; }

        public CDispatch call { set; get; }
        public CShortMessage message { set; get; }
        public CControl control { set; get; }
        public CPosition position { set; get; }

        public COperate()
        {
        }

        public COperate(OPType tp, object obj)
        {
            call = null;
            message = null;
            control = null;
            position = null;
            switch (tp)
            {
                case OPType.Dispatch:
                    call = obj as CDispatch;
                    break;
                case OPType.ShortMessage:
                    message = obj as CShortMessage;
                    break;
                case OPType.Control:
                    control = obj as CControl;
                    break;
                case OPType.Position:
                    position = obj as CPosition;
                    break;
                case OPType.JobTicker:
                    break;
            }
        }
    }
}
