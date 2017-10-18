using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Threading;

using Sigmar.Logger;
using Sigmar.Extension;
using Dispatcher.ViewsModules;

namespace Dispatcher.Service
{
    public class CDispatcher
    {

        private RequestType _type = RequestType.radio;

        public CDispatcher(RequestType type)
        {
            _type = type;
            CTServer.Instance().OnReceiveRequest += new CTServer.ReceiveRequestHandele(ReceiveRequest);
            CTServer.Instance().Timeout += new EventHandler(Timeout);
        }

        private void ReceiveRequest(RequestOpcode call, RequestType type, object param)
        {
            string parameter = param == null ? null : JsonConvert.SerializeObject(param);
            switch (call)
            {
                case RequestOpcode.status:
                case RequestOpcode.wlInfo:
                    OnStatusUpdate(parameter);
                    break;
                case RequestOpcode.callStatus:
                case RequestOpcode.wlCallStatus:
                    OnCallResponse(parameter);
                    break;
                case RequestOpcode.messageStatus:
                    OnShortMessageResponse(parameter);
                    break;
                case RequestOpcode.message:
                    OnShortMessageRequest(parameter);
                    break;
                case RequestOpcode.sendArs:
                    OnArsRequest(parameter);
                    break;
                case RequestOpcode.controlStatus:
                    OnControlResponse(parameter);
                    break;
                case RequestOpcode.queryGpsStatus:
                case RequestOpcode.sendGpsStatus:
                    OnLocationQueryResponse(parameter);
                    break;
                case RequestOpcode.sendGps:
                    OnLocationReport(parameter);
                    break;
                case RequestOpcode.locationIndoor:
                    OnLocationInDoorReport(parameter);
                    break;
                default:
                    CustomRequest(call,  parameter);
                    break;

            }
        }

        private void Timeout(object sender, EventArgs e)
        {

        }
        
        public void GetStatus()
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.status : RequestOpcode.wlInfo;
            var param = new StatusParameter() { getType = (long)StatusType_t.ConnectStatus };
            Request(opcode, param);
        }

        public void GetOnlineList()
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.status : RequestOpcode.wlInfo;
            var param = new StatusParameter() { getType = (long)StatusType_t.OnLineList };
            Request(opcode, param);
        }

        private void OnStatusUpdate(string parameter)
        {
            if (parameter == null || parameter == "") return;
            StatusParameter _status = null;

            try
            {
                _status = JsonConvert.DeserializeObject<StatusParameter>(parameter);
            }
            catch
            {
                //WARNING("Parse  RadioStatusParam Error");
                return;
            }

            if (_status == null) return;

            try
            {
                if ((_status.getType & (long)StatusType_t.ConnectStatus) != 0) OnConnectStatus((_status.info as string).ToLong());
                if ((_status.getType & (long)StatusType_t.OnLineList) != 0) OnOnLineList(JsonConvert.DeserializeObject<List<OnLineStatus_t>>(JsonConvert.SerializeObject(_status.info)));
            }
            catch(Exception ex)
            {
                //WARNING(ex);
            }
        }

        private void OnConnectStatus(long statusvalue)
        {
            if (RunAccess.Mode == RunAccess.Mode_t.CPC || RunAccess.Mode == RunAccess.Mode_t.IPSC || RunAccess.Mode == RunAccess.Mode_t.LCP)
            {
                ServerStatus.Instance().Repeater.SetStatus((statusvalue & 1) == 0);
                ServerStatus.Instance().Mnis.SetStatus((statusvalue & 2) == 0);
            }
            else if (RunAccess.Mode == RunAccess.Mode_t.VehicleStation || RunAccess.Mode == RunAccess.Mode_t.VehicleStationWithMnis)
            {
                ServerStatus.Instance().Mnis.SetStatus((statusvalue & 1) == 0);
                ServerStatus.Instance().VehicleStation.SetStatus((statusvalue & 2) == 0);
            }
            else
            {

            }

        }

        private void OnOnLineList( List<OnLineStatus_t> status)
        {
            foreach (OnLineStatus_t rad in status)
            {
                VMTarget Target = ResourcesMgr.Instance().Members.Find(p => p.RadioID == rad.radioId);
                if (Target == null)
                {
                    Target = ResourcesMgr.Instance().CreateTarget(rad.radioId);
                }
                
                Target.ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.OnlineStatus, rad.IsOnline));
                Target.ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.LocationStatus, rad.IsInGps ? LocationStatus_t.Cycle : LocationStatus_t.Idle));
                Target.ChangeValue.Execute(new TargetStatusChangedEventArgs(ChangedKey_t.LocationInDoorStatus, rad.IsInLocationIndoor ? LocationInDoorStatus_t.Cycle : LocationInDoorStatus_t.Idle));
            }
        }

        public event CallRequestHandler CallRequest;
        public event CallResponseHandler CallResponse;
        public void AllCall()
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.status : RequestOpcode.wlInfo;
            var param = GetCallParameter(ExecType_t.Start, TargetMode_t.All, 0);
            Request(opcode, param);       
        }

        public void GroupCall(int target)
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.call : RequestOpcode.wlCall;
            var param = GetCallParameter(ExecType_t.Start, TargetMode_t.Group, target);
            Request(opcode, param);  
        }

        public void PrivateCall(int target)
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.call : RequestOpcode.wlCall;
            var param = GetCallParameter(ExecType_t.Start, TargetMode_t.Private, target);
            Request(opcode, param);  
        }

        public void StopCall(int target)
        {
            RequestOpcode opcode = _type == RequestType.radio ? RequestOpcode.call : RequestOpcode.wlCall;
            var param = GetCallParameter(ExecType_t.Stop, TargetMode_t.Private, target);
            Request(opcode, param);  
        }

        public virtual void PlayVoice(int target)
        {

        }

        public void OnCallRequest(CallRequestArgs e)
        {
            if (CallRequest != null) CallRequest(e);
        }

        private void OnCallResponse(string parameter)
        {
            if (CallResponse != null)
            {
                CallResponseArgs args = CreateCallResponse(parameter);
                if(args != null)CallResponse(args);
            }
        }

        public virtual object GetCallParameter(ExecType_t exec, TargetMode_t type, int target)
        {
            return new CallParameter()
            {
                Operate = exec,
                Type = type,
                Target = target
            };
        }

        public virtual CallResponseArgs CreateCallResponse(string parameter)
        {
            if (parameter == null || parameter == "") return null;
            try
            {
                CallResponse res = JsonConvert.DeserializeObject<CallResponse>(parameter);
                if(res != null)
                {
                    return new CallResponseArgs(res.Operate, res.Status)
                    {
                        Type = res.Type,
                        Target = res.Target
                    };
                }
                return null;

            }
            catch(Exception  ex)
            {
                //WARNING(ex);
                return null;
            }
        }

        public event ShortMessageRequestHandler ShortMessageRequest;
        public event ShortMessageResponseHandler ShortMessageResponse;

        public void SendShortMessage(int target, string contents)
        {
            RequestOpcode opcode = RequestOpcode.message;
            var param = new ShortMessageParameter(target, contents);
            Request(opcode, param);
        }

        private void OnShortMessageResponse(string parameter)
        {
            if (parameter == null || parameter == "") return;
            ShortMessageResponse _response = null;

            try
            {
                _response = JsonConvert.DeserializeObject<ShortMessageResponse>(parameter);
            }
            catch
            {
                //WARNING("Parse  ShortMessageResponse Error");
                return;
            }

            if (_response == null) return;

            if (ShortMessageResponse != null) ShortMessageResponse(new ShortMessageResponseArgs(_response.Status, _response.Target, _response.Contents));
        }

        private void OnShortMessageRequest(string parameter)
        {
            if (parameter == null || parameter == "") return;
            ShortMessageParameter _request = null;

            try
            {
                _request = JsonConvert.DeserializeObject<ShortMessageParameter>(parameter);
            }
            catch
            {
                //WARNING("Parse  ShortMessageParameter Error");
                return;
            }

            if (_request == null) return;

            if (ShortMessageRequest != null) ShortMessageRequest(new ShortMessageRequestArgs(_request.Source,_request.Contents));
        }

        public event ArsRequestHandler ArsRequest;
        
        private void OnArsRequest(string parameter)
        {
            if (parameter == null || parameter == "") return;
            ArsParameter _request = null;

            try
            {
                _request = JsonConvert.DeserializeObject<ArsParameter>(parameter);
            }
            catch
            {
                //WARNING("Parse  ArsParameter Error");
                return;
            }

            if (_request == null) return;

            if (ArsRequest != null) ArsRequest(new ArsRequestArgs(_request.Target, _request.ISOnline));
        }


        public event ControlResponseHandler ControlResponse;
        public virtual void Control(int target, ControlerType_t type)
        { 
            RequestOpcode opcode = RequestOpcode.control;
            var param = new ControlParameter(target, type);
            Request(opcode, param);       
        }

        private void OnControlResponse(string parameter)
        {
            if (parameter == null || parameter == "") return;
            ControlResponse _response = null;

            try
            {
                _response = JsonConvert.DeserializeObject<ControlResponse>(parameter);
            }
            catch
            {
                //WARNING("Parse  ControlResponse Error");
                return;
            }

            if (_response == null) return;

            if (ControlResponse != null) ControlResponse(new ControlResponseArgs(_response.Type, _response.Target, _response.Status));
        }


        private void OnLocationQueryResponse(string parameter)
        {
            if (parameter == null || parameter == "") return;
            LocationResponse _response = null;

            try
            {
                _response = JsonConvert.DeserializeObject<LocationResponse>(parameter);
            }
            catch
            {
                //WARNING("Parse  LocationResponse Error");
                return;
            }

            if (_response == null) return;

            if (_response.Type == QueryLocationType_t.LocationInDoor)
            {
                OnLocationInDoorResponse(_response);
            }
            else
            {
                OnLocationResponse(_response);
            }
        }


        public event LocationResponseHandler LocationResponse;
        public event LocationReportHandler LocationReport;
       
        public void Location(int target, LocationArgs args)
        {
            if (args == null) return;
            QueryLocationType_t querytype = QueryLocationType_t.Generic;
            switch (args.Type)
            {
                case LocationType_t.Query:
                    querytype = QueryLocationType_t.Generic;
                    break;
                case LocationType_t.Cycle:
                    querytype = QueryLocationType_t.GenericCycle;
                    break;
                case LocationType_t.CsbkQuery:
                    querytype = QueryLocationType_t.CSBK;
                    break;
                case LocationType_t.CsbkCycle:
                    querytype = QueryLocationType_t.CSBKCycle;
                    break;
                case LocationType_t.EnhCsbkQuery:
                    querytype = QueryLocationType_t.Enh;
                    break;
                case LocationType_t.EnhCsbkCycle:
                    querytype = QueryLocationType_t.EnhCycle;
                    break;
                case  LocationType_t.StopCycle:
                    StopCycle(target);
                    return;
                default:
                    return;
            }
            
           
            RequestOpcode opcode = RequestOpcode.queryGps;
            var param = new LocationParameter(){ 
                Operate = ExecType_t.Start,
                Type = querytype,
                Target = target,
                Cycle = args.Cycle
            };

            Request(opcode, param);
        }

        private void StopCycle(int target)
        {          
            RequestOpcode opcode = RequestOpcode.queryGps;
            var param = new LocationParameter()
            {
                Operate = ExecType_t.Stop,
                Type = QueryLocationType_t.GenericCycle,
                Target = target,
                Cycle = 0
            };

            Request(opcode, param);
        }


        private void OnLocationReport(string parameter)
        {
            if (parameter == null || parameter == "") return;
            LocationReport _report = null;

            try
            {
                _report = JsonConvert.DeserializeObject<LocationReport>(parameter);
            }
            catch
            {
                //WARNING("Parse  LocationReport Error");
                return;
            }

            if (_report == null) return;
            if (LocationReport != null) LocationReport(new LocationReportArgs(_report.Source, _report.Gps));
        }


        private void OnLocationInDoorResponse(LocationResponse res)
        {
            if (LocationInDoorResponse != null)
            {
                LocationInDoorResponse(new LocationInDoorResponseArgs(res.Operate, res.Target, res.Status));
            }
        }



        public event LocationInDoorResponseHandler LocationInDoorResponse;
        public event LocationInDoorReportHandler LocationInDoorReport;

        private void OnLocationInDoorReport(string parameter)
        {
            if (parameter == null || parameter == "") return;
            LocationInDoorReport_t _report = null;

            try
            {
                _report = JsonConvert.DeserializeObject<LocationInDoorReport_t>(parameter);
            }
            catch(Exception ex)
            {
                //WARNING("Parse  LocationReport Error",ex);
                return;
            }

            if (_report == null) return;
            if (LocationInDoorReport != null) LocationInDoorReport(new LocationInDoorReportArgs(_report.source, new List<BeaconReport>(){_report.bcon}));
        }
        private void OnLocationResponse(LocationResponse res)
        {
            if(LocationResponse != null)
            {
                LocationResponseArgs args = new LocationResponseArgs(res.Operate, res.Type, res.Status)
                {
                    Target = res.Target,
                    Cycle = res.Cycle
                };
                LocationResponse(args);
            }
        }

        public void LocationInDoor(int target, LocationInDoorArgs args)
        {
            RequestOpcode opcode = RequestOpcode.queryGps;
            var param = new LocationParameter()
            {
                Operate = args.Type == LocationInDoorType_t.Start ? ExecType_t.Start : ExecType_t.Stop,
                Type = QueryLocationType_t.LocationInDoor,
                Target = target,
                Cycle = 0
            };

            Request(opcode, param);
        }
     
        public void Request(RequestOpcode opcode, object parameter)
        {
            if (!CTServer.Instance().IsInitialized) return;
            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    string[] reply = CTServer.Instance().Request(opcode, _type, parameter);

                    if (reply != null && reply.Length >=2)
                    {
                        if (reply[0] == "success")
                        {

                        }
                        else
                        {
                            Log.Message("发起操作失败.");
                        }

                        CustomReply(opcode, reply[0] == "success", reply[1]);
                    }
                }
                catch
                {

                }
            })).Start();
        }
    
        public virtual void CustomReply(RequestOpcode opcode,bool success, string reply)
        {

        }

        public virtual void CustomRequest(RequestOpcode call, string param)
        {

        }
    }

    public class StatusParameter
    {
        public long getType;
        public object info;
    }
    public enum StatusType_t
    {
        ConnectStatus = 1,
        OnLineList = 2,
    }

    public class OnLineStatus_t
    {
        public int radioId;
        public bool IsInGps;
        public bool IsInLocationIndoor;
        public bool IsOnline;
    }

    public class CallParameter
    {
        public ExecType_t Operate;
        public TargetMode_t Type;
        public int Target;
    }

    public enum ExecType_t
    {
        Start = 0,
        Stop = 1,
    }

    public enum TargetMode_t
    {
        All = 1,
        Group = 2,
        Private = 3,
    }

    public class CallResponse
    {
        public ExecType_t Operate;
        public TargetMode_t Type;
        public int Target;
        public int Status;
    }

    public enum OperationStatus_t
    {
        Success = 0,
        Failure = 1,
    }

    public class ShortMessageParameter
    {
        public TargetMode_t Type;
        public int Target;
        public int Source;
        public string Contents;

        public ShortMessageParameter(int target, string contents)
        {
            Type = TargetMode_t.Private;
            Source = 0;

            Target = target;
            Contents = contents;
        }

        public ShortMessageParameter()
        {

        }
    }

    public class ShortMessageResponse
    {
        public TargetMode_t Type;
        public int Target;
        public int Source;
        public string Contents;
        public int Status;
    }

    public class ArsParameter
    {
        public int Target;
        public bool ISOnline;
    }

    public class ControlParameter
    {
        public ControlerType_t Type;
        public int Target;

        public ControlParameter()
        {

        }

        public ControlParameter(int target, ControlerType_t type)
        {
            Type = type;
            Target = target;
        }
    }

    public class ControlResponse
    {
        public ControlerType_t Type;
        public int Target;
        public int Status;
    }

    public enum ControlResponsetatus_t
    {
        Online = 0,
        Offline = 1,
        Failure = 2,
    }

    public enum QueryLocationType_t
    {
        Generic = 12,
        GenericCycle = 13,
        CSBK = 14,
        CSBKCycle = 15,
        Enh = 23,
        EnhCycle = 24,
        LocationInDoor = 25,
    }

    public class LocationParameter
    {
        public ExecType_t Operate;
        public QueryLocationType_t Type;
        public int Target;
        public double Cycle;
    }

    public class LocationResponse
    {
        public ExecType_t Operate;
        public QueryLocationType_t Type;
        public int Target;
        public double Cycle;
        public int Status;
    }

    public class GpsReport
    {
        public bool Valid;
        public double Lon;
        public double Lat;
        public double Alt;
        public double Speed;

    }
    public class LocationReport
    {
        public int Source;
        public GpsReport Gps;
    }

    public class BeaconReport
    {
          public byte[] uuid;
          public int timestamp;
          public int major;
          public int minor;
          public int txpower;
          public int rssi;
    }

    public class LocationInDoorReport_t
    {
        public int source;
        public BeaconReport bcon;
    }
}
