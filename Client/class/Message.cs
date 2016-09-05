using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX 
{
    [Serializable]
    public class CAlarmNotification
    {
       public  string Content { set; get; }
       public DateTime time { set; get; }
    }

    [Serializable]
   public class CRxNotification
   {
       public CRelationShipObj Source { set; get; }
       public DateTime time { set; get; }
   }

    [Serializable]
    public class CMsgNotification
    {
        public CRelationShipObj Source { set; get; }
        public DateTime time { set; get; }
        public string Content { set; get; }
    }

    [Serializable]
    public class CJobTicketNotification
    {
        public CRelationShipObj Source { set; get; }
        public DateTime time { set; get; }

        //job
    }

    [Serializable]
    public class CTackerNotification
    {
        public CRelationShipObj Source { set; get; }
        public DateTime time { set; get; }
    }

}
