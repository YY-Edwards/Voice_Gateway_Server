using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace TrboX
{
    public delegate void OnTcpRx(string str);
    public class TcpInterface
    {
        private OnTcpRx m_OnRx = null;
     
        private Socket clientSocket;
        private Dictionary<Int64, object> ReceiveStr = new Dictionary<Int64, object>();

        public TcpInterface(IPEndPoint addr)
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                clientSocket.Connect(addr); //配置服务器IP与端口  
                Console.WriteLine("连接服务器成功");
            }
            catch
            {
                Console.WriteLine("连接服务器失败，请按回车键退出！");
                return;
            }

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)ReceiveString(); });
            Thread th = new Thread(threadStart);
            th.Start();
        }

        public TcpInterface(IPEndPoint addr, OnTcpRx OnRx)
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                clientSocket.Connect(addr); //配置服务器IP与端口  
                Console.WriteLine("连接服务器成功");
            }
            catch
            {
                Console.WriteLine("连接服务器失败，请按回车键退出！");
                return;
            }

            m_OnRx = OnRx;

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)ReceiveString(); });
            Thread th = new Thread(threadStart);
            th.Start();
        }

        public void Close()
        {
            if (null == clientSocket) return;
            try
            {
                clientSocket.Shutdown(SocketShutdown.Both);
                clientSocket.Close();
            }
            catch { }
            clientSocket = null;
            Console.WriteLine("断开服务器");
        }

        public void WriteString(string str)
        {

            for (int i = 0; i < 10; i++)
            {
                try
                {
                    clientSocket.Send(Encoding.Default.GetBytes(str));
                    Console.WriteLine("向服务器发送消息：{0}", str);
                    return;
                }
                catch
                {
                    Thread.Sleep(10);    //等待1秒钟  
                    continue;
                }
            }

            Close();
        }

        public void ReceiveString()
        {
           try
           {
               byte[] result = new byte[1024];
               int receiveLength = clientSocket.Receive(result);
               string rxstr = Encoding.ASCII.GetString(result, 0, receiveLength);

               m_OnRx(rxstr);

               Console.WriteLine("接收消息：{0}", rxstr);
           }
           catch
           {
               Console.WriteLine(" 连接异常");
           }
            Thread.Sleep(1000);
        }

        public object ReadString(Int64 callId = -1)
        {
            object res = null;
            Int64 del = -1;
            for (int i = 0; i < 50; i++)
            {
                lock (ReceiveStr)
                {
                    if (ReceiveStr.Count > 0)
                    {
                        try
                        {
                            if (callId < 0)
                            {
                                foreach (var value in ReceiveStr)
                                {
                                    res = value.Value;
                                    del = value.Key;
                                    break;
                                }
                            }
                            else
                            {
                                res = ReceiveStr[callId];
                                del = callId;
                            }

                            break;
                        }
                        catch { }
                    }
                }
                Thread.Sleep(100);
            }

            ReceiveStr.Remove(del);
            return res;
        }
    }
}
