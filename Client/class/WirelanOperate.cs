using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    public class WirelanCallParam
    {
        public ExecType operate;
        public TargetType type;
        public long source;
        public long target;
        public bool isCurrent;
    }

    public class WirelanCallStatusParam
    {
        public int status;
        public ExecType operate;
        public TargetType type;
        public long source;
        public long target;
        public bool isCurrent;
    }

    public class WirelanPlayParam
    {
        public long target;
    }

    public class WirelanPlayStatusParam
    {
        public long target;
        public int status;
    }

    public class WirelanOperate
    {
        public RequestType Call;
        private long CallId;
        public List<object> Param;

        public static WirelanCallParam LastCall;

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
                    type = "wl",
                    callId = CallId.ToString(),
                };
            }
            else
            {
                return new TServerRequestStr()
                {
                    call = Call.ToString(),
                    type = "wl",
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
                        json.Add(JsonConvert.SerializeObject(this.ToStr(obj)) + "\r\n");
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

        public WirelanOperate(COperate operate, long PN)
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
                        Call = RequestType.wlCall;
                        Param = BuildCallParam(operate.Operate as CDispatch, targettype, operate.Target);
                    }
                }
                //else if (operate.Type == OPType.ShortMessage)
                //{
                //    TargetType targettype = GetTargetType(operate.Target);
                //    if (TargetType.None != targettype)
                //    {
                //        Call = RequestType.message;
                //        Param = BuildSmsParam(operate.Operate as CShortMessage, targettype, operate.Target);
                //    }
                //}
                //else if (operate.Type == OPType.Position)
                //{
                //    TargetType targettype = GetTargetType(operate.Target);
                //    if (TargetType.None != targettype && TargetType.All != targettype)
                //    {
                //        Call = RequestType.queryGps;
                //        Param = BuildGpsParam(operate.Operate as CPosition, targettype, operate.Target);
                //    }
                //}
                //else if (operate.Type == OPType.Control)
                //{
                //    TargetType targettype = GetTargetType(operate.Target);
                //    if (TargetType.None != targettype && TargetType.All != targettype)
                //    {
                //        Call = RequestType.control;
                //        Param = BuildControlParam(operate.Operate as CControl, targettype, operate.Target);
                //    }
                //}
            }
            catch
            {

            }

            CallId = PN;
        }

        private List<object> BuildCallParam(CDispatch op, TargetType type, CMultMember target)
        {
            List<object> param = new List<object>();


            if (op == null) return param;

            switch (type)
            {
                case TargetType.Private:
                    param.Add(new WirelanCallParam()
                    {
                        operate = op.Exec,
                        type = TargetType.Private,
                        target = target.Target[0].Radio.RadioID
                    });
                    break;
                case TargetType.Group:
                    param.Add(new WirelanCallParam()
                    {
                        operate = op.Exec,
                        type = TargetType.Group,
                        target = target.Target[0].Group.GroupID
                    }); break;
                case TargetType.All:
                    param.Add(new WirelanCallParam()
                    {
                        operate = op.Exec,
                        type = TargetType.All,
                    }); break;
                default:
                    break;
            }

            try
            {
                LastCall = param[0] as WirelanCallParam;
            }
            catch (Exception e)
            {
                DataBase.InsertLog("BuildCallParam:" + e.Message);
            }

            return param;
        }

        //private List<object> BuildSmsParam(CShortMessage op, TargetType type, CMultMember target)
        //{
        //    List<object> param = new List<object>();

        //    switch (type)
        //    {

        //        case TargetType.Private:
        //            param.Add(new RadioSmsParam()
        //            {
        //                Type = TargetType.Private,
        //                Target = target.Target[0].Radio.RadioID,
        //                Contents = op.Message
        //            });
        //            break;
        //        case TargetType.Group:
        //            param.Add(new RadioSmsParam()
        //            {
        //                Type = TargetType.Group,
        //                Target = target.Target[0].Group.GroupID,
        //                Contents = op.Message
        //            });
        //            break;
        //        case TargetType.All:

        //            var group = TargetMgr.TargetList.Group.Where(p => p.Value.Group != null && p.Value.Group.ID > 0 && p.Value.Group.GroupID > 0);

        //            foreach (var item in group)
        //            {
        //                param.Add(new RadioSmsParam()
        //                {
        //                    Type = TargetType.Group,
        //                    Target = item.Value.Group.GroupID,
        //                    Contents = op.Message
        //                });
        //            }

        //            var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID <= 0 && p.Value.Radio != null && p.Value.Radio.RadioID > 0);

        //            foreach (var item in radio)
        //            {
        //                param.Add(new RadioSmsParam()
        //                {
        //                    Type = TargetType.Private,
        //                    Target = item.Value.Radio.RadioID,
        //                    Contents = op.Message
        //                });
        //            }
        //            break;
        //        default:
        //            break;
        //    }

        //    return param;
        //}

        //private List<object> BuildGpsParam(CPosition op, TargetType type, CMultMember target)
        //{
        //    List<object> param = new List<object>();

        //    try
        //    {
        //        switch (type)
        //        {
        //            case TargetType.Private:
        //                param.Add(new RadioGpsParam()
        //                {
        //                    Operate = op.Type,
        //                    Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
        //                    Target = target.Target[0].Radio.RadioID,
        //                    Cycle = op.Cycle
        //                });

        //                if (op.IsCycle && op.Type == ExecType.Start)
        //                    TargetMgr.TargetList.Radio[target.Target[0].Radio.ID].Radio.IsGPS = true;
        //                if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[target.Target[0].Radio.ID].Radio.IsGPS = false;
        //                break;
        //            case TargetType.Group:
        //                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == target.Target[0].Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
        //                foreach (var item in radio)
        //                {
        //                    param.Add(new RadioGpsParam()
        //                    {
        //                        Operate = ExecType.Start,
        //                        Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
        //                        Target = item.Value.Radio.RadioID,
        //                        Cycle = op.Cycle
        //                    });

        //                    if (op.IsCycle && op.Type == ExecType.Start)
        //                        TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = true;
        //                    if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = false;
        //                }
        //                break;
        //            case TargetType.All:

        //                var group = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
        //                foreach (var item in group)
        //                {
        //                    param.Add(new RadioGpsParam()
        //                    {
        //                        Operate = ExecType.Start,
        //                        Type = op.IsCycle ? (op.IsCSBK ? (op.IsEnh ? QueryGPSType.EnhCycyle : QueryGPSType.CSBKCycle) : QueryGPSType.GenericCycle) : (op.IsCSBK ? (op.IsEnh ? QueryGPSType.Enh : QueryGPSType.CSBK) : QueryGPSType.Generic),
        //                        Target = item.Value.Radio.RadioID,
        //                        Cycle = op.Cycle
        //                    });

        //                    if (op.IsCycle && op.Type == ExecType.Start)
        //                        TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = true;
        //                    if (op.Type == ExecType.Start) TargetMgr.TargetList.Radio[item.Value.Radio.ID].Radio.IsGPS = false;
        //                }
        //                break;
        //            default:
        //                break;
        //        }
        //    }
        //    catch { }

        //    return param;
        //}

        //private List<object> BuildControlParam(CControl op, TargetType type, CMultMember target)
        //{
        //    List<object> param = new List<object>();

        //    try
        //    {
        //        switch (type)
        //        {
        //            case TargetType.Private:
        //                param.Add(new RadioControlsParam()
        //                {
        //                    Type = op.Type,
        //                    Target = target.Target[0].Radio.RadioID,
        //                });
        //                break;
        //            case TargetType.Group:
        //                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Group.ID == target.Target[0].Group.ID && p.Value.Radio != null && p.Value.Radio.RadioID > 0);
        //                foreach (var item in radio)
        //                {
        //                    param.Add(new RadioControlsParam()
        //                    {
        //                        Type = op.Type,
        //                        Target = item.Value.Radio.ID,
        //                    });
        //                }
        //                break;
        //            case TargetType.All:

        //                var group = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio != null && p.Value.Radio.RadioID > 0);
        //                foreach (var item in group)
        //                {
        //                    param.Add(new RadioControlsParam()
        //                    {
        //                        Type = op.Type,
        //                        Target = item.Value.Radio.ID,
        //                    });
        //                }
        //                break;
        //            default:
        //                break;
        //        }
        //    }
        //    catch { }

        //    return param;
        //}


        //private RequestType GetRequest(COperate op)
        //{

        //    return RequestType.None;
        //}

        //private static GPSParam GPSPrase(object rep)
        //{
        //    try
        //    {
        //        return JsonConvert.DeserializeObject<GPSParam>(JsonConvert.SerializeObject(((TServerResponse)rep).contents));
        //    }
        //    catch
        //    {
        //        return null;
        //    }
        //}

        //private static object ControlParse(object rep)
        //{
        //    try
        //    {
        //        if (((TServerResponse)rep).status == "success") return true;
        //        else return false;
        //    }
        //    catch
        //    {
        //        return false;
        //    }
        //}

        public static void GetStatus(long tpye)
        {
            TServer.Call(JsonConvert.SerializeObject(new TServerRequest()
            {
                call = RequestType.wlInfo.ToString(),
                type = "wl",
                callId = TServer.CallId,
                param = new RadioStatusParam()
                {
                    getType = tpye,
                },
            }));
        }

        public static void wlPlay(long target)
        {
            TServer.Call(JsonConvert.SerializeObject(new TServerRequest()
            {
                call = RequestType.wlPlay.ToString(),
                type = "wl",
                callId = TServer.CallId,
                param = new WirelanPlayParam()
                {
                    target = target,
                },
            }));
        }
    }
}
