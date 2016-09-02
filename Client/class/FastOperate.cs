using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace TrboX
{   

    public enum FastType
    { 
        FastType_Contact,
        FastType_Operate
    };


    [Serializable]
    public class FastOperate
    {
        public FastType m_Type { set; get; }

        public CRelationShipObj m_Contact { set; get; }

        public COperate m_Operate { set; get; }

        public FastOperate()
        {
            m_Contact = null;
            m_Operate = null;
        }
    }
}
