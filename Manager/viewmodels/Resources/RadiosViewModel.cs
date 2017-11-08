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
    public class RadiosViewModel:ResourcesViewModel<Radio>
    {
        private BindElement _bindGroup;
        private BindElement _bindStaff;

        private List<Group> _groups;
        private List<Staff> _staffs;

        public RadiosViewModel()
        {
            _resourcesName = Resource.Radio;
            ResourceOpcode = RequestOpcode.radio;
            RecordsName = "radios";

            _bindGroup = new BindElement()
            {
                ResourceOpcode = RequestOpcode.department,
                AssignOperate = ResourcesManage.OperateType.assignRadio,
                DetachOperate = ResourcesManage.OperateType.detachRadio,
                SoureName= "radio",
                TargetName= "department"
            };

            _bindStaff = new BindElement() 
            {
                ResourceOpcode = RequestOpcode.user,
                AssignOperate = ResourcesManage.OperateType.assignRadio,
                DetachOperate = ResourcesManage.OperateType.detachRadio,
                SoureName = "radio",
                TargetName = "user"
            };

            _groups = new List<Group>();
            _staffs = new List<Staff>();
        }

        public List<Group> GroupList { get { return _groups; } set { _groups = value; NotifyPropertyChanged(new string[] { "Groups", "GroupIndex" }); } }
        public List<Staff> StaffList { get { return _staffs; } set { _staffs = value; NotifyPropertyChanged(new string[] { "Staffs", "StaffIndex" }); } }

        public ICollectionView Groups { get { return new ListCollectionView(_groups); } }
        public ICollectionView Staffs { get { return new ListCollectionView(_staffs); } }

        public int Type { set { Element.Type = value == 0 ? Radio.RadioType.Radio : Radio.RadioType.Ride; } get { if (Element == null)return -1; return Element.Type == Radio.RadioType.Radio ? 0 : 1; } }
        public long RadioID { set { Element.RadioID = value; } get { if (Element == null)return 0; return Element.RadioID; } }
        public string SN { set { Element.SN = value; } get { if (Element == null)return ""; return Element.SN; } }

        public bool HasScreen { set { Element.HasScreen = value; } get { if (Element == null)return false; return Element.HasScreen; } }
        public bool HasKey { set { Element.HasKeyboard = value; } get { if (Element == null)return false; return Element.HasKeyboard; } }
        public bool HasGPS { set { Element.HasGPS = value; } get { if (Element == null)return false; return Element.HasGPS; } }
        public bool HasLocationInDoor { set { Element.HasLocationInDoor = value; } get { if (Element == null)return false; return Element.HasLocationInDoor; } }
        public int GroupIndex 
        {
            get { return _groups == null ? -1 : _groups.FindIndex(p => p.ID == Element.DepartmentID); }
            set { if (value >= 0)Element.DepartmentID = _groups[value].ID; } 
        }
        public int StaffIndex 
        { 
            get { return _staffs == null ? -1 : _staffs.FindIndex(p => p.ID == Element.StaffID); }
            set { if (value >= 0)Element.StaffID = _staffs[value].ID; } 
        }


        protected override void OnResourceChanged()
        {
         
        }

        protected override void OnSelectedChanged()
        {
            if (Element != null) NotifyPropertyChanged(new string[]{
                "Type"
                ,"RadioID"   
                ,"SN"  
                ,"HasScreen"  
                ,"HasKey"  
                ,"HasGPS"  
                ,"HasLocationInDoor"  
                ,"GroupIndex"  
                ,"StaffIndex"  
            });
        }

        public override SaveStatus Save()
        {
            SaveStatus reslut = base.Save() == SaveStatus.Failure ? SaveStatus.Failure : SaveStatus.Success;
            reslut = _bindGroup.Save() ? reslut : SaveStatus.Failure;
            reslut = _bindStaff.Save() ? reslut : SaveStatus.Failure;
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
                    if (Element != null && Element.StaffID > 0) _bindStaff.Assign(Element.ID, Element.StaffID);
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
                    if (Element != null && Element.StaffID > 0) _bindStaff.Assign(Element.ID, Element.StaffID);
                });
            }
        }

        new public ICommand Delete
        {
            get
            {
                return new Command(() =>
                {
                    if (SelectedElement != null)
                    {
                        _bindGroup.Detach(SelectedElement.ID, SelectedElement.DepartmentID);
                        _bindGroup.Detach(SelectedElement.ID, SelectedElement.StaffID);
                    }
                    base.Delete.Execute(null);

                });
            }
        }      
    }
}
