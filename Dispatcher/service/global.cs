using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows;
using Sigmar.Extension;

namespace Dispatcher.Service
{    
    public class FunctionConfigure
    {
        public enum Mode_t
        {
            UnKnown =0 ,
            Repeater = 2,
            RepeaterWithMnis = 4,
            VehicleStation = 1,
            VehicleStationWithMnis = 3,
            Debug = 5,
        }

        public enum FeaturesType_t
        {
            UnKnown,
            Schedule,
            ShortMessage,
            Controler,
            Location,
            LocationInDoor,
        }


        private static bool IsDebug = false;

        public static CBaseSetting BaseSetting;
        public static CRadioSetting RadioSetting;
        public static CRepeaterSetting RepeaterSetting;
        public static CMnisSetting MnisSetting;
        public static CLocationSetting LocationSetting;
        public static CLocationInDoorSetting LocationInDoorSetting;

        private static string[] FuncationList;


        public static void SetBaseSetting(CBaseSetting setting)
        {
            BaseSetting = setting;
        }

        public static void SetRadioSetting(CRadioSetting setting)
        {
            RadioSetting = setting;
        }


        public static void SetRepeaterSetting(CRepeaterSetting setting)
        {
            RepeaterSetting = setting;
        }


        public static void SetMnisSetting(CMnisSetting setting)
        {
            MnisSetting = setting;            
        }


        public static void SetLocationSetting(CLocationSetting setting)
        {
            LocationSetting = setting;
        }


        public static void SetLocationInDoorSetting(CLocationInDoorSetting setting)
        {
            LocationInDoorSetting = setting;
        }


        public static void SetFunctions(string[] funcs)
        {
            FuncationList = funcs;
        }


        private static void InitilizeWorkMode()
        { 
            if (IsDebug) WorkMode = Mode_t.Debug;

            if (RadioSetting != null && RadioSetting.IsEnable)
            {
                if (MnisSetting != null && MnisSetting.IsEnable) WorkMode = Mode_t.VehicleStationWithMnis;
                else WorkMode = Mode_t.VehicleStation;
            }
            else if (RepeaterSetting != null && RepeaterSetting.IsEnable) 
            {
                if (MnisSetting != null && MnisSetting.IsEnable) WorkMode = Mode_t.RepeaterWithMnis;
                else WorkMode = Mode_t.Repeater;
            }
            else WorkMode = Mode_t.UnKnown;
        }

        public static int TimeoutSeconds 
        {
            get
            {
                if (WorkMode == Mode_t.UnKnown || WorkMode == Mode_t.Debug)return 10;
                else if (WorkMode == Mode_t.VehicleStation || WorkMode == Mode_t.VehicleStationWithMnis)
                {
                    if(RadioSetting != null)return RadioSetting.TomeoutSeconds;
                }
                else if (WorkMode == Mode_t.Repeater || WorkMode == Mode_t.RepeaterWithMnis)
                {
                   if(RepeaterSetting != null)return RepeaterSetting.TomeoutSeconds;
                }
             
                return 60;
            }
        } 


        public static Mode_t WorkMode { get; private set; }
        public static string CurrentUser { get; set; }


        private static void InitilizeSystemEnable()
        {
            if(WorkMode == Mode_t.UnKnown)
            {
                DisableAllFunc();
            }
            else if (WorkMode == Mode_t.Debug || WorkMode == Mode_t.VehicleStation || WorkMode == Mode_t.VehicleStationWithMnis)
            {
                EnableAllFunc();
            }
            else if(WorkMode == Mode_t.Repeater)
            {
                _enableSchedule = true;
                _enableShortMessage = false;
                _enableControler = false;
                _enableLocation = false;
                _enableLocationInDoor = false;
            }
            else if (WorkMode == Mode_t.RepeaterWithMnis)
            {
                _enableSchedule = true;
                _enableShortMessage = true;
                _enableControler = false;
                _enableLocation = true;
                _enableLocationInDoor = true;
            }
        }

        private static void InitilizeUserEnable()
        {
            if (FuncationList == null || FuncationList.Length <= 0)
            {
                DisableAllFunc();
                return;
            }

            List<FeaturesType_t> FuncList = new List<FeaturesType_t>();
            foreach (string str in FuncationList)
            {
                FeaturesType_t type = str.ToEnum<FeaturesType_t>();
                if (type != FeaturesType_t.UnKnown)FuncList.Add(type);
            }

            if (!FuncList.Contains(FeaturesType_t.Schedule)) _enableSchedule = false;
            if (!FuncList.Contains(FeaturesType_t.ShortMessage)) _enableShortMessage = false;
            if (!FuncList.Contains(FeaturesType_t.Controler)) _enableControler = false;
            if (!FuncList.Contains(FeaturesType_t.Location)) _enableLocation = false;
            if (!FuncList.Contains(FeaturesType_t.LocationInDoor)) _enableLocationInDoor = false;            
        }

        public static void InitilizeSystemConfiguration()
        {
            InitilizeWorkMode();
            InitilizeSystemEnable();
            //InitilizeUserEnable();

            InitilizeViews();
        }


        private static void EnableAllFunc()
        {
            _enableSchedule = true;
            _enableShortMessage = true;
            _enableControler = true;
            _enableLocation = true;
            _enableLocationInDoor = true;
        }
        private static void DisableAllFunc()
        {
            _enableSchedule = false;
            _enableShortMessage = false;
            _enableControler = false;
            _enableLocation = false;
            _enableLocationInDoor = false;
        }


       

       

        public static bool _enableSchedule;
        public static bool _enableShortMessage;
        public static bool _enableControler;
        public static bool _enableLocation;
        public static bool _enableLocationInDoor;
       
        public static Visibility ScheduleVisible{get{return _enableSchedule ? Visibility.Visible : Visibility.Collapsed;}}
        public static Visibility ShortMessageVisible { get { return _enableShortMessage ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility ControlerVisible { get { return _enableControler ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility LocationVisible { get { return _enableLocation ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility LocationInDoorVisible { get { return _enableLocationInDoor ? Visibility.Visible : Visibility.Collapsed; } }


        public static bool _enableJobTicket = false;
        public static bool _enablePatrol = false;
        public static bool _enableRecord = false;
        public static Visibility JobTicketVisible { get { return _enableJobTicket ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility PatrolVisible { get { return _enablePatrol ? Visibility.Visible : Visibility.Collapsed; } }      
        public static Visibility RecordVisible { get { return _enableRecord ? Visibility.Visible : Visibility.Collapsed; } }



        public static bool _enableReport = true;
        public static bool _enableAlarm = false;
        public static bool _enableExport = false;
        public static bool _enablePrint = false;
        public static bool _enableTrack = false;
        public static Visibility ReportVisible { get { return _enableReport ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility AlarmVisible { get { return _enableAlarm ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility ExportVisible { get { return _enableExport ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility PrintVisible { get { return _enablePrint ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility TrackVisible { get { return _enableTrack ? Visibility.Visible : Visibility.Collapsed; } }

       
        private static bool _enableAdvanceTools = false;
        private static bool _enableHelpFile = false;
        private static bool _usePttToCall = true;
        public static Visibility AdvanceToolsVisible { get { return _enableAdvanceTools ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility HelpFileVisible { get { return _enableHelpFile ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility PTTToCallVisible { get { return _usePttToCall ? Visibility.Visible : Visibility.Collapsed; } }
        public static Visibility CheckBoxToCallVisible { get { return !_usePttToCall ? Visibility.Visible : Visibility.Collapsed; } }




        private static void InitilizeViews()
        {
            if (!_enableSchedule)
            {
                EnableViewNavigationSchedule = false;
                EnableViewNotifyCalled = false;
            }
            if (!_enableShortMessage)
            {
                EnableViewNotifyShortMessage = false;
            }
            if (!_enableControler)
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

        public static bool EnableViewBaseTools = true;
        public static bool EnableViewFastTools = true;

        public static bool EnableViewControlerTools = true;
        public static bool EnableViewLocationInDoorTools = true;

        public static bool EnableViewHelpTools = true;

        public static bool EnableViewResOrganization = true;
        public static bool EnableViewResGroup = true;
        public static bool EnableViewResStaff = true;
        public static bool EnableViewResVehicle = true;
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

        public static double NavigationHeight
        {
            get{

                int count = 0;

                if (_enableSchedule) count++;                
                if (_enableLocation) count++;
                if (_enableLocationInDoor) count++;
                if (_enableJobTicket) count++;
                if (_enablePatrol) count++;
                if (_enableRecord) count++;
                if (_enableReport) count++;


                if (count > 0) return (count-1) * 40 + 42;
                else return 0; 
            }
        }
      
    }
}


