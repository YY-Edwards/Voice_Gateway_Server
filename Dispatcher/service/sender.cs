using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;
using System.Windows.Threading;
using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class CSender
    {
        public delegate void TimeoutHandle(object obj, int sequence);
        public event TimeoutHandle OnTimeout;

        private Thread m_TimoutThread;

        private struct SenderStru
        {
            public int TimeoutMS;
            public long TimeoutTicks;
            public int SendTimes;
            public WriteHandel WriteMethod;
        }
        private static Dictionary<long, SenderStru> CmdLst = new Dictionary<long, SenderStru>();

        public CSender()
        {
            if (m_TimoutThread == null)
            {
                m_TimoutThread = new Thread(new ThreadStart(delegate()
                {
                    while (true)
                    {
                        Thread.Sleep(100);
                        OnTimer100ms();
                    }
                }));
                m_TimoutThread.Start();

            }
        }

        private void OnTimer100ms()
        {
            try
            {
                List<long> timeoutlst = new List<long>();
                lock (CmdLst)
                {
                    foreach (var item in CmdLst)
                    {
                        if (DateTime.Now.Ticks >= item.Value.TimeoutTicks) timeoutlst.Add(item.Key);
                    }

                    foreach (int seq in timeoutlst)
                    {
                        try
                        {
                            if (CmdLst[seq].SendTimes > 0)
                            {
                                CmdLst[seq].WriteMethod();

                                SenderStru stru = CmdLst[seq];
                                stru.TimeoutTicks = DateTime.Now.AddMilliseconds(CmdLst[seq].TimeoutMS).Ticks;
                                stru.SendTimes -= 1;

                                CmdLst[seq] = stru;

                                Console.Write("Resend Bytes(Sequence:" + seq.ToString() + ")\r\n");
                            }
                            else
                            {
                                if (OnTimeout != null)
                                {
                                    OnTimeout(this, seq);
                                }
                                CmdLst.Remove(seq);
                            }
                        }
                        catch
                        {

                        }
                    }

                }
            }
            catch
            {

            }
        }


        public delegate void WriteHandel();
        public void Begin(long seq, int ms, int re, WriteHandel writemethod)
        {
            try
            {
                lock (CmdLst)
                {
                    if (CmdLst.ContainsKey(seq))
                    {
                        CmdLst.Remove(seq);
                    }

                    if (writemethod != null) writemethod();

                    SenderStru stru = new SenderStru();
                    stru.TimeoutMS = ms;
                    stru.TimeoutTicks = DateTime.Now.AddMilliseconds(ms).Ticks;
                    stru.SendTimes = re - 1;
                    stru.WriteMethod = writemethod;

                    CmdLst.Add(seq, stru);
                }
            }
            catch
            {

            }
        }

        public void End(long seq)
        {
            try
            {
                lock (CmdLst)
                {
                    if (CmdLst.ContainsKey(seq))
                    {
                        CmdLst.Remove(seq);
                    }
                }
            }
            catch
            {

            }
        }
    }
}
