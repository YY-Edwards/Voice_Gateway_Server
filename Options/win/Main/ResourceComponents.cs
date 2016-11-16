using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Resources;

namespace TrboX
{
    public class ResourceComponents
    {
        private Main m_Main;
        private TargetMgr m_TargetMgr = new TargetMgr();
        private CTargetRes m_TargetList;
        private List<User> m_Users;

        public ResourceComponents(Main main)
        {
            if (null != main) m_Main = main;

            RegUser();
            m_Users = UserMgr.List();
            m_Main.list_AllFunc.Loaded += delegate { m_Main.list_AllFunc.ItemsSource = User.AddAuth; };
            m_Main.lst_User.Loaded += delegate { SetUserToList(m_Users); };
            m_Main.cmb_AuthorityDest.Loaded += delegate { SetUserToFunc(m_Users); };

            RegDepartment();
            RegStaff();

            m_TargetMgr.UpdateTragetList();
            m_TargetList = TargetMgr.TargetList;

            m_Main.lst_Group.Loaded += delegate { SetDepartmentToList(m_TargetList.Group); };
            m_Main.cmb_StaffDepartment.Loaded += delegate { SetDepartmentToStaff(m_TargetList.Group); };

            m_Main.lst_Staff.Loaded += delegate { SetStaffToList(m_TargetList.Staff); };

        }

        private void SetUserToList(List<User> users)
        {
            if ( m_Main.lst_User.Items.Count > 0) return;           
            if (users != null)
            {
                foreach (User user in users)m_Main.lst_User.Items.Add(new ListViewItem() { Content = user });              
                m_Main.lst_User.SelectedIndex = 0;
            }
        }

        private void SetUserToFunc(List<User> users)
        {
            if (m_Main.cmb_AuthorityDest.Items.Count > 0) return;
            if (users != null)
            {
                foreach (User user in users)m_Main.cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.UserName + (user.Type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user });                
                m_Main.cmb_AuthorityDest.SelectedIndex = 0;
            }
        }

        private void RegUser()
        {
            m_Main.btn_UserNew.Click +=delegate{
                m_Main.lst_User.SelectedIndex = -1;
                m_Main.txt_UserName.IsReadOnly = false;
                m_Main.txt_UserName.Text = "";
                m_Main.psd_UserPassword.Password = "";
            };
            m_Main.lst_User.SelectionChanged +=delegate{ m_Main.txt_UserName.IsReadOnly = true;};


            m_Main.btn_SaveUser.Click +=delegate{
                User user = new User( 
                    m_Main.txt_UserName.Text
                    ,  m_Main.psd_UserPassword.Password
                    ,  m_Main.cmb_UserType.SelectedIndex == 1 ? UserType.Admin : UserType.Guest);

                if ( m_Main.lst_User.SelectedIndex == -1)
                {
                    user.ID = user.Add();
                     m_Main.lst_User.Items.Add(new ListViewItem() { Content = user });
                     m_Main.lst_User.SelectedIndex =  m_Main.lst_User.Items.Count - 1;
                     m_Main.cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.UserName + (user.Type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user });
                     m_Main.cmb_AuthorityDest.SelectedIndex =  m_Main.cmb_AuthorityDest.Items.Count - 1;
                }
                else
                {
                    User destuser = (User)((ListViewItem) m_Main.lst_User.SelectedItem).Content;
                    user.ID = destuser.ID;
                    user.Modify();

                    ((ComboBoxItem) m_Main.cmb_AuthorityDest.Items[ m_Main.lst_User.SelectedIndex]).Tag = user;
                    ((ListViewItem) m_Main.lst_User.Items[ m_Main.lst_User.SelectedIndex]).Content = user;
                } 
            };

           m_Main.btn_UserDel.Click +=delegate{
               if (m_Main.lst_User == null || m_Main.lst_User.SelectedItem == null) return;
               User user = ((ListViewItem)m_Main.lst_User.SelectedItem).Content as User;
                user.Delete();
                int index = m_Main.lst_User.SelectedIndex;
                m_Main.lst_User.Items.RemoveAt(index);
                m_Main.cmb_AuthorityDest.Items.RemoveAt(index);
           };

            m_Main.btn_AddFunc.Click +=delegate{
                if (m_Main.list_AllFunc == null || m_Main.lst_CurrentFunc == null || m_Main.list_AllFunc.SelectedItem == null) return;
                if (!m_Main.lst_CurrentFunc.Items.Contains(m_Main.list_AllFunc.SelectedItem as CAuthority)) m_Main.lst_CurrentFunc.Items.Add(m_Main.list_AllFunc.SelectedItem as CAuthority);

                if(m_Main.cmb_AuthorityDest != null && m_Main.cmb_AuthorityDest.SelectedItem != null)
                {
                    User  user =   (User)((ComboBoxItem)m_Main.cmb_AuthorityDest.SelectedItem).Tag;
                                 
                    List<CAuthority> lst = new List<CAuthority>();

                    foreach (var it in m_Main.lst_CurrentFunc.Items)
                    {
                        lst.Add(it as CAuthority);
                    }
                    user.Auth = lst;
                    user.Func = user.parseAuth();

                    ((ComboBoxItem)m_Main.cmb_AuthorityDest.Items[m_Main.cmb_AuthorityDest.SelectedIndex]).Tag = user;
                    ((ListViewItem)m_Main.lst_User.Items[m_Main.cmb_AuthorityDest.SelectedIndex]).Content = user;
                    user.Modify();
                }
            };

            m_Main.cmb_AuthorityDest.SelectionChanged +=delegate{
                try
                {
                    List<CAuthority> lst = ((User)((ComboBoxItem)m_Main.cmb_AuthorityDest.SelectedItem).Tag).Auth;
                    m_Main.lst_CurrentFunc.Items.Clear();
                    foreach(CAuthority it in lst)
                    {
                        if (!m_Main.lst_CurrentFunc.Items.Contains(it)) m_Main.lst_CurrentFunc.Items.Add(it);
                    }
                }
                catch { }
            };
        }

        private void RegDepartment()
        {
            m_Main.btn_SaveDepartment.Click +=delegate{     
                Department dept = null;
                try
                {
                    dept = new Department(m_Main.txt_GroupName.Text, long.Parse(m_Main.txt_GroupId.Text));
                }
                catch(Exception ex)
                {
                    DataBase.InsertLog(ex.Message);
                }
                if (dept == null) return;

                if (m_Main.lst_Group.SelectedIndex == -1)
                {
                    dept.ID = dept.Add();
                    m_Main.lst_Group.Items.Add(new ListViewItem() { Content = dept });
                    m_Main.lst_Group.SelectedIndex = m_Main.lst_Group.Items.Count - 1;
                }
                else
                {
                    Department destdept = (Department)((ListViewItem)m_Main.lst_Group.SelectedItem).Content;
                    dept.ID = destdept.ID;
                    dept.Modify();
                    ((ListViewItem)m_Main.lst_Group.Items[m_Main.lst_Group.SelectedIndex]).Content = dept;
                }       
            };

           m_Main.btn_DepartmentNew.Click+=delegate{
               m_Main.lst_Group.SelectedIndex = -1;
            };

           m_Main.btn_DepartmentDel.Click += delegate
           {
               if (m_Main.lst_Group == null || m_Main.lst_Group.SelectedItem == null) return;
               Department dept = ((ListViewItem)m_Main.lst_Group.SelectedItem).Content as Department;
                if (dept != null)
                {
                    dept.Delete();
                    m_Main.lst_Group.Items.RemoveAt(m_Main.lst_Group.SelectedIndex);
                }
            };
        }

        private void SetDepartmentToList(Dictionary<int, CMember> depts)
        {
            if (m_Main.lst_Group.Items.Count > 0) return;
            if (depts != null)
            {
                foreach (var item in depts)
                {
                    if (item.Value.Group == null) continue;
                    if (item.Value.Group.GroupID <= 0) continue;

                    m_Main.lst_Group.Items.Add(new ListViewItem() { Content = item.Value });
                }
                m_Main.lst_Group.SelectedIndex = 0;
            }         
        }

        private void SetDepartmentToStaff(Dictionary<int, CMember> depts)
        {
            if (m_Main.cmb_StaffDepartment.Items.Count > 0) return;
            if (depts != null)
            {
                foreach (var item in depts)
                {
                    if (item.Value.Group == null) continue;
                    if (item.Value.Group.GroupID <= 0) continue;

                    m_Main.cmb_StaffDepartment.Items.Add(new ComboBoxItem() { Content = item.Value.NameInfo, Tag = item.Value });
                }
            }
        }


        private void RegStaff()
        {
            m_Main.btn_StaffNew.Click += delegate { m_Main.lst_Staff.SelectedIndex = -1; };
            m_Main.btn_StaffDel.Click += delegate
            {
                if (m_Main.lst_Staff == null || m_Main.lst_Staff.SelectedItem == null) return;
                Staff staff = ((ListViewItem)m_Main.lst_Staff.SelectedItem).Content as Staff;
                if (staff != null)
                {
                    staff.Delete();
                    m_Main.lst_Staff.Items.RemoveAt(m_Main.lst_Staff.SelectedIndex);
                }
            };

            m_Main.btn_SaveStaff.Click += delegate
            {
                Staff staff = null;
                try
                {
                    staff = new Staff()
                    {
                        Name = m_Main.txt_StaffName.Text,
                        Type = ((StaffType)int.Parse((string)((ComboBoxItem)m_Main.cmb_StaffType.SelectedItem).Tag)).ToString(),
                        PhoneNumber = m_Main.txt_StaffPhone.Text,
                        IsValid = true
                    };
                }
                catch (Exception ex)
                {
                    DataBase.InsertLog(ex.Message);
                }
                if (staff == null) return;

                if (m_Main.lst_Staff.SelectedIndex == -1)
                {
                    staff.ID = (int)staff.Add();

                    m_Main.lst_Staff.Items.Add(new ListViewItem() { Content = staff });
                    m_Main.lst_Staff.SelectedIndex = m_Main.lst_Staff.Items.Count - 1;

                    try
                    {
                        DepartmentMgr.AssignStaff(staff, ((CMember)((ComboBoxItem)m_Main.cmb_StaffDepartment.SelectedItem).Tag).Group.ID);
                    }
                    catch(Exception ex) 
                    {
                        DataBase.InsertLog(ex.Message);
                    }
                }
                else
                {
                    Staff deststaff = (Staff)((ListViewItem)m_Main.lst_Staff.SelectedItem).Content;
                    staff.ID = deststaff.ID;
                    staff.Modify();

                    try
                    {
                        DepartmentMgr.AssignStaff(staff, ((CMember)((ComboBoxItem)m_Main.cmb_StaffDepartment.SelectedItem).Tag).Group.ID);
                    }
                    catch (Exception ex)
                    {
                        DataBase.InsertLog(ex.Message);
                    }
                }       
            };
        }

        private void SetStaffToList(Dictionary<int, CMember> staffs)
        {
            if (m_Main.lst_Staff.Items.Count > 0) return;
            if (staffs != null)
            {
                foreach (var item in staffs)
                {
                    if (item.Value.Staff == null) continue;
                    if (item.Value.Staff.ID <1 ) continue;
                    if (item.Value.Staff.Type == StaffType.Vehicle.ToString()) continue;
                    m_Main.lst_Staff.Items.Add(new ListViewItem() { Content = item.Value });
                }
                m_Main.lst_Staff.SelectedIndex = 0;
            }
        }
    }
}
