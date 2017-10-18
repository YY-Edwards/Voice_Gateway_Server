using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

using Dispatcher.ViewsModules;
using Newtonsoft.Json;
using Sigmar.Encrypt;


using Dispatcher.Modules;
using Sigmar.Logger;

namespace Dispatcher.Service
{
    public class SaveWork : INotifyPropertyChanged
    {
        private volatile static SaveWork _instance = null;
        private static readonly object lockHelper = new object();

        public static SaveWork Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new SaveWork();
                }
            }
            return _instance;
        }


        private bool _isneedsavequickpanel = false;

        public bool IsNeedSaveQuickPanel { get { return _isneedsavequickpanel; } set { _isneedsavequickpanel = value; NotifyPropertyChanged("EnableSaveWorkSpace"); } }
        public bool EnableSaveWorkSpace { get { return IsNeedSaveQuickPanel; } }

        public void Save(VMMain vmmain)
        {
            List<VMQuickItem> quicklist = vmmain.QuickListViewModule.Quick;
            SaveQuickList(quicklist);
            NotifyPropertyChanged("EnableSaveWorkSpace");
            Log.Message("保存工作区成功");
        }

        private enum QuickAllType_t
        {
            Member,
            Group,
            Operation,
        }

        private struct QuickSaveStru_t
        {
            public QuickAllType_t Type;
            public object Content;

            public QuickSaveStru_t(QuickAllType_t type, object content)
            {
                Type = type;
                Content = content;
            }
        }

        private struct QuickOperaton_t
        {
            public TaskType_t Opcode;
            public List<QuickSaveStru_t> Targets;
            public object Agrs;

            public QuickOperaton_t(TaskType_t op, object args = null, List<QuickSaveStru_t> targets = null)
            {
                Opcode = op;
                Targets = targets;
                Agrs = args;
            }

        }

        public void SaveQuickList(List<VMQuickItem> quicklist)
        {
            List<QuickSaveStru_t> willsave = new List<QuickSaveStru_t>();
            foreach (VMQuickItem quick in quicklist)
            { 
                if(quick.Type == QuickPanelType_t.Target)
                {
                    if (quick.Target.TargetType == VMTarget.TargetType_t.Member) willsave.Add(new QuickSaveStru_t(QuickAllType_t.Member, quick.Target.Member));
                    else if (quick.Target.TargetType == VMTarget.TargetType_t.Group) willsave.Add(new QuickSaveStru_t(QuickAllType_t.Group, quick.Target.Group));
                }
                else if(quick.Type == QuickPanelType_t.Operation)
                {
                    if (quick.Operation == null) continue;
                    if (quick.Operation.Targets == null) willsave.Add(new QuickSaveStru_t(QuickAllType_t.Operation, new QuickOperaton_t(quick.Operation.Operation.Type,quick.Operation.Operation.Args )));
                    else
                    {
                        List<QuickSaveStru_t> optargets = new List<QuickSaveStru_t>();
                        foreach(VMTarget target in quick.Operation.Targets)
                        {
                            if (target.TargetType == VMTarget.TargetType_t.Member) optargets.Add(new QuickSaveStru_t(QuickAllType_t.Member, target.Member));
                            else if (target.TargetType == VMTarget.TargetType_t.Group) optargets.Add(new QuickSaveStru_t(QuickAllType_t.Group, target.Group));
                        }
                        willsave.Add(new QuickSaveStru_t(QuickAllType_t.Operation, new QuickOperaton_t(quick.Operation.Operation.Type, quick.Operation.Operation.Args, optargets)));
                    }
                }
            }
            
            
            if (!IsNeedSaveQuickPanel) return;
            string json = JsonConvert.SerializeObject(willsave);
            byte[] base64 = Base64.Encoding(Encoding.UTF8.GetBytes(json));
            System.IO.File.WriteAllBytes(App.QuickListPath, base64);

            _isneedsavequickpanel = false;
        }

        public List<VMQuickItem> ReadQuickList(List<VMTarget> res)
        {
            List<VMQuickItem> quicklist = new List<VMQuickItem>();
            
            if(!System.IO.File.Exists(App.QuickListPath))return new List<VMQuickItem>();



            byte[] base64 = System.IO.File.ReadAllBytes(App.QuickListPath);
            if (base64 == null) return new List<VMQuickItem>();

            try
            {
                string json = Encoding.UTF8.GetString(Base64.Decoding(base64));
                List<QuickSaveStru_t> read =JsonConvert.DeserializeObject<List<QuickSaveStru_t>>(json);
                foreach(QuickSaveStru_t item in read)
                {
                    if(item.Content == null)continue;
                    string contentstr = JsonConvert.SerializeObject(item.Content);
                    
                    switch(item.Type)
                    {
                        case QuickAllType_t.Member:
                            CMember member = JsonConvert.DeserializeObject<CMember>(contentstr);
                            if (member == null) break;
                            VMTarget vmmember = res.Find(p => p.TargetType == VMTarget.TargetType_t.Member && p.Member.ID == member.ID);
                            if (vmmember != null) quicklist.Add(new VMQuickItem(vmmember));                          
                            break;
                        case QuickAllType_t.Group:
                            CGroup group = JsonConvert.DeserializeObject<CGroup>(contentstr);
                            if (group == null) break;
                            VMTarget vmgroup = res.Find(p => p.TargetType == VMTarget.TargetType_t.Group && p.Group.ID == group.ID);
                            if (vmgroup != null) quicklist.Add(new VMQuickItem(vmgroup));
                            break;
                        case QuickAllType_t.Operation:
                            QuickOperaton_t operation = JsonConvert.DeserializeObject<QuickOperaton_t>(contentstr);
                            OperationAgrs Args = null;

                            switch(operation.Opcode)
                            {
                                case TaskType_t.Schedule:Args = JsonConvert.DeserializeObject<CallArgs>(JsonConvert.SerializeObject(operation.Agrs));break;                          
                                case TaskType_t.ShortMessage:Args = JsonConvert.DeserializeObject<ShortMessageArgs>(JsonConvert.SerializeObject(operation.Agrs));break;                                      
                                case TaskType_t.Controler:Args = JsonConvert.DeserializeObject<ControlArgs>(JsonConvert.SerializeObject(operation.Agrs));break;                                      
                                case TaskType_t.Location:Args = JsonConvert.DeserializeObject<LocationArgs>(JsonConvert.SerializeObject(operation.Agrs));break;                                    
                                case TaskType_t.LocationInDoor:Args = JsonConvert.DeserializeObject<LocationInDoorArgs>(JsonConvert.SerializeObject(operation.Agrs));break;   
                                default:break;
                            }


                            if(operation.Targets == null)
                            {
                                VMOperation op = new VMOperation(new COperation(operation.Opcode, Args));
                                if(op != null)quicklist.Add(new VMQuickItem(op));
                            }
                            else
                            {  
                                List<VMTarget> Targets = new List<VMTarget>();

                                foreach(QuickSaveStru_t target in operation.Targets)
                                {
                                    switch(target.Type)
                                    {
                                        case QuickAllType_t.Member:
                                            CMember membertag = JsonConvert.DeserializeObject<CMember>(JsonConvert.SerializeObject(target.Content));
                                            if (membertag == null) break;
                                            VMTarget vmmembertag = res.Find(p => p.TargetType == VMTarget.TargetType_t.Member && p.Member.ID == membertag.ID);
                                            if(vmmembertag != null)Targets.Add(vmmembertag);                                                            
                                            break;
                                        case QuickAllType_t.Group:
                                            CGroup grouptag = JsonConvert.DeserializeObject<CGroup>(JsonConvert.SerializeObject(target.Content));
                                            if (grouptag == null) break;
                                            VMTarget vmgrouptag = res.Find(p => p.TargetType == VMTarget.TargetType_t.Group && p.Group.ID == grouptag.ID);
                                            if(vmgrouptag != null)Targets.Add(vmgrouptag);                                                            
                                            break;
                                      }
                                }

                                VMOperation op = new VMOperation(new COperation(operation.Opcode,Args),Targets);
                                if(op != null)quicklist.Add(new VMQuickItem(op));
                            }
                            break;
                    }
                }
            }
            catch(Exception ex)
            {
                Console.Write(ex.Message);
            }

            return quicklist;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private  void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
