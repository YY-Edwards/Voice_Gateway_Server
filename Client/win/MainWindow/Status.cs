using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.Threading;

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
        public TargetSystemType type;
    };

    public class Status
    {
        private Main m_Main;

        private RunStatus m_RunStatus = new RunStatus();

        public Status(Main win)
        {
            if (null == win) return;
            m_Main = win;

            GetRunMode();


            Thread t = new Thread(() =>
            {
                while (true) { 
                m_Main.Dispatcher.Invoke(new Action(() =>{m_Main.lab_SysTime.Content = DateTime.Now.ToString();}));   
                              
                Thread.Sleep(1000);} });
            t.Start();
        }


        public RunMode GetRunMode()
        {
            RadioSetting radio = new Setting() { Type = SettingType.Radio }.Get() as RadioSetting;
            WireLanSetting repeater = new Setting() { Type = SettingType.WireLan }.Get() as WireLanSetting;  

            if (radio != null && radio.IsEnable) 
            {
                if (radio.IsOnlyRide) m_Main.lab_DeviceSta.Content = "车载台：" + radio.Ride.Ip;
                else m_Main.lab_DeviceSta.Content = "车载台：" + radio.Ride.Ip + "　MNIS：" +  radio.Mnis.Ip;
                
                return RunMode.Radio;
            }


            if (repeater != null && repeater.IsEnable)
            {
                m_Main.lab_DeviceSta.Content = "中继台：" + repeater.Master.Ip + "：" + repeater.Master.Port.ToString();
                return RunMode.Repeater;
            }

            return RunMode.None;
        }

        public RunStatus Get()
        {
            return m_RunStatus;
        }

        public void SetTargetSystemType(TargetSystemType type)
        {
            m_RunStatus.type = type;
            //m_Main.Rpc.SetType(type);
        }
    }
}
