using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;

using System.Net;
using System.Net.Sockets;

namespace TrboX
{
    public class SettingComponents
    {
        private Main m_Main;


        Dictionary<int, int> m_DefaultGroupCmb = new Dictionary<int, int>();
        
        public SettingComponents(Main main)
        {
            if (null != main) m_Main = main;
        }

        public void Set(Setting setting)
        {
           SetBaseSetting(setting.Base);
           SetRadioSetting(setting.Radio);
           SetWireLanSetting(setting.WireLan);
        }

        public Setting Get()
        {
            Setting setting = new Setting();
            setting.Base = GetBaseSetting();
            setting.Radio = GetRadioSetting();
            setting.WireLan = GetWireLanSetting();

            return setting;
        }

        private BaseSetting GetBaseSetting()
        {    
            BaseSetting  Base = new BaseSetting ();
            try
            {

                Base.Svr = new NetAddress();
                try { 
                    IPAddress.Parse(m_Main.txt_SvrIp.Text);
                    Base.Svr.Ip = m_Main.txt_SvrIp.Text;}
                catch { 
                }

                try { Base.Svr.Port = int.Parse(m_Main.txt_SvrPort.Text); }
                catch{
                }

                Base.LogSvr = new NetAddress();

                try
                {
                    IPAddress.Parse(m_Main.txt_LogSvrIp.Text);
                    Base.LogSvr.Ip = m_Main.txt_LogSvrIp.Text;
                }
                catch
                {
                }

                try { Base.LogSvr.Port = int.Parse(m_Main.txt_LogSvrPort.Text); }
                catch
                {
                }

                Base.IsSaveCallLog = (bool)m_Main.chk_LogType_Call.IsChecked;
                Base.IsSaveMsgLog = (bool)m_Main.chk_LogType_Msg.IsChecked;
                Base.IsSavePositionLog = (bool)m_Main.chk_LogType_Position.IsChecked;
                Base.IsSaveControlLog = (bool)m_Main.chk_LogType_Control.IsChecked;
                Base.IsSaveJobLog = (bool)m_Main.chk_LogType_JobTicket.IsChecked;
                Base.IsSaveTrackerLog = (bool)m_Main.chk_LogType_Tracker.IsChecked;
            }
            catch
            {
                Console.WriteLine("常规输入有误");
            }

            return Base;
        }

        private void SetBaseSetting(BaseSetting Base)
        {
            try
            {
                m_Main.txt_SvrIp.Text = Base.Svr.Ip;
                m_Main.txt_SvrPort.Text = Base.Svr.Port.ToString();

                m_Main.txt_LogSvrIp.Text = Base.LogSvr.Ip;
                m_Main.txt_LogSvrPort.Text = Base.LogSvr.Port.ToString();

                m_Main.chk_LogType_Call.IsChecked = Base.IsSaveCallLog;
                m_Main.chk_LogType_Msg.IsChecked = Base.IsSaveMsgLog;
                m_Main.chk_LogType_Position.IsChecked=Base.IsSavePositionLog;
                m_Main.chk_LogType_Control.IsChecked= Base.IsSaveControlLog;
                m_Main.chk_LogType_JobTicket.IsChecked= Base.IsSaveJobLog;
                m_Main.chk_LogType_Tracker.IsChecked = Base.IsSaveTrackerLog;
            }
            catch
            {
                Console.WriteLine("常规输入有误");
            };
        }

        private RadioSetting GetRadioSetting()
        {
            RadioSetting Radio = new RadioSetting ();
            try{
                Radio.IsEnable = (bool)m_Main.chk_EnableRadio.IsChecked;
                Radio.IsOnlyRide = (bool)m_Main.rad_RadioType.IsChecked;

                Radio.Svr = new NetAddress();
                try
                {
                    IPAddress.Parse(m_Main.txt_RadioSvrIp.Text);
                    Radio.Svr.Ip = m_Main.txt_RadioSvrIp.Text;
                }
                catch
                {

                }

                try
                {
                    Radio.Svr.Port = int.Parse(m_Main.txt_RadioSvrPort.Text);
                }catch
                {

                }

                Radio.Ride = new NetAddress();
                try
                {
                    IPAddress.Parse(m_Main.txt_RadioIp.Text);
                    Radio.Ride.Ip = m_Main.txt_RadioIp.Text;
                }
                catch { }


                Radio.Mnis = new NetAddress();
                try
                {
                    IPAddress.Parse(m_Main.txt_RadioMnisIp.Text);
                    Radio.Mnis.Ip = m_Main.txt_RadioMnisIp.Text;
                }
                catch { }

                Radio.Ars = new NetAddress();
                try
                {
                    Radio.Ars.Port = int.Parse(m_Main.txt_RadioArsPort.Text);
                }
                catch { }
                Radio.Message = new NetAddress();

                try
                {
                    Radio.Message.Port = int.Parse(m_Main.txt_RadioMsgPort.Text);

                }
                catch
                {

                }
                Radio.Gps = new NetAddress();

                try
                {
                    IPAddress.Parse(m_Main.txt_RadioGpsIp.Text);
                    Radio.Gps.Ip = m_Main.txt_RadioGpsIp.Text;
                }
                catch { }
            }
            catch
            {
                
            }

            return Radio;
        }

        private void SetRadioSetting(RadioSetting Radio)
        {
            try
            {
                m_Main.chk_EnableRadio.IsChecked = Radio.IsEnable;

                m_Main.rad_RadioType.IsChecked = Radio.IsOnlyRide;
                m_Main.rad_RadioMnisType.IsChecked = !Radio.IsOnlyRide;

                m_Main.txt_RadioSvrIp.Text = Radio.Svr.Ip;
                m_Main.txt_RadioSvrPort.Text = Radio.Svr.Port.ToString();

                m_Main.txt_RadioIp.Text = Radio.Ride.Ip;
                m_Main.txt_RadioMnisIp.Text = Radio.Mnis.Ip;

                m_Main.txt_RadioArsPort.Text = Radio.Ars.Port.ToString();
                m_Main.txt_RadioMsgPort.Text = Radio.Message.Port.ToString();

                m_Main.txt_RadioGpsIp.Text = Radio.Mnis.Ip;
            }
            catch
            {
                
            }
        }

        private WireLanSetting GetWireLanSetting()
        {
            WireLanSetting WireLan = new WireLanSetting();
            try
            {
                WireLan.IsEnable = (bool)m_Main.chk_EnableWireLan.IsChecked;
                WireLan.Type = (bool)m_Main.rad_IPSCType.IsChecked ? WireLanType.IPSC : ((bool)m_Main.rad_CPCType.IsChecked ? WireLanType.CPC : WireLanType.LCP);

                try
                {
                    WireLan.DefaultGroupId = ((CGroup)((ComboBoxItem)m_Main.cmb_DefaultGroup.SelectedItem).Tag).GroupID;
                }
                catch { }

                try
                {
                    WireLan.DefaultChannel = m_Main.cmb_DefaultChannel.SelectedIndex + 1;

                }
                catch { }
                

                try
                {
                    WireLan.MinHungTime = int.Parse(m_Main.txt_MinHungTime.Text);
                }
                catch { }

                try
                {
                    WireLan.MaxSiteAliveTime = int.Parse(m_Main.txt_MaxMasterAliveTime.Text);
                }
                catch { }
                try
                {
                    WireLan.MaxPeerAliveTime = int.Parse(m_Main.txt_MaxPeerAliveTime.Text);
                }
                catch { }

                WireLan.Svr = new NetAddress();
                try
                {
                    IPAddress.Parse(m_Main.txt_WireLanSvrIp.Text);
                    WireLan.Svr.Ip = m_Main.txt_WireLanSvrIp.Text;
                }
                catch { }

                try
                {
                    WireLan.Svr.Port = int.Parse(m_Main.txt_WireLanSvrPort.Text);
                }
                catch { }

                WireLan.Master = new NetAddress();
                try
                {
                    IPAddress.Parse(m_Main.txt_SiteIp.Text);
                    WireLan.Master.Ip = m_Main.txt_SiteIp.Text;
                }
                catch { }

                try
                {
                    WireLan.Master.Port = int.Parse(m_Main.txt_SitePort.Text);
                }

                catch { }

                try
                {
                    WireLan.LocalPeerId = int.Parse(m_Main.txt_LocalPeerId.Text);
                }
                catch { }

                try
                {
                     WireLan.LocalRadioId = int.Parse(m_Main.txt_LocalRadioId.Text);
                }
                catch { }

                WireLan.Dongle.Com = m_Main.cmb_DongleCom.SelectedIndex + 1;
            }
            catch
            {
                Console.WriteLine("WireLan设置输出错误");
            }

            return WireLan;
        }

        public void FileGroupList(Dictionary<int, CGroup> lst)
        {
            m_Main.cmb_DefaultGroup.Items.Clear();
            m_DefaultGroupCmb.Clear();

            foreach(var group in lst)
            {
                if(group.Value.GroupID < 0)continue;
                int index = m_Main.cmb_DefaultGroup.Items.Add(new ComboBoxItem()
                {
                    Content = group.Value.Name + "(ID：" + group.Value.GroupID.ToString() + ")",
                    Tag = group.Value
                });
                m_DefaultGroupCmb.Add(group.Value.GroupID,index);
            }
        }

        private void SetWireLanSetting(WireLanSetting WireLan)
        {
            try
            {
                m_Main.chk_EnableWireLan.IsChecked = WireLan.IsEnable;
                m_Main.rad_IPSCType.IsChecked = (WireLanType.IPSC == WireLan.Type)? true : false;
                m_Main.rad_CPCType.IsChecked = (WireLanType.CPC == WireLan.Type) ? true : false;
                m_Main.rad_LCPType.IsChecked = (WireLanType.LCP == WireLan.Type)? true : false;

                m_Main.cmb_DefaultGroup.SelectedIndex = m_DefaultGroupCmb.ContainsKey(WireLan.DefaultGroupId) ? m_DefaultGroupCmb[WireLan.DefaultGroupId] : -1;
                m_Main.cmb_DefaultChannel.SelectedIndex = WireLan.DefaultChannel - 1;

                m_Main.txt_MinHungTime.Text = WireLan.MinHungTime.ToString();
                m_Main.txt_MaxMasterAliveTime.Text = WireLan.MaxSiteAliveTime.ToString();
                m_Main.txt_MaxPeerAliveTime.Text = WireLan.MaxPeerAliveTime.ToString();

                m_Main.txt_WireLanSvrIp.Text = WireLan.Svr.Ip;
                m_Main.txt_WireLanSvrPort.Text = WireLan.Svr.Port.ToString();

                m_Main.txt_SiteIp.Text = WireLan.Master.Ip;
                m_Main.txt_SitePort.Text = WireLan.Master.Port.ToString();

                m_Main.txt_LocalPeerId.Text = WireLan.LocalPeerId.ToString();
                m_Main.txt_LocalRadioId.Text = WireLan.LocalRadioId.ToString();

                m_Main.cmb_DongleCom.SelectedIndex = WireLan.Dongle.Com - 1;
            }
            catch
            {
                Console.WriteLine("WireLan设置输出错误");
            }
        }
    }

}
