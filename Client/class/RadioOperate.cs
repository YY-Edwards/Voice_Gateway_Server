using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    public class RadioCallParam
    {
        public ExecType Operate;
        public TargetType Type;
        public long Target;
    }

    public class RadioSmsParam
    {
        public TargetType Type;
        public long Target;
        public long Source;
        public string Contents;
    }

    public class RadioArsParam
    {
        public long Target;
        public bool ISOnline;
    }

    public enum QueryGPSType
    {
        Generic = 12,
        GenericCycle = 13,
        CSBK = 14, 
        CSBKCycle = 15,
        Enh = 23,
        EnhCycyle = 24,
    }

    public class RadioGpsParam
    {
        public ExecType Operate;
        public QueryGPSType Type;
        public long Target;
        public double Cycle;
    }

    public class GPSValue
    {
        public bool Valid;
        public double Lon;
        public double Lat;
        public double Alt;
        public double Speed;

    }
    public class GPSParam
    {
        public long Source;
        public GPSValue Gps;
    }

    public class RadioControlsParam
    {
        public ControlType Type;
        public long Target;
    }
    

    public class RadioOperate
    {
        public RequestType Call;
        private long CallId;
        private List<object> Param;

        public ParseDel Parse(string json)
        {
            return null;         
        }

        private object ToStr(object param)
        {
            if (null == Param)
            {
                return new TServerRequestSimpleStr()
                {
                    call = Call.ToString(),
                    type = "radio",
                    callId = CallId.ToString(),
                };
            }
            else
            {
                return new TServerRequestStr()
                {
                    call = Call.ToString(),
                    type = "radio",
                    callId = CallId.ToString(),
                    param = param
                };
            }

        }

        public List<string> Json
        {
            get
            {
                List<string> json = new List<string>();
                if (null != Param)
                foreach (object obj in Param)
                {
                    json.Add (JsonConvert.SerializeObject(this.ToStr(obj)) + "\r\n");
                    CallId++;
                }

                return json;
            }
        }


        private TargetType GetTargetType(CMultMember target)
        {
            if (SelectionType.Null == target.Type) return TargetType.None;
            if (SelectionType.All == target.Type) return TargetType.All;
            if (target.Target == null || 0 >= target.Target.Count) return TargetType.None;
            if (MemberType.Group == target.Target[0].Type) return TargetType.Group;
            else if (null != target.Target[0].Radio && target.Target[0].Radio.ID > 0) return TargetType.Private;
            else return TargetType.None;
        }

        public RadioOperate(COperate operate, long PN)
        {
            Param = null;
            Call = RequestType.None;
            CallId = PN;

            if (operate == null || operate.Operate == null || operate.Target == null) return;
            try
            {
                if (operate.Type == OPType.Dispatch)
                {
                    TargetType targettype = GetTargetType(operate.Target);
                    if (TargetType.None != targettype)
                    {
                        Call = RequestType.call;
                        Param = BuildCallParam(operate.Operate as CDispatch, targettype, operate.Target);
                    }
                }
                else if (operate.Type == OPType.ShortMessage)
                {
                    TargetType targettype = GetTargetType(operate.Target);
                    if (TargetType.None != targettype)
                    {
                        Call = RequestType.message;
                        Param = BuildSmsParam(operate.Operate as CShortMessage, targettype, operate.Target);
                    }
                }
                else if(operate.Type == OPType.Position)
                {
                    TargetType targettype = GetTargetType(operate.Target);
                    if (TargetType.None != targettype && TargetType.All != targettype)
                    {
                        Call = RequestType.queryGps;
                        Param = BuildGpsParam(operate.Operate as CPosition, targettype, operate.Target);
                    }
                }
                else if(operate.Type == OPType.Control)
                {
                    TargetType targettype = GetTargetType(operate.Target);
                    if (TargetType.None != targettype && TargetType.All != targettype)
                    {
                        Call = RequestType.control;
                        Param = BuildControlParam(operate.Operate as CControl, targettype, operate.Target);
                    }
                }
            }
            catch
            {

            }

            CallId = PN;
        }

        private List<object> BuildCallParam(CDispatch op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();

            if (op != null && op.Exec == ExecType.Stop)
            {
                param.Add(new RadioCallParam() { Operate = ExecType.Stop });
            }
            else
            {

                switch (type)
                {
                    case TargetType.Private:
                        param.Add(new RadioCallParam()
                        {
                            Operate = ExecType.Start,
                            Type = TargetType.Private,
                            Target = target.Target[0].Radio.RadioID
                        });
                        break;
                    case TargetType.Group:
                        param.Add(new RadioCallParam()
                        {
                            Operate = ExecType.Start,
                            Type = TargetType.Group,
                            Target = target.Target[0].Group.GroupID
                        });break;
                    case TargetType.All:
                        param.Add(new RadioCallParam()
                        {
                            Operate = ExecType.Start,
                            Type = TargetType.All,
                        });break;
                    default:
                        break;
                }
            }

            return param;
        }

        private List<object> BuildSmsParam(CShortMessage op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();

            if(type == TargetType.All)
            {
                var group = TargetMgr.TargetList.Group.Where(p => p.Value.Group != null && p.Value.Group.ID > 0 && p.Value.Group.GroupID > 0);

                foreach (var item in group)
                {
                    param.Add(new RadioSmsParam()
                    {
                        Type = TargetType.Group,
                        Target = item.Value.Group.GroupID,
                        Contents = op.Message
                    });                     
                }

                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID <= 0  && p.Value.Radio != null  && p.Value.Radio.RadioID > 0);

                foreach (var item in radio)
                {
                    param.Add(new RadioSmsParam()
                    {
                        Type = TargetType.Private,
                        Target = item.Value.Radio.RadioID,
                        Contents = op.Message
                    });
                }
            }
            else if(type != TargetType.None)
            {
                foreach(CMember trgt in target.Target)
                {
                    if(trgt.Type == MemberType.Group)
                    {
                        param.Add(new RadioSmsParam()
                        {
                            Type = TargetType.Group,
                            Target = trgt.Group.GroupID,
                            Contents = op.Message
                        });
                    }
                    else
                    {
                        param.Add(new RadioSmsParam()
                        {
                            Type = TargetType.Private,
                            Target = trgt.Radio.RadioID,
                            Contents = op.Message
                        });
                    }

                }
            }

            return param;
        }

        private List<object> BuildGpsParam(CPosition op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();

            try
            {
                if (type == TargetType.All)
                {
                    var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                    foreach (var item in radio)
                    {
                        param.Add(new RadioGpsParam()
                        {
                            Operate = ExecType.Start,
                            Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
                            Target = item.Value.Radio.RadioID,
                            Cycle = op.Cycle
                        });

                        if (op.IsCycle && op.Type == ExecType.Start)
                            TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = true;
                        if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = false;
                    }
                }
                else if (type != TargetType.None)
                {
                    foreach (CMember trgt in target.Target)
                    {
                        if (trgt.Type == MemberType.Group)
                        {
                            var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == target.Target[0].Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                            foreach (var item in radio)
                            {
                                param.Add(new RadioGpsParam()
                                {
                                    Operate = ExecType.Start,
                                    Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
                                    Target = item.Value.Radio.RadioID,
                                    Cycle = op.Cycle
                                });

                                if (op.IsCycle && op.Type == ExecType.Start)
                                    TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = true;
                                if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = false;
                            }       
                        }
                        else
                        {
                            param.Add(new RadioGpsParam()
                            {
                                Operate = op.Type,
                                Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
                                Target = target.Target[0].Radio.RadioID,
                                Cycle = op.Cycle
                            });

                            if (op.IsCycle && op.Type == ExecType.Start)
                                TargetMgr.TargetList.Radio[target.Target[0].Radio.ID].Radio.IsGPS = true;
                            if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[target.Target[0].Radio.ID].Radio.IsGPS = false;
                        }

                    }
                }
            }
            catch{}

            return param;
        }

        private List<object> BuildControlParam(CControl op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();

            try
            {
                if (type == TargetType.All)
                {
                    var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                    foreach (var item in radio)
                    {
                        param.Add(new RadioControlsParam()
                        {
                            Type = op.Type,
                            Target = item.Value.Radio.ID,
                        });
                    }
                }
                else if (type != TargetType.None)
                {
                    foreach (CMember trgt in target.Target)
                    {
                        if (trgt.Type == MemberType.Group)
                        {
                            var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == target.Target[0].Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                            foreach (var item in radio)
                            {
                                param.Add(new RadioControlsParam()
                                {
                                    Type = op.Type,
                                    Target = item.Value.Radio.ID,
                                });
                            }
                        }
                        else
                        {
                            param.Add(new RadioControlsParam()
                            {
                                Type = op.Type,
                                Target = target.Target[0].Radio.RadioID,
                            });
                        }

                    }
                }
            }
            catch { }

            return param;
        }
    }
}
