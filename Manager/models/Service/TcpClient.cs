using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net.Sockets;
using System.Threading;

//using Sigmar.Logger;

namespace Manager.Models
{
    public class CTcpClient
    {
        public bool IsConnect { get; private set; }
        public string Host { get; private set; }
        public int Port{ get; private set; }


        public event Action<object, bool> StatusChanged;
        public event Action<object, byte[]> ReceivedBytes;

        private Socket _Socket;


        public CTcpClient()
        {
            IsConnect = false;
            Host = "";
            Port = 0;
        }

        public void Connect(string host, int port)
        {
            if (Host == host && Port == port && IsConnect)
            {
                if (StatusChanged != null) StatusChanged(this, IsConnect);
                return;
            }

            Host = host;
            Port = port;

            new Task(() => {
                try
                {
                    _Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    _Socket.Connect(Host, Port);
                    

                    new Task(ReceiveString).Start();
                    IsConnect = true;
                }
                catch (Exception ex)
                {
                    IsConnect = false;
                }

                if (StatusChanged != null) StatusChanged(this, IsConnect);
            
            }).Start();
        }


        public void Disconnect()
        {
            if (!IsConnect || _Socket == null)
            {
                if (StatusChanged != null) StatusChanged(this, IsConnect);
                return;
            }


            try
            {               
                _Socket.Close();             
            }
            catch (Exception ex)
            {

            }
            finally
            {
                IsConnect = false;
                if (StatusChanged != null) StatusChanged(this, IsConnect);
            }
        }

        public void Write(byte[] bytes)
        {
            if (!IsConnect || _Socket == null) return;

            new Task(() => {

                try
                {
                    lock (_Socket)
                    {
                        _Socket.Send(bytes);
                        //Log.Report(ReportType_t.Json, Encoding.UTF8.GetString(bytes), true, "127.0.0.1", Host + ":" + Port.ToString());
                    }
                }
                catch
                {
                    IsConnect = false;
                    if (StatusChanged != null) StatusChanged(this, IsConnect);
                }
            }).Start();
        }

        private void ReceiveString()
        {
            while (IsConnect && _Socket != null)
            {
                try
                {
                    byte[] rxbytes = new byte[65536];
                    int receiveLength = _Socket.Receive(rxbytes);
                    if (receiveLength > 0)
                    {
                       rxbytes = rxbytes.Take(receiveLength).ToArray();

                        new Task(() => {
                            //Log.Report(ReportType_t.Json, Encoding.UTF8.GetString(rxbytes), false, Host + ":" + Port.ToString(), "127.0.0.1");

                            if (ReceivedBytes != null) ReceivedBytes(this, rxbytes);
                        }).Start();
                        new Thread(new ThreadStart(delegate()
                        {

                        })).Start();
                    }
                }
                catch
                {
                    IsConnect = false;
                    if (StatusChanged != null) StatusChanged(this, IsConnect);
                }
            }
        }
    }
}
