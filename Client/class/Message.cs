using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX 
{
    
    public enum NotifyType
    {
        Alarm,
        Message,
        Call,
        JobTicker,
        Tracker,
    };
    
    [Serializable]
    public class CAlarmNotification
    {
       public  string Content { set; get; }
    }

    [Serializable]
    public class CRxNotification
    {

    }

    [Serializable]
    public class CMsgNotification
    {
        public string Content { set; get; }
    }

    [Serializable]
    public class CJobTicketNotification
    {
    }

    [Serializable]
    public class CTackerNotification
    {
    }

    [Serializable]
    public class CNotification
    {
        public NotifyType Type { set; get; }
        public CRelationShipObj Source { set; get; }
        public DateTime Time { set; get; }
        public object Content{ set; get; }
    }
}
