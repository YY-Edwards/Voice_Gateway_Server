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
        public FastType Type { set; get; }

        public CMultMember Contact { set; get; }

        public COperate Operate { set; get; }

        public FastOperate()
        {
            Contact = null;
            Operate = null;
        }

        public bool IsEqual(FastOperate operate)
        {
            if (null == operate) return false;
            
            if (Type != operate.Type) return false;
            if (Type == FastType.FastType_Contact)
            {
                if ((null == Contact) && (null == operate.Contact)) return true;
                return (null == Contact) ? false : Contact.IsEqual(operate.Contact);
            }
            else
            {
                if ((null == Operate) && (null == operate.Operate)) return true;
                return (null == Operate) ? false : Operate.IsEqual(operate.Operate);
            }
        }
        public string SimpleName
        {
            get
            {
                if (Type == FastType.FastType_Contact)
                    return Contact.Name;
                else
                    return (null == Operate.Target) ? "" : Operate.Target.SimpleName;
            }
        }

        public string Name
        {
            get {
                if (Type == FastType.FastType_Contact)
                    return Contact.Name;                 
                else
                    return Operate.Name;    
            }
        }

        public string Information
        {
            get
            {
                if (Type == FastType.FastType_Contact)
                    return Contact.Information;
                else
                    return Operate.Information;
            }
        }

        public string NameInfo
        {
            get
            {
                if (Type == FastType.FastType_Contact)
                    return Contact.NameInfo;
                else
                    return Operate.NameInfo;
            }
        }
    }
}
