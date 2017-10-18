using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dispatcher.Service;

using Sigmar.Logger;
namespace Dispatcher.Modules
{
    public class Device
    {
        public bool HasMancar{ get; set; }
        public bool HasDevice { get; set; }

        public bool IsOnline { get; set; }
        public LocationStatus_t LocationStatus { get; set; }
        public LocationInDoorStatus_t LocationInDoorStatus { get; set; }
        public bool IsShutDown { get; set; }
        public CallStatus_t CallStatus { get; set; }

        public DeviceType_t DeviceType { get; set; }

        public int DeviceID { get; set; }
        public int RadioID { get; set; }
        public int GroupID { get; set; }
                //parts
        public bool HasScreen { get; set; }
        public bool HasKeyBoard { get; set; }
        public bool HasLocation { get; set; }
        public bool HasLocatinInDoor { get; set; }

        public Device()
        {
            IsOnline = false;
            LocationStatus  = LocationStatus_t.Idle;
            LocationInDoorStatus = LocationInDoorStatus_t.Idle;
            IsShutDown = false;
            CallStatus = CallStatus_t.Idle;
            DeviceType = DeviceType_t.Handset;

            RadioID = 0;
            GroupID = 0;

            //parts
            HasScreen = false;
            HasKeyBoard = false;
            HasLocation = false;
            HasLocatinInDoor = false;
        }

        public Device SetDeviceType(DeviceType_t type, bool hasmancar = true)
        {
            HasDevice = true;
            DeviceType = type;
            HasMancar = hasmancar;
            return this;
        }

        public Device SetDeviceID(int id)
        {
            RadioID = id;
            return this;
        }

        public Device SetParts(bool hasscreen, bool keyboard, bool location, bool locationindoor)
        {
            HasScreen = hasscreen;
            HasKeyBoard = keyboard;
            HasLocation = location;
            HasLocatinInDoor = locationindoor;
            return this;
        }

        public Device UpdateStatus(ChangedKey_t key, object value)
        {
            switch (key)
            {
                case ChangedKey_t.OnlineStatus:
                    IsOnline = (bool)value;
                    break;
                case ChangedKey_t.LocationStatus:
                    LocationStatus = (LocationStatus_t)value;
                    break;
                case ChangedKey_t.LocationInDoorStatus:
                    LocationInDoorStatus = (LocationInDoorStatus_t)value;
                    break;
                case ChangedKey_t.ShutDownStatus:
                    IsShutDown = (bool)value;
                    break;               
                case ChangedKey_t.CallStatus:
                    CallStatus = (CallStatus_t)value;
                    break;
                default:
                    break;
            }

            return this;
        }

        public enum DeviceType_t
        {
            Handset,
            VehicleStation,
        }
    }
}
