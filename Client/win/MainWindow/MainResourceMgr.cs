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
        private int m_LastNavIndex = -1;

        public MainResourceMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;

            Target.UpdateTragetList();

            UpdateView();
            m_LastNavIndex = m_Main.tab_Mgr.SelectedIndex;

            m_Main.tab_Mgr.SelectionChanged += delegate {
                if (m_LastNavIndex != m_Main.tab_Mgr.SelectedIndex) { m_LastNavIndex = m_Main.tab_Mgr.SelectedIndex;
                UpdateView();
                }
               };
            m_Main.btn_ResSearch.Click += delegate { 
                UpdateView(); };


            m_Main.tree_OrgView.PreviewMouseDoubleClick += delegate { };
            m_Main.tree_OrgView.PreviewMouseRightButtonDown += delegate(object sender, MouseButtonEventArgs e) { OnTreeRightClick(sender, e); };
            m_Main.lst_Employee.PreviewMouseDoubleClick += delegate{ };
            m_Main.lst_Vehicle.PreviewMouseDoubleClick += delegate { };
            m_Main.lst_Group.PreviewMouseDoubleClick += delegate { };
            m_Main.lst_Radio.PreviewMouseDoubleClick += delegate { };



            Thread t = new Thread(() => { ResourceUpdateThread(); });
            t.Start();


            ResourceMgr ResMgr = new ResourceMgr();
            ResMgr.Get();
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
            ContextMenu menu = new ContextMenu() { Style = App.Current.Resources["ContextMenuStyleNormal"] as Style };
            if (ContextMenuType.All != type) menu.Items.Add(new MenuItem() { Header = "添加到快速操作", Height = 28, HorizontalAlignment = HorizontalAlignment.Stretch, FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Tag = "fast", Style = App.Current.Resources["MenuItemStyleSub"] as Style });
            if (ContextMenuType.All != type) menu.Items.Add(new Separator() { Margin = new Thickness(1, 0, 1, 0), Height = 16, Style = App.Current.Resources["SeparatorStyleNormal"] as Style });


            if (m_Main.StatusBar.Get().type != RunMode.Repeater)
            {
                menu.Items.Add(new MenuItem() { Header = "在线检测", Height = 28, Tag = "check", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });

                //if ((ContextMenuType.Group == type) || (ContextMenuType.RadioOn == type))
                menu.Items.Add(new MenuItem() { Header = "远程监听", Height = 28, Tag = "monitor", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
                menu.Items.Add(new Separator() { Margin = new Thickness(1, 0, 1, 0), Height = 16, Style = App.Current.Resources["SeparatorStyleNormal"] as Style });
            }
             //if (ContextMenuType.RadioOff != type)
             //{
                
                 menu.Items.Add(new MenuItem() { Header = "语音调度", Height = 28, Tag = "dispatch", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
                 menu.Items.Add(new MenuItem() { Header = "短消息", Height = 28, Tag = "message", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
                 menu.Items.Add(new MenuItem() { Header = "位置查询", Height = 28, Tag = "position", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
                
            if (m_Main.StatusBar.Get().type != RunMode.Repeater)   
            menu.Items.Add(new MenuItem() { Header = "指令控制", Height = 28, Tag = "control", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
                 //menu.Items.Add(new Separator() { Margin = new Thickness(1, 0, 1, 0), Height = 16, Style = App.Current.Resources["SeparatorStyleNormal"] as Style });
                 //menu.Items.Add(new MenuItem() { Header = "工单", Height = 28, Tag = "jobticker", FontSize = 14, FontFamily = new FontFamily("Hiragino Sans GB W3"), Foreground = new SolidColorBrush(Colors.White), Style = App.Current.Resources["MenuItemStyle1"] as Style });
             //}

            foreach(var item in menu.Items)if(item is MenuItem)((MenuItem)item).Click += new RoutedEventHandler(OnOrganizationMenu_Click);  
            
            return menu;
        }

        private void FillDataToOrgTreeView()
        {
            TreeViewItem OrginItem = m_Main.tree_OrgView.Items.Count > 0 ? m_Main.tree_OrgView.Items[0] as TreeViewItem : null;

            TreeViewItem itemOrg = new TreeViewItem() { Header = "用户/设备" };
            itemOrg.Style = App.Current.Resources["TreeViewItemStyleRoot"] as Style;

            if (null != OrginItem) itemOrg.IsExpanded = OrginItem.IsExpanded;
            else itemOrg.IsExpanded = true;

            itemOrg.ContextMenu = CreateOrgMenu(ContextMenuType.All);
            if (null == itemOrg) return;

            int groupcount = 0;
            List<long> radio_lst = new List<long>();
            if (null != m_TargetList.Group)
            foreach (var group in m_TargetList.Group)
            {
                TreeViewItem itemGroup = new TreeViewItem() { Header = group.Value.Name};
                itemGroup.Style = App.Current.Resources["TreeViewItemStyle2nd"] as Style;

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
                if (null != m_TargetList.Staff)
                    foreach (var staff in m_TargetList.Staff)
                {
                    if (group.Value.Group.ID != staff.Value.Group.ID) continue;
                    if (null != staff.Value.Radio) radio_lst.Add(staff.Value.Radio.RadioID);

                    TreeViewItem childitem = new TreeViewItem();
                    childitem.Header = staff.Value.Name;
                    childitem.ToolTip = staff.Value.Name + (("" == staff.Value.InformationWithoutGroup) ? "" : "：") + staff.Value.InformationWithoutGroup;
                    childitem.Style = App.Current.Resources["TreeViewItemStyle3rd"] as Style;
                    childitem.Tag = staff.Value;

                    if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    {
                        if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[groupcount]).Items.Count > itemcount))
                        {
                            if (true == ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsFocused)childitem.Focus();
                            childitem.IsSelected = ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsSelected;
                        }
                    }

                    if (true == childitem.IsSelected) 
                    {
                        m_Main.CurrentTraget = new CMultMember() { Type = SelectionType.Single, Target = new List<CMember>() };
                        m_Main.CurrentTraget.Target.Add(staff.Value);
                    }


                    if ((null == staff.Value.Radio) || (false == staff.Value.Radio.IsOnline)) childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);                    
                    else childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);
                  
                    itemGroup.Items.Add(childitem);
                    itemcount++;
                }

                if (null != m_TargetList.Radio)
                foreach (var radio in m_TargetList.Radio)
                {
                    if (group.Value.Group.ID != radio.Value.Group.ID) continue;
                    if(radio_lst.Contains(radio.Value.Radio.RadioID))continue;

                    TreeViewItem childitem = new TreeViewItem();
                    childitem.Header = radio.Value.Name;
                    childitem.ToolTip = radio.Value.Name + (("" == radio.Value.InformationWithoutGroup) ? "" : "：") + radio.Value.InformationWithoutGroup;
                    childitem.Style = App.Current.Resources["TreeViewItemStyle3rd"] as Style;
                    childitem.Tag = radio.Value;

                    if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                    {
                        if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[groupcount]).Items.Count > itemcount))
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
                    //}
                    //else if(false == target.Radio.IsOnline)
                    //{
                    //    m_Main.bdr_Tool_Base.IsEnabled = false;
                    //    m_Main.bdr_Tool_Ctrl.IsEnabled = false;

                    //    m_Main.menu_Target.Visibility = Visibility.Visible;
                    //    //monitor
                    //    m_Main.menu_Target_Monitor.IsEnabled = false;

                    //    //call
                    //    m_Main.menu_Target_Call.IsEnabled = false;

                    //    //message
                    //    m_Main.menu_Target_Message.IsEnabled = false;

                    //    //position
                    //    m_Main.menu_Target_Position.IsEnabled = false;
                    //    m_Main.menu_Target_PositionCycle.IsEnabled = false;
                    //    m_Main.menu_Target_PositionCSBK.IsEnabled = false;
                    //    m_Main.menu_Target_PositionCSBKCycle.IsEnabled = false;
                    //    m_Main.menu_Target_PositionEnh.IsEnabled = false;
                    //    m_Main.menu_Target_PositionEnhCycle.IsEnabled = false;
                    //    m_Main.menu_Target_Trail.IsEnabled = false;

                    //    //control
                    //    m_Main.menu_Target_StartUp.IsEnabled = false;
                    //    m_Main.menu_Target_Shut.IsEnabled = false;
                    //    m_Main.menu_Target_Sleep.IsEnabled = false;
                    //    m_Main.menu_Target_Week.IsEnabled = false;

                    //    //job ticket
                    //    m_Main.menu_Target_JobTicket.IsEnabled = false;
                    }
                    else
                    {
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
                case "check":
                    new COperate(OPType.Control, trg, new CControl(){Type = ControlType.Check}).Exec();
                    m_Main.EventList.AddEvent("提示：在线检测（" + trg.NameInfo  + ")");
                    break;
                case "monitor":
                    new COperate(OPType.Control, trg, new CControl() { Type = ControlType.Monitor }).Exec();
                    m_Main.EventList.AddEvent("提示：远程监听（" + trg.NameInfo + ")");
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
            m_Main.lst_Group.Items.Clear();
            m_Main.lst_Group.UpdateLayout();
            if (null != m_TargetList.Group)
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
            m_Main.lst_Employee.Items.Clear();

            if (null != m_TargetList.Staff)
            foreach (var staff in m_TargetList.Staff)
            {
                if (null != staff.Value.Staff)
                {
                    if (StaffType.Vehicle == staff.Value.Staff.Type) continue;

                    ListViewItem item = new ListViewItem() { Content = staff.Value };
                    if ((null == staff.Value.Radio) || (false == staff.Value.Radio.IsOnline))
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                    else
                        item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                    m_Main.lst_Employee.Items.Add(item);
                }
                
            }
        }
        private void FillDataToVehicleList()
        {
            m_Main.lst_Vehicle.Items.Clear();

            if (null != m_TargetList.Staff)
                foreach (var staff in m_TargetList.Staff)
            {
                if (null != staff.Value.Staff)
                {
                    if (StaffType.Staff == staff.Value.Staff.Type) continue;

                    ListViewItem item = new ListViewItem() { Content = staff.Value };

                    if ((null == staff.Value.Radio) || (false == staff.Value.Radio.IsOnline))
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

            if (null != m_TargetList.Radio)
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
                m_TargetList = TargetMgr.TargetList;
                Filter(m_Main.txt_ResCondition.Text);


                if (null != m_Main.tab_Mgr) switch (m_Main.tab_Mgr.SelectedIndex)
                    {             
                    case 0:
                      FillDataToOrgTreeView();
                      break;
                case 1:
                      FillDataToGroupList();break;
                case 2:
                      FillDataToEmployeeList();break;
                    case 3:
                      FillDataToVehicleList();break;
                    case 4:
                      FillDataToRadioList();break;
                    default:
                        break;
                }
            })); 
        }

        static DependencyObject VisualUpwardSearch<T>(DependencyObject source)
        {
            while (source != null && source.GetType() != typeof(T))
                source = VisualTreeHelper.GetParent(source);

            return source;
        }

        private void OnTreeRightClick(object sender, MouseButtonEventArgs e)
        {
            var treeViewItem = VisualUpwardSearch<TreeViewItem>(e.OriginalSource as DependencyObject) as TreeViewItem;
            if (treeViewItem != null)
            {
                treeViewItem.Focus();
                e.Handled = true;
            }
        }
        private bool IsAccept(string condition, CMember dest)
        {
            if (((null != dest.Group) && (dest.Group.GroupID.ToString().ToLower().Contains(condition.ToLower()) || dest.Group.Name.ToLower().Contains(condition.ToLower())))
                      || ((null != dest.Staff) && dest.Staff.Name.ToLower().Contains(condition.ToLower()))
                      || ((null != dest.Radio) && dest.Radio.RadioID.ToString().ToLower().Contains(condition.ToLower())))
            {
                return true;
            }
            else
                return false;
        }

        private void Filter(string condition)
        {
            List<int> willdel = new List<int> ();
            List<long> hasgroup = new List<long>();

            if (null != m_TargetList.Staff)
                foreach (var it in m_TargetList.Staff)
                    if (!IsAccept(condition, it.Value)) willdel.Add(it.Key);
                    else if ((null != it.Value) && (null != it.Value.Group)) hasgroup.Add(it.Value.Group.ID);

            foreach (int key in willdel) m_TargetList.Staff.Remove(key);
            willdel.Clear();

            if (null != m_TargetList.Radio)
                foreach (var it in m_TargetList.Radio)
                    if (!IsAccept(condition, it.Value)) willdel.Add(it.Key);
                    else if ((null != it.Value) && (null != it.Value.Group)) hasgroup.Add(it.Value.Group.ID);

            foreach (int key in willdel) m_TargetList.Radio.Remove(key);
            willdel.Clear();

            if (null != m_TargetList.Group)
                foreach (var it in m_TargetList.Group)
                {
                    bool remove = true;
                    foreach (int id in hasgroup)
                        if ((null != it.Value) && (null != it.Value.Group) && (it.Value.Group.ID == id))
                        {
                            remove = false;
                            break;
                        }
                    if ((remove == true) && (!IsAccept(condition, it.Value))) willdel.Add(it.Key);
                }

            foreach (int key in willdel) m_TargetList.Group.Remove(key);
            willdel.Clear();
        }


        private bool IsNeedUpdate = false;

        public void SetRadioOnline(long id , bool online)
        {
            try
            {                
                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio.RadioID == id);
                foreach (var item in radio)
                {
                    item.Value.Radio.IsOnline = online;
                    Target.Update(item.Value.Radio);
                    IsNeedUpdate = true;

                    m_Main.SubWindow.UpdateOpWin(item.Value, 1, online);
                    m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 1, online);
                    return;
                }

                Target.Update(new Radio() {ID = -2, RadioID  = id, IsOnline = online});
                IsNeedUpdate = true;
            }
            catch
            {}
        }

        public void SetGpsOnline(long id, bool online)
        {
            try
            {
                var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio.RadioID == id);
                foreach (var item in radio)
                {
                    item.Value.Radio.IsGPS = online;
                    Target.Update(item.Value.Radio);

                    m_Main.SubWindow.UpdateOpWin(item.Value, 2, online);
                    m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 2, online);

                    IsNeedUpdate = true;
                    return;
                }

                Target.Update(new Radio() { ID = -2, RadioID = id, IsGPS = online });
                IsNeedUpdate = true;
            }
            catch
            { }
        }

        public void SetTx(TargetType type, long id, bool online)
        {
            try
            {
                if (type == TargetType.All)
                {
                    TargetMgr.IsTx = online;
                    m_Main.SubWindow.UpdateOpWin(null, 4, online);
                    m_Main.WorkArea.FastPanel.UpdateOpWin(null,4, online);
                }
                else if (type == TargetType.Private)
                {
                    var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio.RadioID == id);
                    foreach (var item in radio)
                    {
                        item.Value.Radio.IsTx = online;
                        Target.Update(item.Value.Radio);
                        IsNeedUpdate = true;

                        m_Main.SubWindow.UpdateOpWin(item.Value, 4, online);
                        m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 4, online);
                        return;
                    }

                    Radio rad = new Radio() { ID = -2, RadioID = id, IsTx = online };
                    Target.Update(rad);
                    IsNeedUpdate = true;
                    m_Main.WorkArea.FastPanel.UpdateOpWin(new CMember() { Type = MemberType.Radio, Radio = rad }, 4, online);

                }
                else if (type == TargetType.Group)
                {
                    var group = TargetMgr.TargetList.Group.Where(p => p.Value.Group.GroupID == id);
                    foreach (var item in group)
                    {
                        item.Value.Group.IsTx = online;
                        Target.Update(item.Value.Group);
                        IsNeedUpdate = true;

                        m_Main.SubWindow.UpdateOpWin(item.Value, 4, online);
                        m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 4, online);
                        return;
                    }

                    Department dep = new Department() { ID = -2, Name = "组：" + id.ToString(), GroupID = id, IsTx = online };

                    Target.Update(dep);
                    IsNeedUpdate = true;
                    m_Main.WorkArea.FastPanel.UpdateOpWin(new CMember() { Type = MemberType.Group, Group = dep }, 4, online);
                }
            }
            catch
            { }
        }
        public void SetRx(TargetType type, long id, bool online)
        {
            try
            {
                if (type == TargetType.All)
                {
                    TargetMgr.IsRx = online;
                    m_Main.SubWindow.UpdateOpWin(null, 8, online);
                    m_Main.WorkArea.FastPanel.UpdateOpWin(null, 8, online);
                }
                else  if (type == TargetType.Private)
                {
                    var radio = TargetMgr.TargetList.Radio.Where(p => p.Value.Radio.RadioID == id);
                    foreach (var item in radio)
                    {
                        item.Value.Radio.IsRx = online;
                        Target.Update(item.Value.Radio);
                        IsNeedUpdate = true;

                        m_Main.SubWindow.UpdateOpWin(item.Value, 8, online);
                        m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 8, online);
                        return;
                    }

                    Radio rad = new Radio() { ID = -2, RadioID = id, IsRx = online };
                    Target.Update(rad);
                    IsNeedUpdate = true;
                    m_Main.WorkArea.FastPanel.UpdateOpWin(new CMember() {Type = MemberType.Radio , Radio = rad}, 8, online);
                }
                else if (type == TargetType.Group)
                {
                    var group = TargetMgr.TargetList.Group.Where(p => p.Value.Group.GroupID == id);
                    foreach (var item in group)
                    {
                        item.Value.Group.IsRx = online;
                        Target.Update(item.Value.Group);
                        IsNeedUpdate = true;

                        m_Main.SubWindow.UpdateOpWin(item.Value, 8, online);
                        m_Main.WorkArea.FastPanel.UpdateOpWin(item.Value, 8, online);
                        return;
                    }
                    Department dep = new Department() { ID = -2, Name = "组：" + id.ToString(), GroupID = id, IsRx = online };

                    Target.Update(dep);
                    IsNeedUpdate = true;
                    m_Main.WorkArea.FastPanel.UpdateOpWin(new CMember() { Type = MemberType.Group, Group = dep }, 8, online);
                }
            }
            catch
            { }
        }


        private void ResourceUpdateThread()
        {
            while(true)
            {
                if (IsNeedUpdate)
                {
                    UpdateView();
                    IsNeedUpdate = false;
                }
                Thread.Sleep(1000);
            }
        }

    }
}
