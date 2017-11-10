using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Sigmar.Logger;
using System.Threading;
using Dispatcher.Modules;
using Dispatcher.ViewsModules;

namespace Dispatcher.Service
{
    public class ResourcesMgr
    {
        private volatile static ResourcesMgr _instance = null;
        private static readonly object lockHelper = new object();

        public static ResourcesMgr Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new ResourcesMgr();
                }
            }
            return _instance;
        }

        private static long DeviceMask = 0x0000000000000000;
        private static long ManCarMask = 0x1000000000000000;
        private static long GroupMask = 0x2000000000000000;


        public event EventHandler OnResourcesLoaded;

        public VMTarget AllTarget;
        public List<VMTarget> Groups = new List<VMTarget>();
        public List<VMTarget> Members = new List<VMTarget>();

        public List<VMArea> Areas = new List<VMArea>();
        public List<VMBeacon> Beacons = new List<VMBeacon>();

        public List<VMTarget> All
        {
            get
            {
                List<VMTarget> all = new List<VMTarget>();
                all.Add(AllTarget);
                all.AddRange(Groups);
                all.AddRange(Members);
                return all;
            }
        }

        public bool IsInitialized = false;


        private CDepartmentMgr _groupmgr;
        private CRadioMgr _radiomgr;
        private CStaffMgr _staffmgr;

        private CAreaMgr _areamgr;
        private CBeaconMgr _beaconmgr;

        public ResourcesMgr()
        {

            if(_groupmgr == null)
            {
                _groupmgr = new CDepartmentMgr();
                _groupmgr.OnResourceChanged += new CResource.ResourceChangedHandle(OnResourceChanged);
            }

            if(_radiomgr == null)
            {
                _radiomgr = new CRadioMgr();
                _radiomgr.OnResourceChanged += new CResource.ResourceChangedHandle(OnResourceChanged);
            }

            if(_staffmgr == null)
            {
                _staffmgr = new CStaffMgr();
                _staffmgr.OnResourceChanged += new CResource.ResourceChangedHandle(OnResourceChanged);
            }

            if(_areamgr == null)
            {
                _areamgr = new CAreaMgr();
                _areamgr.OnResourceChanged += new CResource.ResourceChangedHandle(OnResourceChanged);
            }

            if (_beaconmgr == null)
            {
                _beaconmgr = new CBeaconMgr();
                _beaconmgr.OnResourceChanged += new CResource.ResourceChangedHandle(OnResourceChanged);
            }
        }

        private Semaphore _waitloadedresources;

        private void OnResourceChanged(RequestOpcode type, List<CElement> res)
        {
            _waitloadedresources.Release();
            //switch(type)
            //{
            //    case RequestOpcode.department:
            //        break;
            //}
        }

        public void InitializeResources(VMResources vmresource)
        {
            if (IsInitialized) return;
            IsInitialized = true;

            if (!CLogServer.Instance().IsInitialized) return;
            _waitloadedresources = new Semaphore(0, 1);
            
            new Thread(new ThreadStart(delegate()
            {                              
                while(true)
                {

                    try
                    {
                        _groupmgr.Query();
                        _waitloadedresources.WaitOne();
                        _radiomgr.Query();
                        _waitloadedresources.WaitOne();
                        _staffmgr.Query();
                        _waitloadedresources.WaitOne();
                        // CDepartmentMgr

                        Groups.Clear();
                        if(_groupmgr.List != null)
                        {
                            foreach(CDepartment department in _groupmgr.List)
                            {
                                Groups.Add(new VMTarget(new CGroup() { GroupID = department.GroupID, ID = department.ID | GroupMask, Name = department.Name }));
                            }
                        }

                        foreach (VMTarget group in Groups)
                        {
                            //group.RegisterLoggerEvents(vmresource);
                            group.OnOperated += new OperatedEventHandler(vmresource.OnChildOperated);
                        }


                        List<CElement> NoBounded= new List<CElement>();
                        if(_staffmgr.List != null)
                        NoBounded = _staffmgr.List.FindAll(p => true);


                        Members.Clear();
                        if(_radiomgr.List != null)
                        {
                            foreach(CRadio radio in _radiomgr.List)
                            {
                                VMTarget group = Groups != null ? Groups.Find(p => p.ID == (radio.DepartmentID | GroupMask)) : null;

                                CStaff staff = NoBounded.Find(p => p.ID == radio.StaffID) as CStaff;
                                NoBounded.RemoveAll(p => p.ID == radio.StaffID);

                                if(staff != null)
                                {
                                    
                                    Members.Add(new VMTarget(new CMember() {
                                        GroupID = group == null ? 0 : group.Group.GroupID,
                                        ManCarName = staff.Name,
                                        ManCarID = staff.ID | ManCarMask,
                                        RadioID = radio.RadioID,
                                        DeviceID = radio.ID | DeviceMask
                                    }
                                    .SetMancarType(staff.Type == StaffType.Staff ? Mancar.ManCarType_t.Staff : Mancar.ManCarType_t.Vehicle)
                                    .SetDeviceType(radio.Type == RadioType.Radio ? Device.DeviceType_t.Handset : Device.DeviceType_t.VehicleStation)
                                    .SetParts(radio.HasScreen, radio.HasKeyboard, radio.HasGPS, radio.HasLocationInDoor) as CMember));
                                }
                                else
                                {
                                    Members.Add(new VMTarget(new CMember()
                                    {
                                        GroupID = group == null ? 0 : group.Group.GroupID,
                                        RadioID = radio.RadioID,
                                        DeviceID = radio.ID | DeviceMask
                                    }
                                  .SetDeviceType(radio.Type == RadioType.Radio ? Device.DeviceType_t.Handset : Device.DeviceType_t.VehicleStation, false)
                                  .SetParts(radio.HasScreen, radio.HasKeyboard, radio.HasGPS, radio.HasLocationInDoor) as CMember));                              
                                }
                            }

                            if(NoBounded.Count > 0)
                            {
                                foreach (CStaff staff in NoBounded)
                                {
                                    if (staff != null)
                                    {
                                        Members.Add(new VMTarget(new CMember()
                                        {
                                            GroupID = staff.DepartmentID,
                                            ManCarName = staff.Name,
                                            ManCarID = staff.ID | ManCarMask,
                                        }
                                        .SetMancarType(staff.Type == StaffType.Staff ? Mancar.ManCarType_t.Staff : Mancar.ManCarType_t.Vehicle, false)as CMember));
                                    }                                  
                                }

                            }
                        }



                        foreach (VMTarget member in Members)
                        {
                            //member.RegisterLoggerEvents(vmresource);
                            member.OnOperated += new OperatedEventHandler(vmresource.OnChildOperated);
                        }

                        AllTarget = new VMTarget(new CGroup(true) { Name = "所有设备", ByName = "用户/设备" });
                        //AllTarget.RegisterLoggerEvents(vmresource);
                        AllTarget.OnOperated += new OperatedEventHandler(vmresource.OnChildOperated);



                        _areamgr.Query();
                        _waitloadedresources.WaitOne();

                        _beaconmgr.Query();
                        _waitloadedresources.WaitOne();

                        if (_areamgr.List != null) foreach (CArea area in _areamgr.List) Areas.Add(new VMArea(area));
                        if (_beaconmgr.List != null) foreach (CBeacon bracon in _beaconmgr.List) Beacons.Add(new VMBeacon(bracon));

                    }

                    catch(Exception ex)
                    {
                        //WARNING("Loaded resources failure,redo it after 10s");
                        Thread.Sleep(10000);
                        continue;
                    }
                    break;
                }
                
                if (OnResourcesLoaded != null) OnResourcesLoaded(this, null);
            })).Start();            
        }

        public VMTarget CreateTarget(int radioid, NotifyKey_t notify = NotifyKey_t.Alarm)
        {
            CMember member = new CMember() { RadioID = radioid };
            
            switch(notify)
            {
                case NotifyKey_t.ShortMessage:
                    member.SetParts(true, true, false, false);
                    break;
                case NotifyKey_t.LocationInDoor:
                    member.SetParts(false, false, false, true);
                    break;
                case NotifyKey_t.Location:
                    member.SetParts(false, false, true, false);
                    break;
            }

            VMTarget target = new VMTarget(member.UpdateStatus(ChangedKey_t.OnlineStatus, true) as CMember);
            Members.Add(target);
            if (OnResourcesLoaded != null) OnResourcesLoaded(this, null);

            return target;
        }
    }
}
