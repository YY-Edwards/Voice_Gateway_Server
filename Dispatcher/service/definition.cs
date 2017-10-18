using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Dispatcher.Modules;

using System.ComponentModel;

namespace Dispatcher.Service
{
    public enum OperateType_t
    {
        //system operate
        WindowMove,
        WindowMinimize,
        WindowMaximize,
        WindowRestore,
        WindowClose,

        //MenuFile
        SaveWorkspace,
        ExportWorkspace,
        Print,

        //MenuView 
        OpenTools,
        CloseTools,
        OpenResource,
        CloseResource,
        OpenNavigation,
        CloseNavigation,
        OpenEvent,
        CloseEvent,
        OpenNotify,
        CloseNotify,
        CloseAllNotify,

        SetCurrentTarget,       
        //Tools
        NavigationWork,
        NewFast,
  
        //status
        ConnectTServer,
        ConnectLogServer,

        //OpenWindow
        OpenNewOperateWindow,
        OpenHelpWindow,
        OpenNewFastWindow,
        OpenSaveWarningWindow,
        OpenOperateWindow,

        AddNotify,
        DrawLocationPoint,
    }


    public enum RemoteOperationType_t
    {
        //log server
        add,
        list,
        count,
        del,
        update,
        auth,

        listUser,
        assignUser,
        detachUser,


        listRadio,
        assignRadio,
        detachRadio,
    }

  
    public enum TaskType_t
    {
        Schedule,
        ShortMessage,
        Controler,
        Location,
        LocationInDoor,
        JobTicket,
        Patrol,
    }

    public enum HelpWindowType_t
    {
        About,
        Help,
    }
    

    public enum ToolsKey_t
    {
        Base,
        Fast,
        Controler,
        LocationInDoor,
        Help,
    }

    public enum ResourceKey_t
    {
        Organization ,
        Group,
        Staff,
        Vehicle,
        Device,
        Beacon,
    }

    public enum NavigationKey_t
    {
        Schedule,
        Location,
        LocationInDoor,
        Record,
        JobTicket,
        Patrol,
        Report,
    }

    public enum NotifyKey_t
    {
        Alarm,
        Called,
        ShortMessage,
        JobTicket,
        Patrol,

        //send
        Control,
        Location,
        LocationInDoor,      
    }

    public struct TargetOperateArgs
    {
        public TaskType_t ? Key;
        public object Target;
        public object Parameter;

        public TargetOperateArgs(TaskType_t key, object target, object param = null)
        {
            Key = key;
            Target = target;
            Parameter = param;
        }

        public TargetOperateArgs(object target, object param = null)
        {
            Key = null;
            Target = target;
            Parameter = param;
        }
    }


    public enum CallOperatedType_t
    {
        Start,
        Stop
    }
    public enum ControlerType_t
    {
        Check,
        Monitor,
        ShutDown,
        StartUp,
    }
    public enum LocationType_t
    {
        Query,
        Cycle,

        CsbkQuery,
        CsbkCycle,
        EnhCsbkQuery,
        EnhCsbkCycle,

        StopCycle,
        Track,
    }

    public enum LocationInDoorType_t
    {
        Start,
        Stop,
    }
    public enum QuickPanelType_t
    {
        Operation,
        Target,
    }

    public enum CallStatus_t
    {
        Idle,
        Tx,
        Rx,
    }
    public enum LocationStatus_t
    {
        Idle,
        Cycle,
        CsbkCycle,
        EnhCsbkCycle
    }

    public enum LocationInDoorStatus_t
    {
        Idle,
        Cycle,
    }

    public enum ChangedKey_t
    {
        All,
        OnlineStatus,
        LocationStatus,
        LocationInDoorStatus,
        ShutDownStatus,
        CallStatus,
    }
}
