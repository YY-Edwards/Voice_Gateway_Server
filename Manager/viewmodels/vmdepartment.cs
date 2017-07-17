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
    public class CVMDepartment:CVMManager, INotifyPropertyChanged
    {
        
        public CVMDepartment()
        {
            if (m_Department == null)
            {
                m_Department = new CDepartmentMgr();
                m_Department.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditDepartment == null) m_EditDepartment = new CDepartment();           
        }


        private CDepartmentMgr m_Department;
        public ObservableCollection<CRElement> Departments { get { return new ObservableCollection<CRElement>(m_Department.List); } }
        public List<CRElement> DepartmentList { get { return new List<CRElement>(m_Department.List); } }


        private CDepartment m_EditDepartment;
        public CDepartment EditDepartment
        {
            get { return m_EditDepartment; }
            set
            {
                if (value == null)
                {
                    m_EditDepartment = new CDepartment();
                    m_Department.IsNew = true;
                }
                else
                {
                    m_EditDepartment = value;
                    m_Department.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                    PropertyChanged(this, new PropertyChangedEventArgs("GroupID"));
                }
            }
        }

        public string Name { set { m_EditDepartment.Name = value; } get { if (m_EditDepartment == null)return null; return m_EditDepartment.Name; } }
        public long GroupID { set { m_EditDepartment.GroupID = value; } get { if (m_EditDepartment == null)return 0; return m_EditDepartment.GroupID; } }

        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.department && PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Departments"));
            }
        }


        //command
        public ICommand New { get { return new CDelegateCommand(NewDepartment); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteDepartment); } }
        public ICommand Save { get { return new CDelegateCommand(SaveDepartment); } }


        private void NewDepartment()
        {
            m_Department.IsNew = true;
            m_EditDepartment = new CDepartment();
            PropertyChanged(this, new PropertyChangedEventArgs("EditDepartment"));
            PropertyChanged(this, new PropertyChangedEventArgs("Name"));
            PropertyChanged(this, new PropertyChangedEventArgs("GroupID"));
        }

        private void DeleteDepartment()
        {
            if (m_EditDepartment == null) return;
            m_Department.Delete(m_EditDepartment.ID);
            PropertyChanged(this, new PropertyChangedEventArgs("Departments"));
        }

        //parameter:password,can not binding on passwordbox
        private void SaveDepartment(object parameter)
        {
            if (m_EditDepartment == null) return;

            try
            {
                if (parameter == null ||!(parameter is ListView)) return;


                ListView lst = parameter as ListView;

                if (m_Department.IsNew)
                {
                    m_Department.Add(m_EditDepartment);
                }
                else
                {
                    m_Department.Modify(m_EditDepartment.ID, m_EditDepartment);
                }

                PropertyChanged(this, new PropertyChangedEventArgs("Departments"));
                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_Department.IsNew = false;
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_Department.Save();
        }

        public void Get()
        {
            m_Department.Query();
        }

    }
}
