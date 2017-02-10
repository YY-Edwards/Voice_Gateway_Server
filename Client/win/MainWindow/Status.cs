using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.Threading;
using System.Net;
using System.Net.Sockets;

namespace TrboX
{    
    public enum RunMode
    {
        None,
        Radio,
        Repeater,
    }
    public class RunStatus
    {
        public RunMode type;
    };

    public class Status
    {
        private Main m_Main;

        private RunStatus m_RunStatus = new RunStatus();
        private BaseSetting bases;
        private RadioSetting radio;
        private WireLanSetting repeater;

        public Status(Main win)
        {
            if (null == win) return;
            m_Main = win;

            //GetRunMode();

            Thread t = new Thread(() =>
            {
                while (true) { 
                m_Main.Dispatcher.Invoke(new Action(() =>{m_Main.lab_SysTime.Content = DateTime.Now.ToString();}));   
                              
                Thread.Sleep(1000);} });
            t.Start();
        }

        public RunMode GetRunMode()
        {
            radio = new Setting() { Type = SettingType.Radio }.Get() as RadioSetting;
            repeater = new Setting() { Type = SettingType.WireLan }.Get() as WireLanSetting;


            RunMode type = RunMode.None;

            if (repeater != null && repeater.IsEnable)
            {
                m_Main.lab_DeviceSta.Content = "未连中继台";
                type = RunMode.Repeater;
            }


            if (radio != null && radio.IsEnable)
            {
                m_Main.lab_DeviceSta.Content = "未连接调度设备　　未连接数据设备";
                type = RunMode.Radio;
            }



            SetTargetSystemType(type);
            return RunMode.None;
        }

        public RunStatus Get()
        {
            return m_RunStatus;
        }

        public void SetTargetSystemType(RunMode type)
        {
            m_RunStatus.type = type;
            TServer.SystemType = type;
        }
        public void SetConectSta(int sta)
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                m_Main.lab_DeviceSta.Content = "";
                if (m_RunStatus.type == RunMode.Radio)
                {
                    if (radio == null) return;

                    string ip = "";
                    try
                    {
                        byte[] addr = IPAddress.Parse(radio.Ride.Ip).GetAddressBytes();
                        addr[3] = (byte)((addr[3] > 0) ? (addr[3] - 1) : 0);
                        ip = new IPAddress(addr).ToString();
                    }
                    catch { }



                    if ((sta & 2) == 0)
                    {
                        m_Main.lab_DeviceSta.Content = "已连接" + ip + "(调度)";
                    }
                    else
                    {
                        m_Main.lab_DeviceSta.Content = "未连接调度设备";
                    }

                    if ((sta & 1) == 0)
                    {
                        m_Main.lab_DeviceSta.Content += "　　已连接" + radio.Ride.Ip + "(数据)";
                    }
                    else
                    {
                        m_Main.lab_DeviceSta.Content += "　　未连接数据设备";
                    }

                }
                else if (m_RunStatus.type == RunMode.Repeater)
                {
                    if (repeater == null) return;

                    if ((sta & 1) == 0)
                    {
                        m_Main.lab_DeviceSta.Content = "已连接中继台" + repeater.Master.Ip + "：" +repeater.Master.Port.ToString();
                    }
                    else
                    {
                        m_Main.lab_DeviceSta.Content = "未连中继台";
                    }
                }
            }));
        }
    }
}
