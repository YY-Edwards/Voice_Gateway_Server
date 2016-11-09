using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Threading;

namespace TrboX
{

    public class RadioStatus
    {
        public long radioId;
        public bool IsInGps;
        public bool IsOnline;
    }

    public enum StatusType
    {
        ContectStatus = 1,
        RadioStatus = 2,
    }
    public class RadioStatusParam
    {
        public long getType;
        public object info;
    }
    public class RadioCallParam
    {
        public ExecType Operate;
        public TargetType Type;
        public long Target;
    }

    public class RadioCallStatusParam
    {
        public ExecType Operate;
        public TargetType Type;
        public long Target;
        public int Status;
    }


    public class RadioSmsParam
    {
        public TargetType Type;
        public long Target;
        public long Source;
        public string Contents;
    }

    public class RadioSmsStatusParam
    {
        public TargetType Type;
        public long Target;
        public long Source;
        public string Contents;
        public int Status;
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

    public class RadioGpsStatusParam
    {
        public ExecType Operate;
        public QueryGPSType Type;
        public long Target;
        public double Cycle;
        public int Status;
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

    public class RadioControlsStatusParam
    {
        public ControlType Type;
        public long Target;
        public int Status;
    }
    

    public class RadioOperate
    {
        public RequestType Call;
        private long CallId;
        public List<object> Param;

        public static RadioCallParam LastCall;

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
                    if (TargetType.None != targettype)
                    {
                        Call = RequestType.queryGps;
                        Param = BuildGpsParam(operate.Operate as CPosition, targettype, operate.Target);
                    }
                }
                else if(operate.Type == OPType.Control)
                {
                    TargetType targettype = GetTargetType(operate.Target);
                    if (TargetType.None != targettype)
                    {
                        Call = RequestType.control;
                        Param = BuildControlParam(operate.Operate as CControl, targettype, operate.Target);
                    }
                }
            }
            catch (Exception e)
            {
                DataBase.InsertLog("RadioOperate:" + e.Message);
            }

            CallId = PN;
        }

        private List<object> BuildCallParam(CDispatch op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();


            if(op == null)return param;

            switch (type)
            {
                case TargetType.Private:
                    param.Add(new RadioCallParam()
                    {
                        Operate = op.Exec,
                        Type = TargetType.Private,
                        Target = target.Target[0].Radio.RadioID
                    });
                    break;
                case TargetType.Group:
                    param.Add(new RadioCallParam()
                    {
                        Operate = op.Exec,
                        Type = TargetType.Group,
                        Target = target.Target[0].Group.GroupID
                    });break;
                case TargetType.All:
                    param.Add(new RadioCallParam()
                    {
                        Operate = op.Exec,
                        Type = TargetType.All,
                    });break;
                default:
                    break;
            }
          
            try{
                LastCall = param[0] as RadioCallParam;
            }
            catch (Exception e)
            {
                DataBase.InsertLog("BuildCallParam:" + e.Message);
            }

            return param;
        }

        private List<object> BuildSmsParam(CShortMessage op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();
            List<int> groupid = new List<int>();
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
                        if (trgt.Group == null || trgt.Group.ID <= 0) continue;
                        groupid.Add(trgt.Group.ID);

                        param.Add(new RadioSmsParam()
                        {
                            Type = TargetType.Group,
                            Target = trgt.Group.GroupID,
                            Contents = op.Message
                        });                        
                    }
                }

                foreach (CMember trgt in target.Target)
                {
                    if (trgt.Type != MemberType.Group)
                    {
                        if (trgt.Radio == null || trgt.Radio.ID <= 0) continue;
                        if (trgt.Group != null || trgt.Group.ID > 0)
                        {
                            if (groupid.Contains(trgt.Group.ID)) continue;
                        }

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
            List<int> groupid = new List<int>();
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
                    }
                }
                else if (type != TargetType.None)
                {
                    foreach (CMember trgt in target.Target)
                    {                    
                        if (trgt.Type == MemberType.Group)
                        {
                            if (trgt.Group == null || trgt.Group.ID <= 0) continue;
                            groupid.Add(trgt.Group.ID);

                            var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == trgt.Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                            foreach (var item in radio)
                            {
                                param.Add(new RadioGpsParam()
                                {
                                    Operate = op.Type,
                                    Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
                                    Target = item.Value.Radio.RadioID,
                                    Cycle = op.Cycle
                                });

                            }       
                        }

                    }

                    foreach (CMember trgt in target.Target)
                    {
                        if (trgt.Type != MemberType.Group)
                        {
                            if (trgt.Radio == null || trgt.Radio.ID <= 0) continue;
                            if (trgt.Group != null || trgt.Group.ID > 0)
                            {
                                if (groupid.Contains(trgt.Group.ID)) continue;
                            }

                            param.Add(new RadioGpsParam()
                            {
                                Operate = op.Type,
                                Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
                                Target = trgt.Radio.RadioID,
                                Cycle = op.Cycle
                            });
                        }

                    }
                }
            }
            catch (Exception e)
            {
                DataBase.InsertLog("BuildGpsParam:" + e.Message);
            }

            return param;
        }

        private List<object> BuildControlParam(CControl op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();
            List<int> groupid = new List<int>();
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
                            if (trgt.Group == null || trgt.Group.ID <= 0) continue;
                            groupid.Add(trgt.Group.ID);

                            var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == trgt.Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
                            foreach (var item in radio)
                            {
                                param.Add(new RadioControlsParam()
                                {
                                    Type = op.Type,
                                    Target = item.Value.Radio.ID,
                                });
                            }
                        }
                    }
                    foreach (CMember trgt in target.Target)
                    {
                        if (trgt.Type != MemberType.Group)
                        {
                            if (trgt.Radio == null || trgt.Radio.ID <= 0) continue;
                            if (trgt.Group != null || trgt.Group.ID > 0)
                            {
                                if (groupid.Contains(trgt.Group.ID)) continue;
                            }
                            param.Add(new RadioControlsParam()
                            {
                                Type = op.Type,
                                Target = trgt.Radio.RadioID,
                            });
                        }

                    }
                }
            }
            catch (Exception e)
            {
                DataBase.InsertLog("BuildControlParam:" + e.Message);
            }

            return param;
        }


        public static void GetStatus(long tpye)
        {
            new Thread(new ThreadStart(delegate() {
                TServer.Call(JsonConvert.SerializeObject(new TServerRequest()
                {
                    call = RequestType.status.ToString(),
                    type = "radio",
                    callId = TServer.CallId,
                    param = new RadioStatusParam()
                    {
                        getType = tpye,
                    },
                }));                       
            })).Start(); 

        }
    }
}
