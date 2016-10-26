using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Globalization;

namespace TrboX
{
   

    public class RadioCallParam
    {
        public ExecType Operate;
        public TargetType Type;
        public long Target;
    }

    public class RadioPrivateCallParam
    {
        public long id;

        public RadioPrivateCallParam(COperate operate)
        {
            try{               
                id = operate.Target.Target[0].Radio.RadioID;
            }
            catch{}           
        }
    }
    public class RadioGroupCallParam
    {
        public long id;

        public RadioGroupCallParam(COperate operate)
        {
            try
            {
                id = operate.Target.Target[0].Group.GroupID;
            }
            catch { }
        }
    }

    public class RadioPrivateSmsParam
    {
        public long id;
        public string msg;
        public RadioPrivateSmsParam(COperate operate)
        {
            try
            {
                id = operate.Target.Target[0].Radio.RadioID;
                msg = ((CShortMessage)operate.Operate).Message;
            }
            catch { }

            //byte[] temp;
            //temp = Encoding.Default.GetBytes(msg);
            ////利用 Encoding 类的 Convert 方法，将 temp 的编码由 gb2312 转换为 big5 编码
            //temp = Encoding.Convert(Encoding.Default, Encoding.UTF8, temp);
            ////将   byte 数组 转换为 string
            //msg = Encoding.UTF8.GetString(temp);

            msg = Encoding.Default.GetString(Encoding.UTF8.GetBytes(msg)); 
            //msg = RadioGroupSmsParam.UnicodeToString(msg);
        }
    } 

    public class RadioGroupSmsParam
    {
        public long id;
        public string msg;

        public static string UnicodeToString(string srcText)
        {

            string dst = "";
            string src = srcText;
            int len = srcText.Length / 6;
            for (int i = 0; i <= len - 1; i++)
            {
                string str = "";
                str = src.Substring(0, 6).Substring(2);
                src = src.Substring(6);
                byte[] bytes = new byte[2];
                bytes[1] = byte.Parse(int.Parse(str.Substring(0, 2), NumberStyles.HexNumber).ToString());
                bytes[0] = byte.Parse(int.Parse(str.Substring(2, 2), NumberStyles.HexNumber).ToString());
                dst += Encoding.Unicode.GetString(bytes);
            }
            return dst;
        } 

        public RadioGroupSmsParam(COperate operate)
        {
            try
            {
                id = operate.Target.Target[0].Group.GroupID;
                msg = ((CShortMessage)operate.Operate).Message;
            }
            catch { }
            msg = UnicodeToString(msg);
        }
    }

    public class RadioOperateStr
    {
        public string call;
        public string type;
        public string callId;
        public object param;
    }

    public class RadioOperateSimpleStr
    {
        public string call;
        public string type;
        public string callId;
    }

    public class RadioOperate
    {
        public RequestType Call;
        private long CallId;
        private object Param;

        public ParseDel Parse;

        private object ToStr()
        {

            if (null == Param)
            {
                return new RadioOperateSimpleStr()
                {
                    call = Call.ToString(),
                    type = "radio",
                    callId = CallId.ToString(),
                };
            }
            else
            {
                return new RadioOperateStr()
                {
                    call = Call.ToString(),
                    type = "radio",
                    callId = CallId.ToString(),
                    param = Param
                };
            }

        }

        public string Json
        {
            get
            {
                return JsonConvert.SerializeObject(this.ToStr()) + "\r\n";
            }
        }

        public RadioOperate(COperate operate, long PN)
        {
            Param = null;

            try
            {
                if (operate.Type == OPType.Dispatch)
                {
                    if (SelectionType.Null == operate.Target.Type || operate.Target.Target.Count <= 0) Call = RequestType.call;
                    Call = RequestType.call;

                    if (SelectionType.Null != operate.Target.Type)
                    {
                        if (MemberType.Group == operate.Target.Target[0].Type)
                        {
                            Call = RequestType.groupCall;
                            Param = new RadioGroupCallParam(operate);
                        }
                        else if (null != operate.Target.Target[0].Radio && operate.Target.Target[0].Radio.ID > 0)
                        {
                            Call = RequestType.call;
                            Param = new RadioPrivateCallParam(operate);
                        }
                    }
                }
                else if (operate.Type == OPType.ShortMessage)
                {
                    if ((SelectionType.Single == operate.Target.Type) || (SelectionType.Multiple == operate.Target.Type))
                    {
                        if (MemberType.Group == operate.Target.Target[0].Type)
                        {
                            Call = RequestType.sendGroupSms;
                            Param = new RadioGroupSmsParam(operate);

                        }
                        else if (null != operate.Target.Target[0].Radio && operate.Target.Target[0].Radio.ID > 0)
                        {
                            Call = RequestType.sendSms;
                            Param = new RadioPrivateSmsParam(operate);
                        }
                    }
                }
            }
            catch
            {
               
            }

            CallId = PN;

            Parse = null;
        }

        //public string Parse(COperate op, long PN)
        //{
        //    return "";
        //}

       

       private RequestType GetRequest(COperate op)
        {
            

            return RequestType.None;
        }

       private RequestType GetParam(RequestType type, COperate op)
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
               else if (op.Type == OPType.ShortMessage)
               {
                   if ((SelectionType.All == op.Target.Type) || (SelectionType.Null == op.Target.Type) || (op.Target.Target.Count <= 0)) return RequestType.None;
                   if (MemberType.Group == op.Target.Target[0].Type) return RequestType.sendGroupSms;
                   else
                   {
                       if (null != op.Target.Target[0].Radio && op.Target.Target[0].Radio.ID > 0) return RequestType.sendSms;
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
