using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace TrboX
{

    public enum TargetSystemType
    {
        radio, 
        Reapeater
    }
    public class RpcInterface
    {
       // private TcpInterface TServer = new TcpInterface(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9001));

        public delegate object ParseResult(RequestType type, object res);
        private ParseResult m_Rx = null;
        private TargetSystemType m_Type = TargetSystemType.radio;

        private Dictionary<long, RequestType> m_SenedRequest = new Dictionary<long, RequestType>();

        public RpcInterface()
        {
            Thread t = new Thread(() =>{OnRx();});
            t.Start();
        }
        public void SetType(TargetSystemType type)
        {
            m_Type = type;
        }

        private void OnRx()
        { 
            while(true)
            {
                object obj = TServer.ReadString();
               
                    try
                    { if(obj is TcpRequset)
                {
                        m_Rx((RequestType)Enum.Parse(typeof(RequestType), ((TcpRequset)obj).call), obj);
                    }
                         else if(obj is TcpResponse)
                {
                     m_Rx(m_SenedRequest[((TcpRequset)obj).callId], obj);
                }
                    }
                    catch
                    {

                    }
                }
            }
        
        public void SetReceiveFunc(ParseResult pr)
        {
            m_Rx = pr;
        }
        public void ExecuteOperate( COperate op)
        {
            RpcCall(GetRequest(op), Convert(op));
        }


        private void RpcCall(RequestType type, object param)
        {
            TServer.WriteString(JsonParse.Op2Json(type, param, m_Type));
            m_SenedRequest.Add(JsonParse.CallID, type);
        }

        private object Convert(COperate op)
        {
            switch (m_Type)
            {
                case TargetSystemType.radio:
                    return RadioOperate.Convert(op);
                case TargetSystemType.Reapeater:
                    return null;
            }
            return null;
        }

        private RequestType GetRequest(COperate op)
        {
            switch (m_Type)
            {
                case TargetSystemType.radio:
                    return RadioOperate.GetRequest(op);
                case TargetSystemType.Reapeater:
                    return RequestType.None;
            }
            return RequestType.None;
        }

    }
}
