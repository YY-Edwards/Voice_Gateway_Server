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

    public class ItemIn
    {
        public int dept { set; get; }
        public int staff { set; get; }
    }
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
            RegRadio();

            m_TargetMgr.UpdateTragetList();
            m_TargetList = TargetMgr.TargetList;

            m_Main.lst_Group.Loaded += delegate { SetDepartmentToList(m_TargetList.Group); };

            m_Main.lst_Staff.Loaded += delegate {
                SetDepartmentToStaff(m_TargetList.Group); 
                SetStaffToList(m_TargetList.Staff);            
            };
            m_Main.cmb_RadioStaff.Loaded += delegate { };

            m_Main.lst_Radio.Loaded += delegate {

                SetDepartmentToRadio(m_TargetList.Group);
                SetStaffToRadio(m_TargetList.Staff); 
                SetRadioToList(m_TargetList.Radio); 
            };

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
                foreach (User user in users)m_Main.cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.UserName + (user.Type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user,
                                                                                                    Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                                                    Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                                                    FontSize = 13,
                                                                                                    Height = 32,
                });                
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
                     m_Main.cmb_AuthorityDest.Items.Add(new ComboBoxItem() { Content = user.UserName + (user.Type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)"), Tag = user,
                                                                             Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                             Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                             FontSize = 13,
                                                                             Height = 32,
                     });
                     m_Main.cmb_AuthorityDest.SelectedIndex =  m_Main.cmb_AuthorityDest.Items.Count - 1;
                }
                else
                {
                    User destuser = (User)((ListViewItem) m_Main.lst_User.SelectedItem).Content;
                    user.ID = destuser.ID;
                    user.Modify();
                    ((ComboBoxItem)m_Main.cmb_AuthorityDest.Items[m_Main.lst_User.SelectedIndex]).Content = user.UserName + (user.Type == UserType.Admin.ToString() ? "(管理员)" : "(来宾)");
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

                m_Main.lst_User.SelectedIndex = m_Main.lst_User.Items.Count == 0? -1 : 0;

           };

            m_Main.btn_AddFunc.Click +=delegate{
                if (m_Main.list_AllFunc == null || m_Main.lst_CurrentFunc == null || m_Main.list_AllFunc.SelectedItem == null) return;              

                if(m_Main.cmb_AuthorityDest != null && m_Main.cmb_AuthorityDest.SelectedItem != null)
                {
                    if (!m_Main.lst_CurrentFunc.Items.Contains(m_Main.list_AllFunc.SelectedItem as CAuthority)) m_Main.lst_CurrentFunc.Items.Add(m_Main.list_AllFunc.SelectedItem as CAuthority);

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

            m_Main.btn_DelFunc.Click += delegate
            {
                if (m_Main.list_AllFunc == null || m_Main.lst_CurrentFunc == null || m_Main.lst_CurrentFunc.SelectedItem == null) return;
                if (m_Main.cmb_AuthorityDest != null && m_Main.cmb_AuthorityDest.SelectedItem != null)
                {
                    m_Main.lst_CurrentFunc.Items.Remove(m_Main.lst_CurrentFunc.SelectedItem);

                    User user = (User)((ComboBoxItem)m_Main.cmb_AuthorityDest.SelectedItem).Tag;

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

                    m_Main.cmb_StaffDepartment.Items.Add(new ComboBoxItem() { Content = dept.Name, Tag = dept,
                                                            Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                            Foreground = new SolidColorBrush(Color.FromArgb(255, 190,195,199)),
                                                            FontSize = 13,
                                                            Height = 32,                                               
                    });
                    m_Main.cmb_RadioDepartment.Items.Add(new ComboBoxItem() { Content = dept.Name, Tag = dept,
                                                                              Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                              Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                              FontSize = 13,
                                                                              Height = 32,
                    });
                    m_Main.lst_Group.SelectedIndex = m_Main.lst_Group.Items.Count - 1;
                }
                else
                {
                    Department destdept = (Department)((ListViewItem)m_Main.lst_Group.SelectedItem).Content;
                    dept.ID = destdept.ID;
                    dept.Modify();
                    ((ListViewItem)m_Main.lst_Group.Items[m_Main.lst_Group.SelectedIndex]).Content = dept;

                    ((ComboBoxItem)m_Main.cmb_StaffDepartment.Items[m_Main.lst_Group.SelectedIndex]).Content = dept.Name;
                    ((ComboBoxItem)m_Main.cmb_StaffDepartment.Items[m_Main.lst_Group.SelectedIndex]).Tag = dept;
                    ((ComboBoxItem)m_Main.cmb_RadioDepartment.Items[m_Main.lst_Group.SelectedIndex]).Content = dept.Name;
                    ((ComboBoxItem)m_Main.cmb_RadioDepartment.Items[m_Main.lst_Group.SelectedIndex]).Tag = dept;
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
                    
                    m_Main.cmb_StaffDepartment.Items.RemoveAt(m_Main.lst_Group.SelectedIndex);
                    m_Main.cmb_RadioDepartment.Items.RemoveAt(m_Main.lst_Group.SelectedIndex);

                    m_Main.lst_Group.SelectedIndex = m_Main.lst_Group.Items.Count == 0 ? -1 : 0;
                }
            };
        }


        private ItemIn GetIndex(Staff staff)
        {
            try
            {
                int i = 0;
                if(m_Main.cmb_StaffDepartment !=null)
                {
                    foreach(var item in m_Main.cmb_StaffDepartment.Items)
                    {
                        List<Staff> staffs = DepartmentMgr.ListStaff(((Department)((ComboBoxItem)item).Tag).ID);
                        if (staffs != null && staffs.Where(p => p.ID == staff.ID).ToList().Count > 0) return new ItemIn(){dept = i};
                        i++;
                    }
                }

            }
            catch(Exception ex)
            {
                DataBase.InsertLog(ex.Message);                
            }

            return new ItemIn() { dept = -1 };
        }

        private ItemIn GetIndex(Radio radio)
        {
            int dept = -1, staff = -1;
            try
            {
                int i = 0;
                if (m_Main.cmb_StaffDepartment != null)
                {
                    foreach (var item in m_Main.cmb_RadioDepartment.Items)
                    {
                        List<Radio> radios = DepartmentMgr.ListRadio(((Department)((ComboBoxItem)item).Tag).ID);
                        if (radios != null && radios.Where(p => p.ID == radio.ID).ToList().Count > 0)
                        {
                            dept = i;
                            break;
                        }
                        i++;
                    }
                }

            }
            catch (Exception ex)
            {
                DataBase.InsertLog(ex.Message);
            }

            try
            {
                int i = 0;
                if (m_Main.cmb_StaffDepartment != null)
                {
                    foreach (var item in m_Main.cmb_RadioStaff.Items)
                    {
                        List<Radio> radios = StaffMgr.ListRadio(((Staff)((ComboBoxItem)item).Tag).ID);
                        if (radios != null && radios.Where(p => p.ID == radio.ID).ToList().Count > 0)
                        {
                            staff = i;
                            break;
                        }
                        i++;
                    }
                }

            }
            catch (Exception ex)
            {
                DataBase.InsertLog(ex.Message);
            }

            return new ItemIn() { dept = dept, staff = staff };
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

                    m_Main.lst_Group.Items.Add(new ListViewItem() { Content = item.Value.Group });
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

                    m_Main.cmb_StaffDepartment.Items.Add(new ComboBoxItem() { Content = item.Value.Group.Name, Tag = item.Value.Group ,
                                                                              Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                              Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                              FontSize = 13,
                                                                              Height = 32,
                    });
                }
            }
        }

        private void SetDepartmentToRadio(Dictionary<int, CMember> depts)
        {
            if (m_Main.cmb_RadioDepartment.Items.Count > 0) return;
            if (depts != null)
            {
                foreach (var item in depts)
                {
                    if (item.Value.Group == null) continue;
                    if (item.Value.Group.GroupID <= 0) continue;

                    m_Main.cmb_RadioDepartment.Items.Add(new ComboBoxItem() { Content = item.Value.Group.Name, Tag = item.Value.Group,
                                                                              Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                              Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                              FontSize = 13,
                                                                              Height = 32,
                    });
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
                    try
                    {
                        m_Main.cmb_RadioStaff.Items.RemoveAt(m_Main.lst_Staff.SelectedIndex);
                    }
                    catch { }
                    m_Main.lst_Staff.SelectedIndex = m_Main.lst_Staff.Items.Count == 0 ? -1 : 0;
                }
            };

            m_Main.btn_SaveStaff.Click += delegate
            {
                Staff staff = null;


                try
                {

                    int typev= int.Parse((string)((ComboBoxItem)m_Main.cmb_StaffType.SelectedItem).Tag);
                    staff = new Staff()
                    {
                        Name = m_Main.txt_StaffName.Text,
                        Type = (StaffType)typev,
                        PhoneNumber = m_Main.txt_StaffPhone.Text,
                        //IsValid = true
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

                    try
                    {
                        long groupid = ((Department)((ComboBoxItem)m_Main.cmb_StaffDepartment.SelectedItem).Tag).ID;
                        if (groupid >= 0) DepartmentMgr.AssignStaff(staff.ID, groupid);
                    }
                    catch(Exception ex) 
                    {
                        DataBase.InsertLog(ex.Message);
                    }

                    m_Main.lst_Staff.Items.Add(new ListViewItem() { Content = staff, Tag = m_Main.cmb_StaffDepartment.SelectedIndex,});
                    m_Main.lst_Staff.SelectedIndex = m_Main.lst_Staff.Items.Count - 1;

                    m_Main.cmb_RadioStaff.Items.Add(new ComboBoxItem() {Content= staff.Name + "(" + (staff.Type == StaffType.Vehicle ? "车辆" : "人员") + ")",Tag = staff,
                                                                        Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                        Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                        FontSize = 13,
                                                                        Height = 32,
                    });
                }
                else
                {
                    Staff deststaff = ((ListViewItem)m_Main.lst_Staff.SelectedItem).Content as Staff;
                    staff.ID = deststaff.ID;
                    staff.Modify();

                    try
                    {
                        try
                        {
                            Department dept = ((ComboBoxItem)m_Main.cmb_StaffDepartment.Items[GetIndex(staff).dept]).Tag as Department;
                            DepartmentMgr.DetachStaff(staff.ID, dept.ID);
                        }
                        catch
                        {

                        }
                        long groupid = ((Department)((ComboBoxItem)m_Main.cmb_StaffDepartment.SelectedItem).Tag).ID;
                        if (groupid >= 0) DepartmentMgr.AssignStaff(staff.ID, groupid);

                        ((ListViewItem)m_Main.lst_Staff.Items[m_Main.lst_Staff.SelectedIndex]).Content = staff;
                        ((ListViewItem)m_Main.lst_Staff.Items[m_Main.lst_Staff.SelectedIndex]).Tag = m_Main.cmb_StaffDepartment.SelectedIndex;

                        ((ComboBoxItem)m_Main.cmb_RadioStaff.Items[m_Main.lst_Staff.SelectedIndex]).Content = staff.Name + "(" + (staff.Type == StaffType.Vehicle ? "车辆" : "人员") + ")";
                        ((ComboBoxItem)m_Main.cmb_RadioStaff.Items[m_Main.lst_Staff.SelectedIndex]).Tag = staff;
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
                    m_Main.lst_Staff.Items.Add(new ListViewItem() { Content = item.Value.Staff, Tag = GetIndex(item.Value.Staff).dept });
                }
                m_Main.lst_Staff.SelectedIndex = 0;
            }
        }

        private void SetStaffToRadio(Dictionary<int, CMember> staffs)
        {
            if (m_Main.cmb_RadioStaff.Items.Count > 0) return;
            if (staffs != null)
            {
                foreach (var item in staffs)
                {
                    if (item.Value.Staff == null) continue;
                    if (item.Value.Staff.ID <1 ) continue;
                    m_Main.cmb_RadioStaff.Items.Add(new ComboBoxItem() { Content = item.Value.Staff.Name + "(" + (item.Value.Staff.Type == StaffType.Vehicle ? "车辆" :"人员") + ")", Tag = item.Value.Staff ,
                                                                         Style = App.Current.Resources["ComboBoxItemStyleNormal"] as Style,
                                                                         Foreground = new SolidColorBrush(Color.FromArgb(255, 190, 195, 199)),
                                                                         FontSize = 13,
                                                                         Height = 32,
                    });
                }
                m_Main.lst_Staff.SelectedIndex = 0;
            }
        }
        


        private void RegRadio()
        {

            m_Main.btn_AddRadio.Click +=delegate
            {
                m_Main.lst_Radio.SelectedIndex = -1;
            };
            m_Main.btn_DelRadio.Click += delegate
            {
                if (m_Main.lst_Radio == null || m_Main.lst_Radio.SelectedItem == null) return;
                Radio radio = ((ListViewItem)m_Main.lst_Radio.SelectedItem).Content as Radio;
                if (radio != null)
                {
                    radio.Delete();
                    m_Main.lst_Radio.Items.RemoveAt(m_Main.lst_Radio.SelectedIndex);
                    m_Main.lst_Radio.SelectedIndex = m_Main.lst_Radio.Items.Count == 0 ? -1 : 0;
                }
            };

            m_Main.btn_SaveRadio.Click += delegate
            {
                Radio radio = null;
                try
                {
                    radio = new Radio()
                    {
                        Type = m_Main.cmb_RadioType.SelectedIndex == 1 ? RadioType.Ride : RadioType.Radio,
                        RadioID = long.Parse(m_Main.txt_RadioID.Text),
                        HasGPS = (bool)m_Main.chk_HasGPS.IsChecked,
                        HasScreen = (bool)m_Main.chk_HasScreen.IsChecked,
                        HasKeyboard = (bool)m_Main.chk_HasKeyboard.IsChecked, 
                        SN = m_Main.txt_RadioSN.Text,
                        IsValid = true
                    };
                }
                catch (Exception ex)
                {
                    DataBase.InsertLog(ex.Message);
                }
                if (radio == null) return;

                if (m_Main.lst_Radio.SelectedIndex == -1)
                {
                    radio.ID = (int)radio.Add();

                    try
                    {
                        long groupid = ((Department)((ComboBoxItem)m_Main.cmb_RadioDepartment.SelectedItem).Tag).ID;
                        if (groupid >= 0) DepartmentMgr.AssignRadio(radio.ID, groupid);
                    }
                    catch (Exception ex)
                    {
                        DataBase.InsertLog(ex.Message);
                    }

                    try
                    {
                        long staffid = ((Staff)((ComboBoxItem)m_Main.cmb_RadioStaff.SelectedItem).Tag).ID;
                        if (staffid >= 0) StaffMgr.AssignRadio(radio.ID, staffid);
                    }
                    catch (Exception ex)
                    {
                        DataBase.InsertLog(ex.Message);
                    }

                    m_Main.lst_Radio.Items.Add(new ListViewItem() { Content = radio, Tag = new ItemIn(){dept = m_Main.cmb_RadioDepartment.SelectedIndex, staff =m_Main.cmb_RadioStaff.SelectedIndex} });
                    m_Main.lst_Radio.SelectedIndex = m_Main.lst_Radio.Items.Count - 1;
                }
                else
                {
                    Radio destradio = ((ListViewItem)m_Main.lst_Radio.SelectedItem).Content as Radio;
                    radio.ID = destradio.ID;
                    radio.Modify();

                    try
                    {
                        ItemIn index = GetIndex(radio);

                        try
                        {
                            Department dept = ((ComboBoxItem)m_Main.cmb_RadioDepartment.Items[index.dept]).Tag as Department;
                            DepartmentMgr.DetachRadio(radio.ID, dept.ID);
                        }
                        catch
                        {

                        }

                        try
                        {
                            Staff staff = ((ComboBoxItem)m_Main.cmb_RadioStaff.Items[index.staff]).Tag as Staff;
                            StaffMgr.DetachRadio(radio.ID, staff.ID);
                        }
                        catch
                        {

                        }

                        long groupid = ((Department)((ComboBoxItem)m_Main.cmb_RadioDepartment.SelectedItem).Tag).ID;
                        if (groupid >= 0) DepartmentMgr.AssignRadio(radio.ID, groupid);

                        long staffid = ((Staff)((ComboBoxItem)m_Main.cmb_RadioStaff.SelectedItem).Tag).ID;
                        if (staffid >= 0) StaffMgr.AssignRadio(radio.ID, staffid);
                    }
                    catch (Exception ex)
                    {
                        DataBase.InsertLog(ex.Message);
                    }

                    ((ListViewItem)m_Main.lst_Radio.SelectedItem).Content = radio;
                    ((ListViewItem)m_Main.lst_Radio.SelectedItem).Tag = new ItemIn() { dept = m_Main.cmb_RadioDepartment.SelectedIndex, staff = m_Main.cmb_RadioStaff.SelectedIndex };          
                }
            };
        }

        private void SetRadioToList(Dictionary<int, CMember> radios)
        {
            if (m_Main.lst_Radio.Items.Count > 0) return;
            if (radios != null)
            {
                foreach (var item in radios)
                {
                    if (item.Value.Radio == null) continue;
                    if (item.Value.Radio.ID < 1) continue;
                    m_Main.lst_Radio.Items.Add(new ListViewItem() { Content = item.Value.Radio, Tag = GetIndex(item.Value.Radio) });
                }
                m_Main.lst_Radio.SelectedIndex = 0;
            }
        }
    }
}
