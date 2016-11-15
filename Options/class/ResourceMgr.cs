using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;


namespace TrboX
{
    public enum OperateType
    {
        add,
        list,
        count,
        del,
        update,

        listUser,
        assignUser,
        detachUser,

        listRadio,
        assignRadio,
        detachRadio,
    }

    public class Critera
    {
        [DefaultValue(null)]
        public string[][] condition;

        [DefaultValue(null)]
        public string[] sort;

        [DefaultValue(-1)]
        public int offset;

        [DefaultValue(-1)]
        public int count;
    }


    public class Belong
    {
        public int ID;
        public int Staff;
        public int Department;
        public int Radio;
    }

    public class ResList
    {
        public List<User> User;
        public List<Staff> Staff;
        public List<Department> Department;
        public List<Radio> Radio;
        public List<Belong> Belong;
    }

    public class ResourceMgr
    {
        public ResList Get()
        {
            ResList res = new ResList();
            res.User = UserMgr.List();

            res.Staff = StaffMgr.List();
            res.Department = DepartmentMgr.List();
            res.Radio = RadioMgr.List();

            res.Belong = new List<Belong>();

            List<long> allstaff = new List<long>();
            List<long> allradio = new List<long>();
            foreach (var it in res.Staff) allstaff.Add(it.id);
            foreach (var it in res.Radio) allradio.Add(it.id);

            foreach (var it in res.Department)
            {
                List<Staff>  staff = DepartmentMgr.ListStaff(it.id);
                List<Radio>  radio = DepartmentMgr.ListRadio(it.id);
                List<long> currradio = new List<long>();
                foreach (var id in radio) currradio.Add(it.id);

                foreach (var s in staff)
                {
                    allstaff.Remove(s.id);
                    List<Radio> rad = StaffMgr.ListRadio(s.id);
                    if(rad.Count > 0)
                    {
                        foreach(var r in rad)
                        {
                            currradio.Remove(r.id);
                            allradio.Remove(r.id);                           
                            res.Belong.Add(new Belong() { Department = (int)it.id, Staff = s.id, Radio= r.id });
                        }
                    }
                    else
                    {
                        res.Belong.Add(new Belong() { Department = (int)it.id, Staff = s.id, Radio = -1 });
                    }                   
                }

                foreach(var id in currradio)
                {
                    allradio.Remove(id);
                    res.Belong.Add(new Belong() { Department = (int)it.id, Staff = -1, Radio = (int)id });
                }
            }

            foreach (var it in allstaff)
            {
                List<Radio> rad = StaffMgr.ListRadio(it);
                if (rad.Count > 0)
                {
                    foreach (var r in rad)
                    {
                        allradio.Remove(r.id);
                        res.Belong.Add(new Belong() { Department = -1, Staff = (int)it, Radio = r.id });
                    }
                }
                else
                {
                    res.Belong.Add(new Belong() { Department = -1, Staff = (int)it, Radio = -1 });
                }    
            }

            foreach(var it in allradio)
            {
                res.Belong.Add(new Belong() { Department = -1, Staff = -1, Radio = (int)it });
            }

            return res;
        }

        public void Save()
        {
            UserMgr.Save();
            DepartmentMgr.Save();
            StaffMgr.Save();
            RadioMgr.Save();

            DepartmentMgr.SaveDeptStaff();
            DepartmentMgr.SaveDeptRadio();

            StaffMgr.SaveStaffRadio();
        }
    }
}
