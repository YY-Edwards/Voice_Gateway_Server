using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Dispatcher.Modules
{
    public class CMember:Mancar
    {
        public CMember()           
        {

        }


        public MemberType_t MemberType
        {
            get
            {
                if (!HasMancar) return DeviceType == DeviceType_t.Handset ? MemberType_t.Handset : MemberType_t.VehicleStation;
                else return ManCarType == ManCarType_t.Staff ? MemberType_t.Staff : MemberType_t.Vehicle;
            }
        }

        public int ID
        {
            get
            {
                switch (MemberType)
                {
                    case MemberType_t.Staff:
                    case MemberType_t.Vehicle:
                        if (HasDevice) return DeviceID;
                        else return ManCarID;
                    case MemberType_t.Handset:
                    case MemberType_t.VehicleStation:
                        return DeviceID;
                    default:
                        return 0;
                }
            }
        }
                
        public string Name
        {
            get
            {
                switch (MemberType)
                {
                    case MemberType_t.Staff:
                    case MemberType_t.Vehicle:
                        if (HasDevice) return ManCarName;
                        else return ManCarName;
                    case MemberType_t.Handset:
                    case MemberType_t.VehicleStation:
                        return (DeviceType == DeviceType_t.Handset ? "手持台：" : "车载台 ：") + RadioID.ToString();
                    default:
                        return "";
                }
            }
        }
        

        public string FullName
        {
            get 
            {
                switch (MemberType)
                {
                    case MemberType_t.Staff:
                    case MemberType_t.Vehicle:
                        if (HasDevice) return ManCarName + "(" + (DeviceType == DeviceType_t.Handset ? "手持台：" : "车载台 ：") + RadioID.ToString() + ")";
                        else return ManCarName;
                    case MemberType_t.Handset:
                    case MemberType_t.VehicleStation:
                        return (DeviceType == DeviceType_t.Handset ? "手持台：" : "车载台 ：") + RadioID.ToString();
                    default:
                        return "";
                }
            }
        }

        public string DeviceName
        {
            get
            {
                switch (MemberType)
                {
                    case MemberType_t.Staff:
                    case MemberType_t.Vehicle:
                        if (HasDevice) return (DeviceType == DeviceType_t.Handset ? "手持台：" : "车载台 ：") + RadioID.ToString() + "(" + ManCarName + ")";
                        else return "";
                    case MemberType_t.Handset:
                    case MemberType_t.VehicleStation:
                        return (DeviceType == DeviceType_t.Handset ? "手持台：" : "车载台 ：") + RadioID.ToString();
                    default:
                        return "";
                }
            }
        }
        
        public enum MemberType_t
        {
            Staff,
            Vehicle,
            Handset,
            VehicleStation,
        }
    }
}
