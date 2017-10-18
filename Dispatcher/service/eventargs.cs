using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Dispatcher.Modules;
using Dispatcher.ViewsModules;

namespace Dispatcher.Service
{

    public delegate void OperatedEventHandler(OperatedEventArgs e);
    public class OperatedEventArgs
    {
        public OperateType_t Operate;
        public object parameter;
        public OperatedEventArgs(OperateType_t operate, object param = null)
        {
            Operate = operate;
            parameter = param;
        }
    }

    public class TargetStatusChangedEventArgs
    {
        public ChangedKey_t Key;
        public object OldValue;
        public object NewValue;
        public TargetStatusChangedEventArgs(ChangedKey_t key, object oldval, object newval)
        {
            Key = key;
            OldValue = oldval;
            NewValue = newval;
        }

        public TargetStatusChangedEventArgs(ChangedKey_t key, object newval)
        {
            Key = key;
            OldValue = "unknown";
            NewValue = newval;
        }

        public TargetStatusChangedEventArgs(object newval)
        {
            Key = ChangedKey_t.All;
            OldValue = "unknown";
            NewValue = newval;
        }
    }

    //public class ServerStatusChangedEventArgs
    //{
    //    public ServerStatus.ServerStatusType_t Key;

    //    public ServerStatus.ServerStatus_t OldValue;
    //    public ServerStatus.ServerStatus_t NewValue;
    //    public ServerStatusChangedEventArgs(ServerStatus.ServerStatusType_t key, ServerStatus.ServerStatus_t oldval, ServerStatus.ServerStatus_t newval)
    //    {
    //        Key = key;
    //        OldValue = oldval;
    //        NewValue = newval;
    //    }

    //    public ServerStatusChangedEventArgs(ServerStatus.ServerStatusType_t key, ServerStatus.ServerStatus_t newval)
    //    {
    //        Key = key;
    //        NewValue = newval;
    //    }

    //    public ServerStatusChangedEventArgs(ServerStatus.ServerStatusType_t key, bool isconnected)
    //    {
    //        Key = key;
    //        NewValue = new ServerStatus.ServerStatus_t(isconnected);
    //    }

    //    public ServerStatusChangedEventArgs(ServerStatus.ServerStatusType_t key, string host, int port)
    //    {
    //        Key = key;
    //        NewValue = new ServerStatus.ServerStatus_t(host, port);
    //    }
    //}


    public enum ViewType_t
    {
        IconView,
        PanelView,
    }

    public delegate void ViewChangedEventHandler(ViewChangedArgs e);
    public class ViewChangedArgs
    {
        public ViewType_t ViewKey;
        public ViewChangedArgs(ViewType_t key)
        {
            ViewKey = key;
        }
    }

    public interface OperationAgrs
    {
        bool Equal(OperationAgrs dest);
    }
    public class CallArgs : OperationAgrs
    {
        public CallOperatedType_t Type;
        public CallArgs() { }
        public CallArgs(CallOperatedType_t type)
        {
            Type = type;
        }

        
        public bool Equal(OperationAgrs dest)
        {
            CallArgs d = dest as CallArgs;
            if(d == null)return false;
            if(d.Type != Type)return false;
            return true;
        }
    }
    public class ShortMessageArgs : OperationAgrs
    {
        public string Contents{get; set;}

        public ShortMessageArgs() { }
        public ShortMessageArgs(string contents)
        {
            Contents = contents;
        }
        public bool Equal(OperationAgrs dest)
        {
            ShortMessageArgs d = dest as ShortMessageArgs;
            if (d == null) return false;
            if (d.Contents != Contents) return false;
            return true;
        }
    }

    public class ControlArgs : OperationAgrs
    {
        public ControlerType_t Type;       
        public ControlArgs() { }
        public ControlArgs(ControlerType_t type)
        {
            Type = type;
        }

        public bool Equal(OperationAgrs dest)
        {
            ControlArgs d = dest as ControlArgs;
            if (d == null) return false;
            if (d.Type != Type) return false;
            return true;
        }
    }

    public class LocationArgs : OperationAgrs
    {
        public LocationType_t Type;      
        public double Cycle { get; set; }

        public LocationArgs() { }
        public LocationArgs(LocationType_t type, double cycle = 0)
        {
            Type = type;
            Cycle = cycle;
        }

        public bool Equal(OperationAgrs dest)
        {
            LocationArgs d = dest as LocationArgs;
            if (d == null) return false;
            if (d.Type != Type) return false;
            if (d.Cycle != Cycle) return false;
            return true;
        }
    }

    public class LocationInDoorArgs : OperationAgrs
    {
        public LocationInDoorType_t Type;
     
        public LocationInDoorArgs() { }
        public LocationInDoorArgs(LocationInDoorType_t type)
        {
            Type = type;
        }


        public bool Equal(OperationAgrs dest)
        {
            LocationInDoorArgs d = dest as LocationInDoorArgs;
            if (d == null) return false;
            if (d.Type != Type) return false;
            return true;
        }
    }

    public class LocationInDoorReports
    {
        public VMBeacon Beacon;
        public VMTarget Source;

        public LocationInDoorReports(VMTarget source, VMBeacon beacon)
        {
            Source = source;
            Beacon = beacon;
        }
    }


    public delegate void LoginResultHandler(LoginResultArgs e);
    public class LoginResultArgs
    {
        public bool  IsSuccess;

        public LoginResultArgs(bool issuccess)
        {
            IsSuccess = issuccess;
        }
    }


    public delegate void CallRequestHandler(CallRequestArgs e);
    public class CallRequestArgs
    {
        public ExecType_t Opcode;
        public TargetMode_t Type;
        public int Source;
        public int Target;
        public bool IsCurrent;

        public CallRequestArgs(ExecType_t type)
        {
            Opcode = type;
        }
    }

    public delegate void CallResponseHandler(CallResponseArgs e);
    public class CallResponseArgs
    {
        public OperationStatus_t Status;
        public ExecType_t Opcode;
        public TargetMode_t Type;
        public int Source;
        public int Target;
        public bool IsCurrent;

        public CallResponseArgs(ExecType_t type , OperationStatus_t status)
        {
            Status = status;
            Opcode = type;
        }

        public CallResponseArgs(ExecType_t type, int status)
        {
            Status = (OperationStatus_t)status;
            Opcode = type;
        }
    }


    public delegate void ShortMessageResponseHandler(ShortMessageResponseArgs e);
    public class ShortMessageResponseArgs
    {
        public OperationStatus_t Status;
        public int Target;
        public string Contents;

        public ShortMessageResponseArgs(OperationStatus_t status, int target, string contents = "")
        {
            Status = status;
            Target = target;
            Contents = contents;
        }

        public ShortMessageResponseArgs(int status, int target, string contents = "")
        {
            Status = (OperationStatus_t)status;
            Target = target;
            Contents = contents;
        }
    }

    public delegate void ShortMessageRequestHandler(ShortMessageRequestArgs e);
    public class ShortMessageRequestArgs
    {
        public int Source;
        public string Contents;

        public ShortMessageRequestArgs(int source, string contents = "")
        {
            Source = source;
            Contents = contents;
        }
    }

    public delegate void ArsRequestHandler(ArsRequestArgs e);
    public class ArsRequestArgs
    {
        public int Source;
        public bool IsOnline;

        public ArsRequestArgs(int source, bool isonline)
        {
            Source = source;
            IsOnline = isonline;
        }
    }

    public delegate void ControlResponseHandler(ControlResponseArgs e);
    public class ControlResponseArgs
    {
        public ControlerType_t Type;
        public int Target;
        public ControlResponsetatus_t Status;


        public ControlResponseArgs(ControlerType_t type, int target, ControlResponsetatus_t status)
        {
            Type = type;
            Target = target;
            Status = status;
        }
        public ControlResponseArgs(ControlerType_t type, int target, int status)
        {
            Type = type;
            Target = target;
            Status = (ControlResponsetatus_t)status;
        }
    }

    public delegate void LocationResponseHandler(LocationResponseArgs e);
    public class LocationResponseArgs
    {
        public ExecType_t Opcode;
        public LocationType_t Type;
        public int Target;
        public double Cycle;
        public OperationStatus_t Status;

        public LocationResponseArgs(ExecType_t opcode, QueryLocationType_t type, OperationStatus_t status)
        {
            Type = PraseType(type);
            Opcode = opcode;
            Status = status;
        }
        public LocationResponseArgs(ExecType_t opcode, QueryLocationType_t type, int status)
        {
            Type = PraseType(type);
            Opcode = opcode;
            Status = (OperationStatus_t)status;
        }

        private LocationType_t PraseType(QueryLocationType_t type)
        {
            switch (type)
            {
                case QueryLocationType_t.Generic:return LocationType_t.Query;
                case QueryLocationType_t.GenericCycle:return LocationType_t.Cycle;
                case QueryLocationType_t.CSBK:return LocationType_t.CsbkQuery;
                case QueryLocationType_t.CSBKCycle: return LocationType_t.CsbkCycle;
                case QueryLocationType_t.Enh: return LocationType_t.EnhCsbkQuery;
                case QueryLocationType_t.EnhCycle: return LocationType_t.EnhCsbkCycle;             
            }
            return  LocationType_t.StopCycle;
        }
    }


    public delegate void LocationReportHandler(LocationReportArgs e);
    public class LocationReportArgs
    {
        public GpsReport Report;
        public int Source;

        public LocationReportArgs(int source,GpsReport report)
        {
            Report = report;
            Source = source; 
        }     
    }

    public class DrawLocationReportArgs
    {
        public VMTarget Source;
        public GpsReport Report;


        public DrawLocationReportArgs(VMTarget source, GpsReport report)
        {
            Report = report;
            Source = source;
        }
    }


    public delegate void LocationInDoorResponseHandler(LocationInDoorResponseArgs e);
    public class LocationInDoorResponseArgs
    {
        public ExecType_t Opcode;
        public int Target;
        public OperationStatus_t Status;

        public LocationInDoorResponseArgs(ExecType_t opcode, int target, OperationStatus_t status)
        {
            Target = target;
            Opcode = opcode;
            Status = status;
        }
        public LocationInDoorResponseArgs(ExecType_t opcode, int target, int status)
        {
            Target = target;
            Opcode = opcode;
            Status = (OperationStatus_t)status;
        }
    }

    public delegate void LocationInDoorReportHandler(LocationInDoorReportArgs e);

    public class LocationInDoorReportArgs
    {
        public List<BeaconReport> Report;
        public int Source;

        public LocationInDoorReportArgs(int source, List<BeaconReport> report)
        {
            Report = report;
            Source = source; 
        }         
    }
}
