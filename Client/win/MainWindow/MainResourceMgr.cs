using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace TrboX
{
    public class MainResourceMgr
    {
        
        private Main m_Main;
        public TargetMgr m_Target = new TargetMgr();

        public MainResourceMgr(Main win)
        {
            if (null == win) return;
            m_Main = win;

            m_Target.UpdateTragetList();
            FillDataToOrgTreeView();
        }

        private void FillDataToOrgTreeView()
        {
            Dictionary<COrganization, List<COrganization>> OrgList = m_Target.GetOrgList();

            //right button menu 
            TreeViewItem itemOrg = new TreeViewItem() { Header = "用户/设备" };
            itemOrg.Style = App.Current.Resources["TreeViewItemRoot"] as Style;
            itemOrg.IsExpanded = true;

            //menu
            itemOrg.ContextMenu = new ContextMenu();
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "在线检测", Width = 160, Tag = "check", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            itemOrg.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });
            //itemOrg.ContextMenu.Items.Add (new MenuItem() { Header = "远程监听", Width = 160, Tag = "monitor", Style = App.Current.Resources["MenuItemStyleNormal"] as Style }); 
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "语音调度", Width = 160, Tag = "dispatch", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "短消息", Width = 160, Tag = "message", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "位置查询", Width = 160, Tag = "position", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "指令控制", Width = 160, Tag = "control", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
            itemOrg.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });
            itemOrg.ContextMenu.Items.Add(new MenuItem() { Header = "工单", Width = 160, Tag = "jobticker", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });

            for (int i = 0; i < 8; i++)
                if ((i != 1) && (i != 6))
                    ((MenuItem)itemOrg.ContextMenu.Items[i]).Click += new RoutedEventHandler(OnOrganizationMenu_Click);

            if (null == itemOrg) return;

            foreach (var group in OrgList)
            {
                TreeViewItem itemGroup = new TreeViewItem() { Header = "♟♙" + group.Key.target.KeyHeader };
                itemGroup.Style = App.Current.Resources["TreeViewItemStyle2nd"] as Style;
                itemGroup.IsExpanded = group.Key.is_exp;
                itemGroup.Tag = group.Key;


                itemGroup.ContextMenu = new ContextMenu();
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "添加到快速操作", Width = 160, Tag = "fast", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "在线检测", Width = 160, Tag = "check", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "远程监听", Width = 160, Tag = "monitor", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });

                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "语音调度", Width = 160, Tag = "dispatch", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "短消息", Width = 160, Tag = "message", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "位置查询", Width = 160, Tag = "position", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "指令控制", Width = 160, Tag = "control", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                itemGroup.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });
                itemGroup.ContextMenu.Items.Add(new MenuItem() { Header = "工单", Width = 160, Tag = "jobticker", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });

                for (int i = 0; i < 10; i++)
                    if ((i != 1) && (i != 4) && (i != 9))
                        ((MenuItem)itemGroup.ContextMenu.Items[i]).Click += new RoutedEventHandler(OnOrganizationMenu_Click);

                if (null == group.Key.target.group) itemGroup.ContextMenu.Visibility = Visibility.Hidden;


                if (null != group.Value)
                {
                    foreach (var item in group.Value)
                    {
                        TreeViewItem childitem = new TreeViewItem();
                        switch (item.target.key)
                        {
                            case OrgItemType.Type_Employee:
                                childitem.Header = "☺" + item.target.KeyHeaderWithoutGroup;
                                break;
                            case OrgItemType.Type_Vehicle:
                                childitem.Header = "♜" + item.target.KeyHeaderWithoutGroup;
                                break;
                            case OrgItemType.Type_Radio:
                                childitem.Header = "☏" + item.target.KeyHeaderWithoutGroup;
                                break;
                            case OrgItemType.Type_Ride:
                                childitem.Header = "◔" + item.target.KeyHeaderWithoutGroup;
                                break;
                            default: break;
                        }

                        if (null != item.target.radio)
                        {
                            if (true == item.target.radio.is_online)
                            {
                                childitem.Header = childitem.Header + "√";
                            }
                            else
                            {
                                childitem.Header = childitem.Header + "X";
                            }
                        }

                        childitem.Tag = item;

                        childitem.Style = App.Current.Resources["TreeViewItemStyle3rd"] as Style;

                        childitem.ContextMenu = new ContextMenu();
                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "添加到快速操作", Width = 160, Tag = "fast", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });

                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "在线检测", Width = 160, Tag = "check", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "远程监听", Width = 160, Tag = "monitor", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });

                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "语音调度", Width = 160, Tag = "dispatch", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "短消息", Width = 160, Tag = "message", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "位置查询", Width = 160, Tag = "position", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "指令控制", Width = 160, Tag = "control", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });
                        childitem.ContextMenu.Items.Add(new Separator() { Margin = new Thickness(5) });

                        childitem.ContextMenu.Items.Add(new MenuItem() { Header = "工单", Width = 160, Tag = "jobticker", Style = App.Current.Resources["MenuItemStyleNormal"] as Style });

                        for (int i = 0; i < 11; i++)
                            if ((i != 1) && (i != 4) && (i != 9))
                                ((MenuItem)childitem.ContextMenu.Items[i]).Click += new RoutedEventHandler(OnOrganizationMenu_Click);

                        itemGroup.Items.Add(childitem);
                    }
                }

                itemOrg.Items.Add(itemGroup);
            }
            if (null != m_Main.tree_OrgView) m_Main.tree_OrgView.Items.Add(itemOrg);

            m_Main.tree_OrgView.SelectedItemChanged += delegate(object sender, RoutedPropertyChangedEventArgs<object> e)
            {
                COrganization target = ((TreeViewItem)((TreeView)sender).SelectedItem).Tag as COrganization;

                if (null == target) //All
                {
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;

                }
                else if (OrgItemType.Type_Group == target.target.key) //Group
                {
                    m_Main.bdr_Tool_Base.IsEnabled = true;
                    m_Main.bdr_Tool_Ctrl.IsEnabled = true;
                    m_Main.menu_Target.Visibility = Visibility.Visible;
                }
                else if (null != target.target.radio)//radio
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
            TreeViewItem item = ((ContextMenu)((MenuItem)sender).Parent).PlacementTarget as TreeViewItem;

            switch ((string)((MenuItem)sender).Tag)
            {
                case "fast":
                    m_Main.WorkArea.FastPanel.Add(new FastOperate()
                    {
                        m_Type = FastType.FastType_Contact,
                        m_Contact = ((COrganization)item.Tag).target
                    });
                    break;
            };
        }

    }
}
