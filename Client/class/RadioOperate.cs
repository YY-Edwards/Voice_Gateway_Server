using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{

    public class RadioCallParam
    {
        public long id;
        public RadioCallParam(COperate op)
        {
            try
            {
                id = op.Target.Target[0].Radio.RadioID;
            }
            catch {
                id = 0;
            }           
        }
    }
    public class RadioOperate
    {
        public RadioOperate(COperate operate)
        {

        }


        public string Parse(COperate op, long PN)
        {
            return "";
        }


        public static object Convert(COperate op)
        {
            switch(GetRequest(op))
            {
                case RequestType.call:
                    return new RadioCallParam(op);
            }
            
            return null;
        }




        public static RequestType GetRequest(COperate op)
        {
            try
            {
                if (op.Type == OPType.Dispatch)
                {
                    if (SelectionType.All == op.Target.Type) return RequestType.allCall;
                    if ((SelectionType.Null == op.Target.Type) || (op.Target.Target.Count <= 0)) return RequestType.None;
                    if (MemberType.Group == op.Target.Target[0].Type) return RequestType.groupCall;
                    else
                    {
                        if (null != op.Target.Target[0].Radio && op.Target.Target[0].Radio.ID > 0) return RequestType.call;
                    }
                }
            }
            catch
            {
                return RequestType.None;
            }

            return RequestType.None;

        }


    }
}
