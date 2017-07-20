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

namespace Manager
{
    public class CVMStaff:CVMManager, INotifyPropertyChanged
    {
        public CVMStaff()
        {
            if (m_Staff == null)
            {
                m_Staff = new CStaffMgr();
                m_Staff.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditStaff == null) m_EditStaff = new CStaff();           
        }


        private CStaffMgr m_Staff;
        public ObservableCollection<CRElement> Staffs { get { return new ObservableCollection<CRElement>(m_Staff.List); } }
        public List<CRElement> StaffList { get { return new List<CRElement>(m_Staff.List); } }

        private CStaff m_EditStaff;
        public CStaff EditStaff
        {
            get { return m_EditStaff; }
            set
            {
                if (value == null)
                {
                    m_EditStaff = new CStaff();
                    m_Staff.IsNew = true;
                }
                else
                {
                    OrginStaff = value.Copy();
                    m_EditStaff = value;
                    m_Staff.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Type"));
                    PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                    PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
                    PropertyChanged(this, new PropertyChangedEventArgs("PhoneNumber"));
                }
            }
        }

        public int Type { set { m_EditStaff.Type = value == 0 ? StaffType.Staff : StaffType.Vehicle; } get { if (m_EditStaff == null)return -1; return m_EditStaff.Type == StaffType.Staff ? 0 : 1; } }
        public string Name { set { m_EditStaff.Name = value; } get { if (m_EditStaff == null)return ""; return m_EditStaff.Name; } }
        public long DepartmentID { set { m_EditStaff.DepartmentID = value; } get { if (m_EditStaff == null)return 0; return m_EditStaff.DepartmentID; } }
        public string PhoneNumber { set { m_EditStaff.PhoneNumber = value; } get { if (m_EditStaff == null)return ""; return m_EditStaff.PhoneNumber; } }

        private CDepartment CurrentDept;
        private CStaff OrginStaff;

        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.staff && PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Staffs"));
            }
        }


        //command
        public ICommand New { get { return new CDelegateCommand(NewEle); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteEle); } }
        public ICommand Save { get { return new CDelegateCommand(SaveEle); } }

        public ICommand DepartmentChanged { get { return new CDelegateCommand(OnDepartmentChanged); } }



        private void NewEle()
        {
            m_Staff.IsNew = true;
            m_EditStaff = new CStaff();
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("EditStaff"));
                PropertyChanged(this, new PropertyChangedEventArgs("Type"));
                PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
                PropertyChanged(this, new PropertyChangedEventArgs("PhoneNumber"));                
            }
        }

        private void DeleteEle()
        {
            if (m_EditStaff == null) return;
            m_Staff.Delete(m_EditStaff.ID);
            if (m_EditStaff.ID > 0) m_Staff.Detach(m_EditStaff, m_EditStaff.DepartmentID);
            
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("Staffs"));
        }

        //parameter:password,can not binding on passwordbox
        private void SaveEle(object parameter)
        {
            if (m_EditStaff == null || !(parameter is ListView)) return;

            try
            {
                ListView lst = parameter as ListView;              
 
                if (m_Staff.IsNew)
                {
                    if (m_EditStaff.Name != null && m_EditStaff.Name != string.Empty)
                    {
                        m_Staff.Add(m_EditStaff);
                        if (CurrentDept != null) m_Staff.Assign(m_EditStaff, CurrentDept.GroupID);
                    }
                }
                else
                {
                    
                    if (m_EditStaff.ID > 0)
                    {
                        if (OrginStaff != null && OrginStaff.DepartmentID > 0) m_Staff.Detach(m_EditStaff, OrginStaff.DepartmentID);
                    }

                    if (CurrentDept != null) m_Staff.Assign(m_EditStaff, CurrentDept.GroupID);
                    m_Staff.Modify(m_EditStaff.ID, m_EditStaff);
                }

                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("Staffs"));
                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_Staff.IsNew = false;
        }

        private void OnDepartmentChanged(object parameter)
        {
            if (parameter == null || !(parameter is ComboBox)) return;
            if(((ComboBox)parameter).SelectedItem != null)
            {
                CurrentDept = ((ComboBox)parameter).SelectedItem as CDepartment;
                m_EditStaff.DepartmentID = CurrentDept.ID;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
            }           
        }


        public void AssignDetach(List<CRElement> depts)
        {
            m_Staff.AssignDetach(depts);
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_Staff.Save();            
        }

        public void Get() 
        {
            m_Staff.Query();
        }      
    }
}
