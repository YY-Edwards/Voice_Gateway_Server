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
    public class CVMRadio:CVMManager, INotifyPropertyChanged
    {
        public CVMRadio()
        {
            if (m_Radio == null)
            {
                m_Radio = new CRadioMgr();
                m_Radio.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditRadio == null) m_EditRadio = new CRadio();           
        }


        private CRadioMgr m_Radio;
        public ObservableCollection<CRElement> Radios {  get {return new ObservableCollection<CRElement>(m_Radio.List); } }


        private CRadio m_EditRadio;
        public CRadio EditRadio
        {
            get { return m_EditRadio; }
            set
            {
                if (value == null)
                {
                    m_EditRadio = new CRadio();
                    m_Radio.IsNew = true;
                }
                else
                {
                    OrginRadio = value.Copy();
                    m_EditRadio = value;
                    m_Radio.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Type"));
                    PropertyChanged(this, new PropertyChangedEventArgs("RadioID"));
                    PropertyChanged(this, new PropertyChangedEventArgs("SN"));

                    PropertyChanged(this, new PropertyChangedEventArgs("HasScreen"));
                    PropertyChanged(this, new PropertyChangedEventArgs("HasKey"));
                    PropertyChanged(this, new PropertyChangedEventArgs("HasGPS"));
                    PropertyChanged(this, new PropertyChangedEventArgs("HasLocationInDoor"));

                    PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
                    PropertyChanged(this, new PropertyChangedEventArgs("StaffID"));
                }
            }
        }

        public int Type { set { m_EditRadio.Type = value == 0 ? RadioType.Radio : RadioType.Ride; } get { if (m_EditRadio == null)return -1; return m_EditRadio.Type == RadioType.Radio ? 0 : 1; } }
        public long RadioID { set { m_EditRadio.RadioID = value; } get { if (m_EditRadio == null)return 0; return m_EditRadio.RadioID; } }
        public string SN { set { m_EditRadio.SN = value; } get { if (m_EditRadio == null)return ""; return m_EditRadio.SN; } }

        public bool HasScreen { set { m_EditRadio.HasScreen = value; } get { if (m_EditRadio == null)return false; return m_EditRadio.HasScreen; } }
        public bool HasKey { set { m_EditRadio.HasKeyboard = value; } get { if (m_EditRadio == null)return false; return m_EditRadio.HasKeyboard; } }
        public bool HasGPS { set { m_EditRadio.HasGPS = value; } get { if (m_EditRadio == null)return false; return m_EditRadio.HasGPS; } }
        public bool HasLocationInDoor { set { m_EditRadio.HasLocationInDoor = value; } get { if (m_EditRadio == null)return false; return m_EditRadio.HasLocationInDoor; } }



        public long DepartmentID { set { m_EditRadio.DepartmentID = value; } get { if (m_EditRadio == null)return 0; return m_EditRadio.DepartmentID; } }
        public long StaffID { set { m_EditRadio.StaffID = value; } get { if (m_EditRadio == null)return 0; return m_EditRadio.StaffID; } }


        private CDepartment CurrentDept;
        private CStaff CurrentStaff;
        private CRadio OrginRadio;

        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.radio && PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Radios"));
            }
        }

        //command
        public ICommand New { get { return new CDelegateCommand(NewEle); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteEle); } }
        public ICommand Save { get { return new CDelegateCommand(SaveEle); } }

        public ICommand DepartmentChanged { get { return new CDelegateCommand(OnDepartmentChanged); } }
        public ICommand StaffChanged { get { return new CDelegateCommand(OnStaffChanged); } }


        private void NewEle()
        {
            m_Radio.IsNew = true;
            m_EditRadio = new CRadio();
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("EditRadio"));
                PropertyChanged(this, new PropertyChangedEventArgs("Type"));
                PropertyChanged(this, new PropertyChangedEventArgs("RadioID"));
                PropertyChanged(this, new PropertyChangedEventArgs("SN"));

                PropertyChanged(this, new PropertyChangedEventArgs("HasScreen"));
                PropertyChanged(this, new PropertyChangedEventArgs("HasKey"));
                PropertyChanged(this, new PropertyChangedEventArgs("HasGPS"));
                PropertyChanged(this, new PropertyChangedEventArgs("HasLocationInDoor"));

                PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
                PropertyChanged(this, new PropertyChangedEventArgs("StaffID"));               
            }
        }

        private void DeleteEle()
        {
            if (m_EditRadio == null) return;
            m_Radio.Delete(m_EditRadio.ID);

            if (m_EditRadio.ID > 0)
            {
                m_Radio.DetachDept(m_EditRadio, m_EditRadio.DepartmentID);
                m_Radio.DetachStaff(m_EditRadio, m_EditRadio.StaffID);
            }

            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("Radios"));
        }

        //parameter:password,can not binding on passwordbox
        private void SaveEle(object parameter)
        {
            if (m_EditRadio == null || !(parameter is ListView)) return;

            try
            {
                ListView lst = parameter as ListView;

                if (m_Radio.IsNew)
                {
                    if (m_EditRadio.RadioID  > 0)
                    {
                        m_Radio.Add(m_EditRadio);
                        if (CurrentDept != null) m_Radio.AssignDept(m_EditRadio, CurrentDept.GroupID);
                        if (CurrentStaff != null) m_Radio.AssignStaff(m_EditRadio, CurrentStaff.Name);
                    }
                }
                else
                {
                    if (m_EditRadio.RadioID > 0)
                    {

                        if (m_EditRadio.ID > 0)
                        {
                            if (OrginRadio != null)
                            {
                                if (OrginRadio.DepartmentID > 0) m_Radio.DetachDept(m_EditRadio, OrginRadio.DepartmentID);
                                if (OrginRadio.StaffID > 0) m_Radio.DetachDept(m_EditRadio, OrginRadio.StaffID);
                            }
                        }

                        if (CurrentDept != null) m_Radio.AssignDept(m_EditRadio, CurrentDept.GroupID);
                        if (CurrentStaff != null) m_Radio.AssignStaff(m_EditRadio, CurrentStaff.Name);

                        m_Radio.Modify(m_EditRadio.ID, m_EditRadio);
                    }
                }

                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("Radios"));
                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_Radio.IsNew = false;
        }

        private void OnDepartmentChanged(object parameter)
        {
            if (parameter == null || !(parameter is ComboBox)) return;
            if(((ComboBox)parameter).SelectedItem != null)
            {
                CurrentDept = ((ComboBox)parameter).SelectedItem as CDepartment;
                m_EditRadio.DepartmentID = CurrentDept.ID;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("DepartmentID"));
            }           
        }

        private void OnStaffChanged(object parameter)
        {
            if (parameter == null || !(parameter is ComboBox)) return;
            if (((ComboBox)parameter).SelectedItem != null)
            {
                CurrentStaff = ((ComboBox)parameter).SelectedItem as CStaff;
                m_EditRadio.StaffID = CurrentStaff.ID;
                if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("StaffID"));
            }
        }


        public void AssignDetach(List<CRElement> depts, List<CRElement> staffs)
        {
            m_Radio.AssignDetach(depts, staffs);
        }

        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_Radio.Save();            
        }

        public void Get() 
        {
            m_Radio.Query();
        }      
    }
}
