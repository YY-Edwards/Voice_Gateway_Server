using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows;
using Sigmar.Extension;

namespace Dispatcher.Service
{
    public class RunAccess
    {
        public static string[] Enable;
        public static Mode_t Mode = Mode_t.None;
        public enum Mode_t
        {
            None,
            All,
            LCP,
            CPC,
            IPSC,
            VehicleStation,
            VehicleStationWithMnis,
        }

        public enum FeaturesType_t
        {
            Schedule,
            ShortMessage,
            Controler,
            Location,
            JobTicket,
            Patrol,
            LocationInDoor,
            Record,
        }
    }
    public class FunctionConfigure
    {       
        static FunctionConfigure()
        {
            InitilizeExtras();           

            switch (RunAccess.Mode)
            {
                case RunAccess.Mode_t.None:
                    DisableAll();
                    break;
                case RunAccess.Mode_t.All:
                    EnableAll();
                    break;
                case RunAccess.Mode_t.LCP:
                case RunAccess.Mode_t.CPC:
                case RunAccess.Mode_t.IPSC:
                    EnableRepeater();
                    break;
                case RunAccess.Mode_t.VehicleStation:
                case RunAccess.Mode_t.VehicleStationWithMnis:
                    VehicleStation();
                    break;
                default:
                    break;
            }

            //SelectFunc(RunAccess.Enable);
            InitilizeViews();

        }


        private static void InitilizeExtras()
        {
#if false // debug
            _enableReport = true;
            _enableAlarm = true;
            _enableExport = true;
            _enablePrint = true;

            _enableAdvanceTools = true;
            _enableHelpFile = true;

            _enableDeviceSleep = true;
            _enableDebug = true;
#else
            _enableReport = true;
            _enableAlarm = true;
            _enableExport = false;
            _enablePrint = false;

            _enableAdvanceTools = false;
            _enableHelpFile = false;
            _usePttToCall = true;
            _enableDebug = true;
#endif
        }
        private static void DisableAll()
        {
            _enableSchedule = false;
            _enableShortMessage = false;
            _enableControler = false;
            _enableLocation = false;
            _enableJobTicket = false;
            _enablePatrol = false;
            _enableLocationInDoor = false;
            _enableRecord = false;
        }

        private static void EnableAll()
        {
            _enableSchedule = true;
            _enableShortMessage = true;
            _enableControler = true;
            _enableLocation = true;
            _enableJobTicket = true;
            _enablePatrol = true;
            _enableLocationInDoor = true;
            _enableRecord = true;
        }
       
        private static void EnableRepeater()
        {
            _enableSchedule = true;
            _enableShortMessage = true;
            _enableControler = false;
            _enableLocation = true;
            _enableJobTicket = false;
            _enablePatrol = false;
            _enableLocationInDoor = true;
            _enableRecord = false;
        }

        private static void VehicleStation()
        {
            _enableSchedule = true;
            _enableShortMessage = true;
            _enableControler = true;
            _enableLocation = true;
            _enableJobTicket = false;
            _enablePatrol = false;
            _enableLocationInDoor = true;
            _enableRecord = false;
        }

        private static void SelectFunc(string[] access)
        {
            if (access == null || access.Length <=0)
            {
                DisableAll();
                return;
            }

            List<RunAccess.FeaturesType_t> FuncList = new List<RunAccess.FeaturesType_t>();
            foreach(string str in access)
            {
                RunAccess.FeaturesType_t type = str.ToEnum<RunAccess.FeaturesType_t>();
                FuncList.Add(type);                
            }

            if (!FuncList.Contains(RunAccess.FeaturesType_t.Schedule)) _enableSchedule = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.ShortMessage)) _enableShortMessage = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.Controler)) _enableControler = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.Location )) _enableLocation = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.JobTicket)) _enableJobTicket = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.Patrol)) _enablePatrol = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.LocationInDoor)) _enableLocationInDoor = false;
            if (!FuncList.Contains(RunAccess.FeaturesType_t.Record)) _enableRecord = false;           
        }

        public static string DebugHost = "192.168.2.140";
        public static bool ConnectReomote = true;


        public static bool _enableSchedule;
        public static bool _enableShortMessage;
        public static bool _enableControler;
        public static bool _enableLocation;
        public static bool _enableJobTicket;
        public static bool _enablePatrol;
        public static bool _enableLocationInDoor;
        public static bool _enableRecord;


        public static bool _enableReport;
        public static bool _enableAlarm;
        public static bool _enableExport;
        public static bool _enablePrint;

        private static bool _enableAdvanceTools;
        private static bool _enableHelpFile;

        private static bool _usePttToCall;

        public static bool _enableDebug;

        private static void InitilizeViews()
        {
            if (!_enableSchedule)
            {
                EnableViewNavigationSchedule = false;
                EnableViewNotifyCalled = false;
            }
            if(!_enableShortMessage)
            {
                EnableViewNotifyShortMessage = false;
            }
            if(!_enableControler)
            {
                EnableViewControlerTools = false;
            }

            if (!_enableLocation)
            {
                EnableViewNavigationLocation = false;
            }
            if (!_enableJobTicket)
            {
                EnableViewNavigationJobTicket = false;
                EnableViewNotifyJobTicket = false;
            }
            if (!_enablePatrol)
            {
                EnableViewNavigationPatrol = false;
                EnableViewNotifyPatrol = false;
            }
            if (!_enableLocationInDoor)
            {
                EnableViewLocationInDoorTools = false;
                EnableViewNavigationLocationInDoor = false;
            }

            if (!_enableRecord)
            {
                EnableViewNavigationRecord = false;
            }
        }
        //initilize views
        public static bool EnableViewBaseTools = true;
        public static bool EnableViewFastTools = true;

        public static bool EnableViewControlerTools = true;
        public static bool EnableViewLocationInDoorTools = true;

        public static bool EnableViewHelpTools = true;

        public static bool EnableViewResOrganization  = true;
        public static bool EnableViewResGroup  = true; 
        public static bool EnableViewResStaff   = true;  
        public static bool EnableViewResVehicle  = true;
        public static bool EnableViewResDevice = true;
        public static bool EnableViewResBeacon = true;

        public static bool EnableViewNavigationSchedule = true;
        public static bool EnableViewNavigationLocation = true;
        public static bool EnableViewNavigationLocationInDoor = true;
        public static bool EnableViewNavigationRecord = true;
        public static bool EnableViewNavigationJobTicket = true;
        public static bool EnableViewNavigationPatrol = true;        
        public static bool EnableViewNavigationReport = true;

        public static bool EnableViewEvent = true;

        public static bool EnableViewNotifyAlarm = true;
        public static bool EnableViewNotifyCalled = true;
        public static bool EnableViewNotifyShortMessage = true;
        public static bool EnableViewNotifyJobTicket = true;
        public static bool EnableViewNotifyPatrol = true;
        

        public static Visibility ScheduleVisible{get{return _enableSchedule ? Visibility.Visible : Visibility.Collapsed;}}
        public static Visibility ShortMessageVisible { get { return _enableShortMessage ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility ControlerVisible { get { return _enableControler ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility LocationVisible { get { return _enableLocation ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility JobTicketVisible { get { return _enableJobTicket ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility PatrolVisible { get { return _enablePatrol ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility LocationInDoorVisible { get { return _enableLocationInDoor ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility RecordVisible { get { return _enableRecord ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility ReportVisible { get { return _enableReport ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility AlarmVisible { get { return _enableAlarm ? Visibility.Visible : Visibility.Collapsed; } }


        public static Visibility ExportVisible { get { return _enableExport ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility PrintVisible { get { return _enablePrint ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility AdvanceToolsVisible { get { return _enableAdvanceTools ? Visibility.Visible : Visibility.Collapsed; } }

        public static Visibility HelpFileVisible { get { return _enableHelpFile ? Visibility.Visible : Visibility.Collapsed; } }

        public static Visibility PTTToCallVisible { get { return _usePttToCall ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility CheckBoxToCallVisible { get { return !_usePttToCall ? Visibility.Visible : Visibility.Collapsed; } }
      
    }
}


