using System;
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
        
        private Main m_Main;
        public TargetMgr m_Target = new TargetMgr();

        private Dictionary<COrganization, List<COrganization>> OrgList;

        public MainResourceMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_Target.UpdateTragetList();
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
            foreach (var group in OrgList)
            {
                TreeViewItem itemGroup = new TreeViewItem() { Header = group.Key.target.KeyHeader };
                itemGroup.Style = App.Current.Resources["TreeViewItemStyleGroup"] as Style;

                if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))                
                    itemGroup.IsExpanded = ((TreeViewItem)OrginItem.Items[groupcount]).IsExpanded;               
                else itemGroup.IsExpanded = group.Key.is_exp; 

                itemGroup.Tag = group.Key;
                itemGroup.ContextMenu = CreateOrgMenu(ContextMenuType.Group);
                if (null == group.Key.target.group) itemGroup.ContextMenu.Visibility = Visibility.Hidden;


                if (null != group.Value)
                {
                    int itemcount = 0;
                    foreach (var item in group.Value)
                    {
                        TreeViewItem childitem = new TreeViewItem();
                        childitem.Header = item.target.KeyHeaderWithoutGroup;
                        childitem.Style = App.Current.Resources["TreeViewItemStyleMember"] as Style;
                        childitem.Tag = item;


                        if ((null != OrginItem) && (OrginItem.Items.Count > groupcount))
                        {
                            if ((null != (TreeViewItem)OrginItem.Items[groupcount]) && (((TreeViewItem)OrginItem.Items[itemcount]).Items.Count > itemcount))
                            {
                               if(true == ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsFocused)
                                    childitem.Focus();
                               childitem.IsSelected = ((TreeViewItem)((TreeViewItem)OrginItem.Items[groupcount]).Items[itemcount]).IsSelected;
                               
                            }
                        }


                        if(true == childitem.IsSelected)m_Main.CurrentTraget = item;


                        
                        
                        if ((null == item.target.radio) || (false == item.target.radio.is_online))
                        {
                            childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                        }
                        else
                        {
                            childitem.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);
                        }

                        itemGroup.Items.Add(childitem);
                        itemcount++;
                    }  
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
                m_Main.CurrentTraget = ((TreeViewItem)((TreeView)sender).SelectedItem).Tag as COrganization;


                if (null == m_Main.CurrentTraget) //All
                {
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;

                }
                else if (OrgItemType.Type_Group == m_Main.CurrentTraget.target.key) //Group
                {
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;
                }
                else if (null != m_Main.CurrentTraget.target.radio)//radio
                {
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;
                }
                else // No Radio
                {
                    m_Main.bdr_Tool_Base.IsEnabled = false;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = false;
                    m_Main.menu_Target.Visibility = Visibility.Collapsed;
                }

            };
            m_Main.tree_OrgView.PreviewMouseDoubleClick += delegate(object sender, MouseButtonEventArgs e)
            {

            };
        }

        private void OnOrganizationMenu_Click(object sender, RoutedEventArgs e)
        {
            
            COrganization org = null;
            if(((ContextMenu)((MenuItem)sender).Parent).PlacementTarget is TreeViewItem)
            {
                org = ((TreeViewItem)((ContextMenu)((MenuItem)sender).Parent).PlacementTarget).Tag as COrganization;
            }
            else if(((ContextMenu)((MenuItem)sender).Parent).PlacementTarget is ListBoxItem)
            {
                org = ((ListBoxItem)((ContextMenu)((MenuItem)sender).Parent).PlacementTarget).Content as COrganization;
            }

            if (null == org) return;

            switch ((string)((MenuItem)sender).Tag)
            {
                case "fast":
                    m_Main.WorkArea.FastPanel.Add(new FastOperate()
                    {
                        m_Type = FastType.FastType_Contact,
                        m_Contact = org
                    });
                    break;
                case "dispatch":
                    m_Main.SubWindow.OpenOrCreateTragetWin(org, new COperate(OPType.Dispatch, null));
                    break;
                case "message":
                    m_Main.SubWindow.OpenOrCreateTragetWin(org, new COperate(OPType.ShortMessage, null));
                    break;
                case "position":
                    m_Main.SubWindow.OpenOrCreateTragetWin(org, new COperate(OPType.Position, null));
                    break;
                case "control":                   
                    break;
                case "jobticker":
                    m_Main.SubWindow.OpenOrCreateTragetWin(org, new COperate(OPType.JobTicker, null));
                    break;
            };
        }

        private void FillDataToGroupList()
        {
            m_Main.lst_Group.View = (ViewBase)m_Main.FindResource("GroupView");
            m_Main.lst_Group.Items.Clear();
            foreach (var group in OrgList)
            {
                if (null != group.Key.target.group)
                {
                    ListViewItem item = new ListViewItem() { Content = group.Key };
                    item.ContextMenu = CreateOrgMenu(ContextMenuType.Group);
                    m_Main.lst_Group.Items.Add(item);
                }
            }          
        }

        private void FillDataToEmployeeList()
        {
            m_Main.lst_Employee.View = (ViewBase)m_Main.FindResource("EmployeeView");
            m_Main.lst_Employee.Items.Clear();
            foreach (var group in OrgList)
            {
                foreach(var org in group.Value)
                {
                    if ((null != org.target.employee) && (OrgItemType.Type_Employee == org.target.key)) 
                    {
                        ListViewItem item = new ListViewItem() { Content = org };
                        if ((null == org.target.radio) || (false == org.target.radio.is_online))
                            item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);                       
                        else
                            item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);                        
                       
                        m_Main.lst_Employee.Items.Add(item);
                    }
                }
            }
        }
        private void FillDataToVehicleList()
        {
            m_Main.lst_Vehicle.View = (ViewBase)m_Main.FindResource("VehicleView");
            m_Main.lst_Vehicle.Items.Clear();
            foreach (var group in OrgList)
            {
                foreach (var org in group.Value)
                {
                    if ((null != org.target.vehicle) && (OrgItemType.Type_Vehicle == org.target.key))
                    {
                        ListViewItem item = new ListViewItem() { Content = org };

                        if ((null == org.target.radio) || (false == org.target.radio.is_online))
                            item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                        else
                            item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                        m_Main.lst_Vehicle.Items.Add(item);
                    }
                }
            }
        }
        private void FillDataToRadioList()
        {
            m_Main.lst_Radio.View = (ViewBase)m_Main.FindResource("RadioView");
            Dictionary<int, ListBoxItem> RadioList = new Dictionary<int, ListBoxItem>();
            m_Main.lst_Radio.Items.Clear();
            foreach (var group in OrgList)
            {
                foreach (var org in group.Value)
                {
                    if (null != org.target.radio)
                    {
                        if(!RadioList.ContainsKey(org.target.radio.id))
                        {
                            ListViewItem item = new ListViewItem() { Content = org };
                            if (false == org.target.radio.is_online)
                                item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOff);
                            else
                                item.ContextMenu = CreateOrgMenu(ContextMenuType.RadioOn);

                            RadioList.Add(org.target.radio.id, item);
                            m_Main.lst_Radio.Items.Add(item);
                        }
                    }
                }
            }
        }

        private void Update(CRadio radio)
        {
            if (!m_Target.g_RadioList.ContainsKey(radio.id))
            {
                m_Target.g_RadioList.Add(radio.id, radio);
            }
            else
            {
                m_Target.g_RadioList[radio.id] = radio;
            }          
        }

        private void UpdateView()
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                m_Main.ChoiceContact.ContactSrc = m_Target.OrgList;
                              
                OrgList = m_Target.OrgList;
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
                foreach (var group in OrgList)
                {
                    //foreach (var item in group.Value)
                    //{
                    //    if (null != item.target.radio)
                    //    {
                    //        CRadio radio = new CRadio() { id = item.target.radio.id, radio_id = item.target.radio.radio_id, is_online = !item.target.radio.is_online, type = item.target.radio.type };
                    //        Update(radio);

                    //    }
                    //}

                    Thread.Sleep(5000);
                    //UpdateView();
                }                               
            }
        }

    }
}
