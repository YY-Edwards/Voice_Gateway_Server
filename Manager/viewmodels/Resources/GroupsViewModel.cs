using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;
using System.Windows.Data;

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{   
    public class GroupsViewModel: ResourcesViewModel<Group>
    {
        public GroupsViewModel()
        {
            _resourcesName = Resource.Group;
            ResourceOpcode = RequestOpcode.department;
            RecordsName = "departments";         
        }

        public List<Group> GroupList { get { return _elements; } }

        public string GroupName { set { Element.Name = value; } get { return Element.Name; } }
        public int GroupID { set { Element.GroupID = value; } get { return Element.GroupID; } }

        protected override void OnResourceChanged()
        {
            NotifyPropertyChanged("GroupList");
        }

        protected override void OnSelectedChanged()
        {
            if (Element != null) NotifyPropertyChanged(new string[]{
                "GroupName"
                ,"GroupID"          
            });
        }
    }
}
