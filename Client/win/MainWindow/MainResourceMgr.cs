﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Threading;

namespace TrboX
{

    public class MainResourceMgr
    {
        public TargetMgr Target = new TargetMgr();

        private Main m_Main;   
        private CTargetRes m_TargetList;

        public MainResourceMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;

            Target.UpdateTragetList();
            UpdateView();

            Thread t = new Thread(() => { NoficationThread(); });
            t.Start();

        }

        private enum ContextMenuType
        {
            All,
            Group,
            RadioOn,
            RadioOff,
        };

        private ContextMenu CreateOrgMenu(ContextMenuType type)
        {
            ContextMenu menu = new ContextMenu();
            if (ContextMenuType.All != type) menu.Items.Add(new MenuItem() { Header = "添加到快速操作", Width = 160, Tag = "fast", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            if (ContextMenuType.All != type) menu.Items.Add(new Separator() { Margin = new Thickness(5) });

            menu.Items.Add(new MenuItem() { Header = "在线检测", Width = 160, Tag = "check", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });

            if ((ContextMenuType.Group == type) || (ContextMenuType.RadioOn == type))
                menu.Items.Add(new MenuItem() { Header = "远程监听", Width = 160, Tag = "monitor", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });

             if (ContextMenuType.RadioOff != type)
             {
                 menu.Items.Add(new Separator() { Margin = new Thickness(5) });
                 menu.Items.Add(new MenuItem() { Header = "语音调度", Width = 160, Tag = "dispatch", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                 menu.Items.Add(new MenuItem() { Header = "短消息", Width = 160, Tag = "message", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                 menu.Items.Add(new MenuItem() { Header = "位置查询", Width = 160, Tag = "position", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                 menu.Items.Add(new MenuItem() { Header = "指令控制", Width = 160, Tag = "control", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                 menu.Items.Add(new Separator() { Margin = new Thickness(5) });
                 menu.Items.Add(new MenuItem() { Header = "工单", Width = 160, Tag = "jobticker", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
             }

            foreach(var item in menu.Items)if(item is MenuItem)((MenuItem)item).Click += new RoutedEventHandler(OnOrganizationMenu_Click);  
            
            return menu;
        }

        private void FillDataToOrgTreeView()
        {
            TreeViewItem OrginItem = m_Main.tree_OrgView.Items.Count > 0 ? m_Main.tree_OrgView.Items[0] as TreeViewItem : null;

            TreeViewItem itemOrg = new TreeViewItem() { Header = "用户/设备" };
            itemOrg.Style = App.Current.Resources["TreeViewItemRoot"] as Style;

            if (null != OrginItem) itemOrg.IsExpanded = OrginItem.IsExpanded;
            else itemOrg.IsExpanded = true;

            itemOrg.ContextMenu = CreateOrgMenu(ContextMenuType.All);
            if (null == itemOrg) return;

            int groupcount = 0;
            List<int> radio_lst = new List<int>();
            foreach (var group in m_TargetList.Group)
            {
                TreeViewItem itemGroup = new TreeViewItem() { Header = group.Value.Name};
                itemGroup.Style = App.Current.Resources["TreeViewItemStyleGroup"] as Style;

                if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    itemGroup.IsExpanded = ((TreeViewItem)OrginItem.Items[groupcount]).IsExpanded;
                else itemGroup.IsExpanded = true;

                itemGroup.Tag = group.Value;
                itemGroup.ContextMenu = CreateOrgMenu(ContextMenuType.Group);
                if (-1 == group.Value.Group.GroupID) itemGroup.ContextMenu.Visibility = Visibility.Hidden;
                else if ("" != group.Value.Information)
                {
                    itemGroup.ToolTip = group.Value.Information;
                }

                int itemcount = 0;
               
                foreach(var employee in m_TargetList.Employee)
                {
                    if (group.Value.Group.ID != employee.Value.Group.ID) continue;
                    if (null != employee.Value.Radio) radio_lst.Add(employee.Value.Radio.RadioID);

                    TreeViewItem childitem = new TreeViewItem();
                    childitem.Header = employee.Value.Name;
                    childitem.ToolTip = employee.Value.Name + (("" == employee.Value.InformationWithoutGroup) ? "" : "：") + employee.Value.InformationWithoutGroup;
                    childitem.Style = App.Current.Resources["TreeViewItemStyleMember"] as Style;
                    childitem.Tag = employee.Value;

                    if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    {
                        if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[itemcount]).Items.Count > itemcount))
                        {
                            if (true == ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsFocused)childitem.Focus();
                            childitem.IsSelected = ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsSelected;
                        }
                    }

                    if (true == childitem.IsSelected) 
                    {
                        m_Main.CurrentTraget = new CMultMember() { Type = SelectionType.Single, Target = new List<CMember>() };
                        m_Main.CurrentTraget.Target.Add(employee.Value);
                    }


                    if ((null == employee.Value.Radio) || (false == employee.Value.Radio.IsOnline))childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);                    
                    else childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);
                  
                    itemGroup.Items.Add(childitem);
                    itemcount++;
                }

                foreach (var vehicle in m_TargetList.Vehicle)
                {
                    if (group.Value.Group.ID != vehicle.Value.Group.ID) continue;
                    if (null != vehicle.Value.Radio) radio_lst.Add(vehicle.Value.Radio.RadioID);

                    TreeViewItem childitem = new TreeViewItem();
                    childitem.Header = vehicle.Value.Name;
                    childitem.ToolTip = vehicle.Value.Name + (("" == vehicle.Value.InformationWithoutGroup) ? "" : "：") + vehicle.Value.InformationWithoutGroup;
                    childitem.Style = App.Current.Resources["TreeViewItemStyleMember"] as Style;
                    childitem.Tag = vehicle.Value;

                    if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    {
                        if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[itemcount]).Items.Count > itemcount))
                        {
                            if (true == ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsFocused) childitem.Focus();
                            childitem.IsSelected = ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsSelected;
                        }
                    }

                    if (true == childitem.IsSelected)
                    {
                        m_Main.CurrentTraget = new CMultMember() { Type = SelectionType.Single, Target = new List<CMember>() };
                        m_Main.CurrentTraget.Target.Add(vehicle.Value);
                    }


                    if ((null == vehicle.Value.Radio) || (false == vehicle.Value.Radio.IsOnline)) childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    itemGroup.Items.Add(childitem);
                    itemcount++;
                }

                foreach (var radio in m_TargetList.Radio)
                {
                    if (group.Value.Group.ID != radio.Value.Group.ID) continue;
                    if(radio_lst.Contains(radio.Value.Radio.RadioID))continue;

                    TreeViewItem childitem = new TreeViewItem();
                    childitem.Header = radio.Value.Name;
                    childitem.ToolTip = radio.Value.Name + (("" == radio.Value.InformationWithoutGroup) ? "" : "：") + radio.Value.InformationWithoutGroup;
                    childitem.Style = App.Current.Resources["TreeViewItemStyleMember"] as Style;
                    childitem.Tag = radio.Value;

                    if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    {
                        if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[itemcount]).Items.Count > itemcount))
                        {
                            if (true == ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsFocused) childitem.Focus();
                            childitem.IsSelected = ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsSelected;
                        }
                    }

                    if (true == childitem.IsSelected)
                    {
                        m_Main.CurrentTraget = new CMultMember() { Type = SelectionType.Single, Target = new List<CMember>() };
                        m_Main.CurrentTraget.Target.Add(radio.Value);
                    }


                    if (false == radio.Value.Radio.IsOnline) childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    itemGroup.Items.Add(childitem);
                    itemcount++;
                }

                itemOrg.Items.Add(itemGroup);
                groupcount++;
            }
            if (null != m_Main.tree_OrgView)
            {
                m_Main.tree_OrgView.Items.Clear();
                m_Main.tree_OrgView.Items.Add(itemOrg);
            }

            m_Main.tree_OrgView.SelectedItemChanged += delegate(object sender, RoutedPropertyChangedEventArgs<object> e)
            {
                if (null == ((TreeView)sender).SelectedItem) return;

                m_Main.CurrentTraget = new CMultMember() { Target = new List<CMember>() };

                CMember target = ((TreeViewItem)((TreeView)sender).SelectedItem).Tag as CMember;

                if (null == target) //All
                {
                    m_Main.CurrentTraget.Type = SelectionType.All;
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;
                }
                else if (MemberType.Group == target.Type) //Group
                {
                    m_Main.CurrentTraget.Type = SelectionType.Single;
                    m_Main.CurrentTraget.Target.Add(target);
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;
                }
                else//radio
                {

                    m_Main.CurrentTraget.Type = SelectionType.Single;
                    m_Main.CurrentTraget.Target.Add(target);
                    if(null == target.Radio)
                    {
                        m_Main.bdr_Tool_Base.IsEnabled = false;
                        m_Main.bdr_Tool_Ctrl.IsEnabled = false;
                        m_Main.menu_Target.Visibility = Visibility.Collapsed;
                    }
                    else if(false == target.Radio.IsOnline)
                    {
                        m_Main.bdr_Tool_Base.IsEnabled = false;
                        m_Main.bdr_Tool_Ctrl.IsEnabled = false;

                        m_Main.menu_Target.Visibility = Visibility.Visible;
                        //monitor
                        m_Main.menu_Target_Monitor.IsEnabled = false;

                        //call
                        m_Main.menu_Target_Call.IsEnabled = false;

                        //message
                        m_Main.menu_Target_Message.IsEnabled = false;

                        //position
                        m_Main.menu_Target_Position.IsEnabled = false;
                        m_Main.menu_Target_PositionCycle.IsEnabled = false;
                        m_Main.menu_Target_PositionCSBK.IsEnabled = false;
                        m_Main.menu_Target_PositionCSBKCycle.IsEnabled = false;
                        m_Main.menu_Target_PositionEnh.IsEnabled = false;
                        m_Main.menu_Target_PositionEnhCycle.IsEnabled = false;
                        m_Main.menu_Target_Trail.IsEnabled = false;

                        //control
                        m_Main.menu_Target_StartUp.IsEnabled = false;
                        m_Main.menu_Target_Shut.IsEnabled = false;
                        m_Main.menu_Target_Sleep.IsEnabled = false;
                        m_Main.menu_Target_Week.IsEnabled = false;

                        //job ticket
                        m_Main.menu_Target_JobTicket.IsEnabled = false;
                    }
                    else{
                        m_Main.bdr_Tool_Base.IsEnabled = true;
                        m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                        m_Main.menu_Target.Visibility = Visibility.Visible;
                    }
                }


            };
            m_Main.tree_OrgView.PreviewMouseDoubleClick += delegate(object sender, MouseButtonEventArgs e)
            {

            };
        }

        private void OnOrganizationMenu_Click(object sender, RoutedEventArgs e)
        {
            CMember org = null;
            

            if (((ContextMenu)((MenuItem)sender).Parent).PlacementTarget is TreeViewItem)
            {
                org = ((TreeViewItem)((ContextMenu)((MenuItem)sender).Parent).PlacementTarget).Tag as CMember;               
            }
            else if (((ContextMenu)((MenuItem)sender).Parent).PlacementTarget is ListBoxItem)
            {
                org = ((ListBoxItem)((ContextMenu)((MenuItem)sender).Parent).PlacementTarget).Content as CMember;
            }


            CMultMember trg = new CMultMember() { Type = (null == org) ? SelectionType.All : SelectionType.Single, Target = new List<CMember>() };
            trg.Target.Add(org);

            switch ((string)((MenuItem)sender).Tag)
            {
                case "fast":
                    m_Main.WorkArea.FastPanel.Add(new FastOperate()
                    {
                        Type = FastType.FastType_Contact,
                        Contact = trg
                    });
                    break;
                case "dispatch":
                    m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Dispatch, trg, null));
                    break;
                case "message":
                    m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.ShortMessage, trg, null));
                    break;
                case "position":
                    m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.Position, trg, null));
                    break;
                case "control":
                    break;
                case "jobticker":
                    m_Main.SubWindow.OpenOrCreateTragetWin(new COperate(OPType.JobTicker, trg, null));
                    break;
            };
        }

        private void FillDataToGroupList()
        {
            m_Main.lst_Group.View = (ViewBase)m_Main.FindResource("GroupView");
            m_Main.lst_Group.Items.Clear();
            foreach (var group in m_TargetList.Group)
            {
                if ((null != group.Value.Group) && (-1 != group.Value.Group.ID))
                {
                    ListViewItem item = new ListViewItem() { Content = group.Value };
                    item.ContextMenu = CreateOrgMenu(ContextMenuType.Group);
                    m_Main.lst_Group.Items.Add(item);
                }
            }          
        }

        private void FillDataToEmployeeList()
        {
            m_Main.lst_Employee.View = (ViewBase)m_Main.FindResource("EmployeeView");
            m_Main.lst_Employee.Items.Clear();
            foreach (var employee in m_TargetList.Employee)
            {
                if (null != employee.Value.Employee)
                {
                    ListViewItem item = new ListViewItem() { Content = employee.Value };
                    if ((null == employee.Value.Radio) || (false == employee.Value.Radio.IsOnline))
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    m_Main.lst_Employee.Items.Add(item);
                }
                
            }
        }
        private void FillDataToVehicleList()
        {
            m_Main.lst_Vehicle.View = (ViewBase)m_Main.FindResource("VehicleView");
            m_Main.lst_Vehicle.Items.Clear();
            foreach (var vehicle in m_TargetList.Vehicle)
            {
                if (null != vehicle.Value.Vehicle)
                {
                    ListViewItem item = new ListViewItem() { Content = vehicle.Value };

                    if ((null == vehicle.Value.Radio) || (false == vehicle.Value.Radio.IsOnline))
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    m_Main.lst_Vehicle.Items.Add(item);
                }
                
            }
        }
        private void FillDataToRadioList()
        {
            m_Main.lst_Radio.View = (ViewBase)m_Main.FindResource("RadioView");
            m_Main.lst_Radio.Items.Clear();
            foreach (var radio in m_TargetList.Radio)
            {
                if (null != radio.Value.Radio)
                {
                    ListViewItem item = new ListViewItem() { Content = radio.Value };
                    if (false == radio.Value.Radio.IsOnline)
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    m_Main.lst_Radio.Items.Add(item);                  
                }               
            }
        }

        private void UpdateView()
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                m_TargetList = Target.TargetList;
                FillDataToOrgTreeView();
                FillDataToGroupList();
                FillDataToEmployeeList();
                FillDataToVehicleList();
                FillDataToRadioList();
            })); 
        }

        private void NoficationThread()
        {
            while(true)
            {
                //foreach (var rad in m_TargetList.Radio)
                //{
                //    if (null != rad.Value.Radio)
                //    {
                //        CRadio radio = new CRadio() { ID = rad.Value.Radio.ID, RadioID = rad.Value.Radio.RadioID, IsOnline = !rad.Value.Radio.IsOnline, Type = rad.Value.Radio.Type };
                //        m_Target.Update(radio);
                //    } 
                //}

                //UpdateView();
                Thread.Sleep(5000);
            }
        }

    }
}
