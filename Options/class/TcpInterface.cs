using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace TrboX
{
    public delegate void OnConnectDel();
    public delegate void OnTcpRx(string str);
    public delegate void OnTcpRxBytes(byte[] bytes);
    public class TcpInterface
    {
        private OnTcpRx m_OnRx = null;
        private OnTcpRxBytes m_OnRxBytes = null;

        private Socket clientSocket;
        private Dictionary<Int64, object> ReceiveStr = new Dictionary<Int64, object>();
        public bool isConnect = true;
        private IPEndPoint m_addr;

        public OnConnectDel OnConnect = null;
        public OnConnectDel OnDisconnect = null;

        private static Thread thread = null;

        public TcpInterface(IPEndPoint addr)
        {
            m_addr = addr;

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)ReceiveString(); });
            Thread th = new Thread(threadStart);

        }

        public TcpInterface(IPEndPoint addr, OnTcpRx OnRx)
        {
            m_addr = addr;
            m_OnRx = OnRx;

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)ReceiveString(); });
            thread = new Thread(threadStart);
        }

        public TcpInterface(IPEndPoint addr, OnTcpRxBytes OnRx)
        {
            m_addr = addr;
            m_OnRxBytes = OnRx;

            ThreadStart threadStart = new ThreadStart(delegate() { while (true)ReceiveString(); });
            thread = new Thread(threadStart);
        }

        private void TryConnect()
        {
            Open();
        }

        public void Open()
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                clientSocket.Connect(m_addr); //配置服务器IP与端口  
                // Console.WriteLine("连接服务器成功");               
                isConnect = true;
                DataBase.InsertLog("连接服务器" + m_addr.ToString() + "成功");

                if (OnConnect != null) OnConnect();
            }
            catch
            {
                //Console.WriteLine("连接服务器失败");
                if (isConnect) DataBase.InsertLog("连接服务器" + m_addr.ToString() + "失败");
                isConnect = false;
            }

            if (thread != null && !thread.IsAlive) thread.Start();
        }


        public void Close()
        {
            if (null == clientSocket) return;
            try
            {
                clientSocket.Shutdown(SocketShutdown.Both);
                clientSocket.Close();
            }
            catch
            {
                DataBase.InsertLog("关闭Shocket失败");
            }
            clientSocket = null;
            // Console.WriteLine("断开服务器");
            isConnect = false;
            if (OnDisconnect != null) OnDisconnect();
        }

        public void WriteString(string str)
        {
            if (isConnect == false) return;

            for (int i = 0; i < 10; i++)
            {
                try
                {
                    clientSocket.Send(Encoding.Default.GetBytes(str));
                    //Console.WriteLine("向服务器发送消息：{0}", str);
                    DataBase.InsertLog("Write To" + m_addr.ToString() + ":" + str);
                    return;
                }
                catch
                {
                    Thread.Sleep(100);    //等待1秒钟  
                    continue;
                }
            }

            if (isConnect) DataBase.InsertLog("连接服务器" + m_addr.ToString() + "失败");
            isConnect = false;

            Close();
        }

        public void WriteBytes(byte[] bytes)
        {
            if (isConnect == false) return;

            for (int i = 0; i < 10; i++)
            {
                try
                {
                    clientSocket.Send(bytes);
                    //Console.WriteLine("向服务器发送消息：{0}", str);
                    //DataBase.InsertLog("Write To" + m_addr.ToString() + ":" + str);
                    return;
                }
                catch
                {
                    Thread.Sleep(100);    //等待1秒钟  
                    continue;
                }
            }

            if (isConnect) DataBase.InsertLog("连接服务器" + m_addr.ToString() + "失败");
            isConnect = false;

            Close();
        }

        public void ReceiveString()
        {
            while (true)
            {
                if (!isConnect)
                {
                    TryConnect();
                    Thread.Sleep(3000);
                }
                try
                {
                    byte[] result = new byte[65536];
                    int receiveLength = clientSocket.Receive(result);
                    if (m_OnRxBytes != null) m_OnRxBytes(result.Take(receiveLength).ToArray());
                    string rxstr = Encoding.Default.GetString(result, 0, receiveLength);

                    DataBase.InsertLog("Receive From" + m_addr.ToString() + ":" + rxstr);
                    if(m_OnRx!=null)m_OnRx(rxstr);

                }
                catch
                {
                    if (isConnect) DataBase.InsertLog("连接服务器" + m_addr.ToString() + "失败");
                    isConnect = false;
                    //Console.WriteLine(" 连接异常");
                }

                Thread.Sleep(100);
            }
        }
    }
}
