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
    public class StaffsViewModel:ResourcesViewModel<Staff>
    {
        private BindElement _bindGroup;

        private List<Group> _groups;
        public StaffsViewModel()
        {
            _resourcesName = Resource.Staff;
            ResourceOpcode = RequestOpcode.staff;
            RecordsName = "staffs";

            _bindGroup = new BindElement()
            {
                ResourceOpcode = RequestOpcode.department,
                AssignOperate = ResourcesManage.OperateType.assignUser,
                DetachOperate = ResourcesManage.OperateType.detachUser,
                SoureName = "user",
                TargetName = "department"
            };

            _groups = new List<Group>();
        }


        public List<Staff> StaffList { get { return _elements; } }

        public List<Group> GroupList { get { return _groups; } set { if (value != null) { _groups = value; NotifyPropertyChanged(new string[] { "Groups", "GroupIndex" }); } } }
        public ICollectionView Groups { get { return new ListCollectionView(_groups); } }


        public int Type { set { Element.Type = value == 0 ? Staff.StaffType.Staff : Staff.StaffType.Vehicle; } get { if (Element == null)return -1; return Element.Type == Staff.StaffType.Staff ? 0 : 1; } }
        public string Name { set { Element.Name = value; } get { if (Element == null)return ""; return Element.Name; } }
        public string PhoneNumber { set { Element.PhoneNumber = value; } get { if (Element == null)return ""; return Element.PhoneNumber; } }

        public int GroupIndex
        {
            get { return _groups == null ? -1 : _groups.FindIndex(p => p.ID == Element.DepartmentID); }
            set { if (value >= 0)Element.DepartmentID = _groups[value].ID; }
        }

        protected override void OnResourceChanged()
        {
            NotifyPropertyChanged("StaffList");
        }

        protected override void OnSelectedChanged()
        {
            if (Element != null) NotifyPropertyChanged(new string[]{
                "Type"
                ,"Name"     
                ,"PhoneNumber"   
                ,"GroupIndex"   
            });
        }

        public override SaveStatus Save()
        {
            SaveStatus reslut = base.Save() == SaveStatus.Failure ? SaveStatus.Failure : SaveStatus.Success;
            reslut = _bindGroup.Save() ? reslut : SaveStatus.Failure;
            return reslut;
        }


        new public ICommand Create
        {
            get
            {
                return new Command(() =>
                {
                    base.Create.Execute(null);
                    if (Element != null && Element.DepartmentID > 0) _bindGroup.Assign(Element.ID, Element.DepartmentID);
                });
            }
        }

        new public ICommand Modify
        {
            get
            {
                return new Command(() =>
                {
                    base.Modify.Execute(null);
                    if (Element != null && Element.DepartmentID > 0) _bindGroup.Assign(Element.ID, Element.DepartmentID);                    
                });
            }
        }

        new public ICommand Delete
        {
            get
            {
                return new Command(() =>
                {
                    if (SelectedElement != null) _bindGroup.Detach(SelectedElement.ID, SelectedElement.DepartmentID);
                    base.Delete.Execute(null);
                    
                });
            }
        }      

    }
}
