using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net.Sockets;
using System.Threading;

namespace Manager
{
    public class CTcpClient
    {      
        private Socket m_Socket;
        private bool m_IsConnect = false;


        public bool IsConnect
        {
            get
            {
                return m_IsConnect;
            }
        }

        public delegate void OnEventHandle(object sender);
        public event OnEventHandle OnConnected;
        public event OnEventHandle OnDisconnected;

        public delegate void OnRxHandle(object sender, byte[] data);
        public event OnRxHandle OnRecvData;

        private string m_Host = "";
        private int m_Port = 0;

        public string Host
        {
            get { return m_Host; }
        }

        public int Port
        {
            get { return m_Port; }
        }

        public void Connect(string host, int port)
        {
            if (m_Host == host && m_Port == port && m_IsConnect)
            {
                if (OnConnected != null) OnConnected(this);
                return;
            }

            m_Host = host;
            m_Port = port;

            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    m_Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    m_Socket.Connect(host, port);
                    m_IsConnect = true;
                    
                    new Thread(new ThreadStart(delegate()
                    {
                        ReceiveString();
                    })).Start();

                    if (OnConnected != null) OnConnected(this);
                }
                catch (Exception ex)
                {
                   
                }

            })).Start();
        }

        public void Disconnect()
        {
            try
            {
                m_IsConnect = false;
                m_Socket.Close();
                if (OnDisconnected != null) OnDisconnected(this);
            }
            catch (Exception ex)
            {
               
            }
        }

        private void ReceiveString()
        {
            while (m_IsConnect)
            {
                try
                {
                    byte[] result = new byte[65536];
                    int receiveLength = m_Socket.Receive(result);
                    if (receiveLength > 0)
                    {
                        new Thread(new ThreadStart(delegate()
                        {
                            if (OnRecvData != null) OnRecvData(this, result.Take(receiveLength).ToArray());

                        })).Start();
                    }
                }
                catch
                {
                    m_IsConnect = false;
                    if (OnDisconnected != null) OnDisconnected(this);
                }
            }
        }

        public void Write(byte[] bytes)
        {
            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    lock (m_Socket)
                    {
                        if (m_IsConnect) m_Socket.Send(bytes);
                    }
                }
                catch
                {
                    m_IsConnect = false;
                    if (OnDisconnected != null) OnDisconnected(this);
                }

            })).Start();
        }
    }
}
