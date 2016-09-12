using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
namespace TrboX 
{
    public class MainTool
    {
        private Main m_Main;

        public MainTool(Main win)
        {
            if (null == win) return;
            m_Main = win;


            m_Main.btn_Tool_NewFastOperate.Click += delegate
            {
                FastOperate op = new FastOperate()
                {
                    m_Type = FastType.FastType_Operate,
                    //m_Contact = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                    //    , new CEmployee() { id = 2, name = "崔二胯子" }
                    //    , new CVehicle() { id = 12, number = "牛B74110" }
                    //    , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),

                    m_Operate = new COperate(OPType.Dispatch, new CDispatch()
                    {
                        target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                            , new CEmployee() { id = 2, name = "崔二胯子" }
                            , new CVehicle() { id = 12, number = "牛B74110" }
                            , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),
                        targettype = TargetType.Radio
                    })

                    //m_Operate = new COperate(OPType.ShortMessage, new CShortMessage()
                    //{
                    //    target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                    //        , new CEmployee() { id = 2, name = "崔二胯子" }
                    //        , new CVehicle() { id = 12, number = "牛B74110" }
                    //        , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),
                    //    targettype = TargetType.Radio,
                    //    message = "hello,test msg"
                    //})


                    //m_Operate = new COperate(OPType.Position, new CPosition()
                    //{
                    //    target = new CRelationShipObj(OrgItemType.Type_Employee, new CGroup() { id = 1, name = "地勤", group_id = 12 }
                    //        , new CEmployee() { id = 2, name = "崔二胯子" }
                    //        , new CVehicle() { id = 12, number = "牛B74110" }
                    //        , new CRadio() { id = 22, radio_id = 2314, is_online = true, type = RadioType.RIDE }),
                    //    targettype = TargetType.Radio,
                    //    iscsbk = true,
                    //    isenh = false,
                    //    iscycle = false,
                    //})
                };
                m_Main.WorkArea.FastPanel.Add(op);
            };
        }

        public void OperateShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Base.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Base.Visibility = Visibility.Collapsed;
        }

        public void OperateEnable(bool disable = false)
        {
            if (false == disable)
                m_Main.bdr_Tool_Base.IsEnabled = true;
            else
                m_Main.bdr_Tool_Base.IsEnabled = false;
        }

        public void FastShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Fast.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Fast.Visibility = Visibility.Collapsed;
        }

        public void ControlShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Ctrl.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Ctrl.Visibility = Visibility.Collapsed;
        }

        public void ControlEnable(bool disable = false)
        {
            if (false == disable)
                m_Main.bdr_Tool_Ctrl.IsEnabled = true;
            else
                m_Main.bdr_Tool_Ctrl.IsEnabled = false;
        }

        public void HelpShow(bool hide = false)
        {
            if (false == hide)
                m_Main.bdr_Tool_Help.Visibility = Visibility.Visible;
            else
                m_Main.bdr_Tool_Help.Visibility = Visibility.Collapsed;
        }
    }
}
