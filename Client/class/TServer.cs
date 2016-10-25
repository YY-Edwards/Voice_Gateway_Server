using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;

namespace TrboX
{
   
    public class TServer
    {
        private static TcpInterface TCP = null;
        private static long PackageNumber = 0;
        private static Dictionary<int, RequestType> SendList = new Dictionary<int, RequestType>();

        public TServer()
        {
            TCP = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9000), OnReceive);
        }

        public static void InitializeTServer()
        {
            TCP = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9000), OnReceive);
        }

        public static void Write(COperate operate)
        {
            RadioOperate radio = new RadioOperate(operate);

            //RequestType type = Radio.g
            
            
            //Write(Radio.Parse(operate, PackageNumber));
            //SendList.Add(JsonParse.CallID, type);
            PackageNumber++;

        }

        private static void Write(string str)
        {
            if (null != TCP)
            {
                
            }
        }

        private static void OnReceive(string str)
        {

        }
    }
    public enum RequestType
    {
        None,
        //base
        setBaseSetting,
        getBaseSetting,
        setRadioSetting,
        getRadioSetting,
        setRepeaterSetting,
        getRepeaterSetting,

        getUser,
        getUserCount,
        addUser,
        deleteUser,
        updateUser,

        getStaff,
        getStaffCount,
        addStaff,
        deleteStaff,
        updateStaff,

        getDepartment,
        getDepartmentCount,
        addDepartment,
        deleteDepartment,
        updateDepartment,

        getRadio,
        getRadioCount,
        addRadio,
        deleteRadio,
        updateRadio,

        getRadioBelong,
        getRadioBelongCount,
        addRadioBelong,
        deleteRadioBelong,
        updateRadioBelong,

        call,
        groupCall,
        allCall,
    };

}
