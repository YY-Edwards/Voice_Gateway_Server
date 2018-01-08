using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Controls;

using Sigmar;
using Dispatcher;


using Dispatcher.Modules;
using Dispatcher.Service;
using Dispatcher.Views;

namespace Dispatcher.ViewsModules
{
    
    public class VMOrganization : INotifyPropertyChanged
    {
        public event OperatedEventHandler OnOperated;
        public event EventHandler OnResourcesLoaded;
        private ObservableCollection<TreeViewItem> _organizationTree = new ObservableCollection<TreeViewItem>();
        public ICollectionView OrganizationTree { get { return new ListCollectionView(_organizationTree); } }

        public VMOrganization()
        {         
        }


        private void OnAllTargetOperated(OperatedEventArgs e)
        {
            if (OnOperated != null) OnOperated(e);
        }
        public ICommand UpdateResource { get { return new Command(UpdateResourceExec); } }
        public void UpdateResourceExec(object parameter)
        {
            if (parameter == null || !(parameter is ResourcesMgr)) return;
            ResourcesMgr _resource = parameter as ResourcesMgr;

            List<VMTarget> NoBounded = _resource.Members.FindAll(p => true);
            List<TreeViewItem> GroupList = new List<TreeViewItem>();

            foreach (VMTarget group in _resource.Groups)
            {
                List<VMTarget> currentgroup = _resource.Members.FindAll(p => p.Member.GroupID == group.Group.GroupID);
                List<TreeViewItem> memberlist = new List<TreeViewItem>();
                foreach (VMTarget member in currentgroup)
                {
                    memberlist.Add(BuildItem(TreeDeep_t.Member, member));
                }

                GroupList.Add(BuildItem(TreeDeep_t.Group, group, memberlist));

                NoBounded.RemoveAll(p => p.Member.GroupID == group.Group.GroupID);
            }

            if (NoBounded.Count > 0)
            {
                List<TreeViewItem> memberlist = new List<TreeViewItem>();
                foreach (VMTarget member in NoBounded)
                {
                    memberlist.Add(BuildItem(TreeDeep_t.Member, member));
                }


                GroupList.Add(BuildItem(TreeDeep_t.Group, new VMTarget( new CGroup() { Name = "未分组" }), memberlist));
            }

            _organizationTree.Clear();
            _organizationTree.Add(BuildItem(TreeDeep_t.Device, _resource.AllTarget, GroupList));

            NotifyPropertyChanged("OrganizationTree");  
        }

        public ICommand Search { get { return new Command(SearchExec); } }

        private void SearchExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                string key = parameter as string;
                if (_organizationTree != null && _organizationTree.Count > 0) SearchProc(_organizationTree[0].ItemsSource, key);
                NotifyPropertyChanged("OrganizationTree");  
            }
        }

        private Visibility SearchProc(IEnumerable items, string key)
        {
            if (items == null) return Visibility.Collapsed;

            Visibility itemsvisible = Visibility.Collapsed;

            foreach (TreeViewItem item in items)
            {
                if (item.Header is VMTarget)
                {
                    VMTarget vmtarget = item.Header as VMTarget;
                    if (key == null || key == "" || vmtarget.FullName.IndexOf(key) >= 0)
                    {
                        item.Visibility = Visibility.Visible;
                        itemsvisible = Visibility.Visible;
                        SearchProc(item.ItemsSource, key); 
                    }
                    else item.Visibility = SearchProc(item.ItemsSource, key);
                }               
            }

            return itemsvisible;
        }
        private enum TreeDeep_t
        {
            Device,
            Group,
            Member,
        }
        private bool _isfirstgroupexpanded = false;
        private TreeViewItem BuildItem(TreeDeep_t deep, VMTarget current, IEnumerable items = null)
        {
            TreeViewItem item = new TreeViewItem();           
            item.ItemsSource = items;
            item.Header = current;    
            switch(deep)
            {
                case TreeDeep_t.Device:
                    item.IsExpanded = true;
                    item.Style = App.Current.Resources["TreeViewItemStyleRoot"] as Style;
                    break;
                case TreeDeep_t.Group:
                    if (!_isfirstgroupexpanded)
                    {
                        item.IsExpanded = true;
                        _isfirstgroupexpanded = true;
                    }
                    item.Style = App.Current.Resources["TreeViewItemStyle2nd"] as Style;
                    break;
                case TreeDeep_t.Member:
                    item.Style = App.Current.Resources["TreeViewItemStyle3rd"] as Style;
                    break;
                default:
                    break;
            }
            return item;
        }


        public ICommand SelectedItemChanged { get { return new Command(SelectedItemChangedExec); } }

        private void SelectedItemChangedExec(object parameter)
        {
            if (parameter != null && parameter is TreeViewItem)
            {
               TreeViewItem item  =  parameter as TreeViewItem;
               if (item != null && item.Header != null && item.Header is VMTarget)
               {
                   VMTarget target = item.Header as VMTarget;
                   if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.SetCurrentTarget, item.Header));                  
               }
            }
            else
            {
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.SetCurrentTarget, null));     
            }
        }


        public ICommand ChangedStatusForTest { get { return new Command(ChangedStatusForTestExec); } }

        private void ChangedStatusForTestExec(object parameter)
        {
            if(parameter != null && parameter is TreeView)
            {
                TreeView tree = parameter as TreeView;
                TreeViewItem item = tree.SelectedItem as TreeViewItem;
                if (item != null && item.Header is VMTarget)
                {
                    VMTarget vmtarget = item.Header as VMTarget;
                    //vmtarget.ChangeValueExec(new MemberChangedEventArgs(ChangedKey_t.InLocation, LocationStatus_t.CsbkCycle));
                }
                //if (item != null && item.Header is CGroup)
                //{
                //    CGroup group = item.Header as CGroup;
                //    CMember member1 = new CMember() { GroupID = group.GroupID, ManCarName = "Jim", RadioID = 10 }.SetMancarType(Mancar.ManCarType_t.Staff).SetDeviceType(Device.DeviceType_t.Handset).SetParts(true, true, false, true).UpdateStatus(Device.ChangedKey_t.Online, true) as CMember;

                //    AddMember(group, member1);

                //    //item.Items.Add(BuildItem(TreeDeep_t.Group, member1));
                //}
            }
        }

        //private void AddMember(CGroup group, CMember member)
        //{
        //    if (_organizationTree != null && _organizationTree.Count > 0)
        //    {
        //        IEnumerable<TreeViewItem> groups = FindListInOrganizationTree(_organizationTree[0].ItemsSource, group) as IEnumerable<TreeViewItem>;
        //        List<TreeViewItem> list = groups as List<TreeViewItem>;
        //        if (list == null) return;
        //        list.Add(BuildItem(TreeDeep_t.Member, member));
        //        Log.Info(groups.ToString());
        //        //groups.Add(BuildItem(TreeDeep_t.Member, new VMMember(member)));
        //        NotifyPropertyChanged("OrganizationTree");
        //    }
        //}

        private object FindListInOrganizationTree(IEnumerable items, object objects)
        {
            if (items == null) return null;

            foreach (TreeViewItem item in items)
            {
                if (item.Header == objects) return item.ItemsSource;
                else
                {
                    object obj= FindListInOrganizationTree(item.ItemsSource, objects);
                    if(obj  != null)return obj;
                }
            }

            return null;
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion
    }
}
