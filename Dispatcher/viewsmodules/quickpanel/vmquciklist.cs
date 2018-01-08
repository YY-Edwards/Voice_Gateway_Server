using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;

using Sigmar;
using Dispatcher;
using Sigmar.Extension;

using Dispatcher.Service;
using Dispatcher.Modules;
using Dispatcher.Views;

namespace Dispatcher.ViewsModules
{
    public class VMQuickList :INotifyPropertyChanged
    {
        public event OperatedEventHandler OnOperated;

        private List<VMQuickItem> quicklist;
        public List<VMQuickItem> Quick { get { return quicklist; } }

        private ICollectionView _quicklist;
        public ICollectionView QuickList { get{ return _quicklist; } private set{_quicklist = value ; NotifyPropertyChanged("QuickList");} }
        public ICollectionView AllTarget { get { return new ListCollectionView(ResourcesMgr.Instance().All); } }
      
        public ICollectionView QuickTargetList 
        { 
            get
            {
                if (quicklist == null) quicklist = new List<VMQuickItem>();

                if (addlist == null) addlist = new List<VMQuickItem>();
                if (removelist == null) removelist = new List<VMQuickItem>();
                quicklist.RemoveAll(p => removelist.Contains(p));
                quicklist.RemoveAll(p => addlist.Contains(p));
                quicklist.AddRange(addlist);

                List<VMQuickItem> tmpquicklist = quicklist.FindAll(p => p.Type == QuickPanelType_t.Target);
                return new ListCollectionView(tmpquicklist); ;
            }
        }

        public ICollectionView QuickOperationList
        {
            get
            {
                if (quicklist == null) quicklist = new List<VMQuickItem>();
               
                if (addlist == null) addlist = new List<VMQuickItem>();
                if (removelist == null) removelist = new List<VMQuickItem>();
                quicklist.RemoveAll(p => removelist.Contains(p));
                quicklist.RemoveAll(p => addlist.Contains(p));
                quicklist.AddRange(addlist);

                List<VMQuickItem> tmpquicklist = quicklist.FindAll(p => p.Type == QuickPanelType_t.Operation);
                return new ListCollectionView(tmpquicklist);
            }
        }

        private VMOperation _newoperation;
        public VMOperation Operation { get { return _newoperation; } set { _newoperation = value; NotifyPropertyChanged("Operation"); } }

        public int OperateTypeIndex { get { 
            return Operation == null ? 0 : (int)Operation.Operation.Type; }
            set { Operation = new VMOperation(new COperation((TaskType_t)value), Operation.Targets); }
        }

        public  VMQuickList()
        {
            ResourcesMgr.Instance().OnResourcesLoaded += new EventHandler(OnResourcesLoaded);  
            if(_newoperation == null)
            {
                Operation = new VMOperation(new COperation(TaskType_t.Schedule));
            }
        }

        private void OnResourcesLoaded(object sender, EventArgs e)
        {
            quicklist = SaveWork.Instance().ReadQuickList(ResourcesMgr.Instance().All);
            if(!FunctionConfigure._enableSchedule)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.Schedule);
            if(!FunctionConfigure._enableShortMessage)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.ShortMessage);
            if(!FunctionConfigure._enableLocation)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.Location);
            if(!FunctionConfigure._enableControler)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.Controler);
            if(!FunctionConfigure._enableLocationInDoor)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.LocationInDoor);
            if(!FunctionConfigure._enableJobTicket)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.JobTicket);
            if(!FunctionConfigure._enablePatrol)quicklist.RemoveAll(p=>p.Type == QuickPanelType_t.Operation && p.Operation.Operation.Type == TaskType_t.Patrol);


            foreach (VMQuickItem quick in quicklist)
            {
                quick.Closed += new EventHandler(OnClosed);
                //this.RegisterLoggerEvents(quick);
            }
            QuickList = new ListCollectionView(quicklist);
        }

        private void OnClosed(object sender, EventArgs e)
        {
            RemoveQuickExec(sender as VMQuickItem);
        }

        public ICommand AddQuick { get { return new Command(AddQuickExec); } }
        private void AddQuickExec(object parameter)
        {
            if (parameter is VMQuickItem)
            {
                VMQuickItem quick = parameter as VMQuickItem;
                if (quicklist.Find(p => p.Equal(quick)) == null) quicklist.Add(quick);       
                SaveWork.Instance().IsNeedSaveQuickPanel = true;
                QuickList = new ListCollectionView(quicklist);
            }
            else if (parameter is List<VMQuickItem>)foreach (VMQuickItem quick in parameter as List<VMQuickItem>) AddQuickExec(quick);
            else if (parameter is VMTarget)
            {
                VMQuickItem quick = new VMQuickItem(parameter as VMTarget);
                quick.Closed += new EventHandler(OnClosed);
                AddQuickExec(quick);                
            }
            else if (parameter is VMOperation)
            {
                VMQuickItem quick = new VMQuickItem(parameter as VMOperation);
                quick.Closed += new EventHandler(OnClosed);
                AddQuickExec(quick);
            }
            else return;
        }


        public ICommand RemoveQuick { get { return new Command(RemoveQuickExec); } }
        private void RemoveQuickExec(object parameter)
        {
            if (parameter is VMQuickItem)
            {
                VMQuickItem quick = parameter as VMQuickItem;
                quicklist.RemoveAll(p => p.Equal(quick));
                SaveWork.Instance().IsNeedSaveQuickPanel = true;
                QuickList = new ListCollectionView(quicklist);
            }
            else if (parameter is List<VMQuickItem>) foreach (VMQuickItem quick in parameter as List<VMQuickItem>) RemoveQuickExec(quick);
            else if (parameter is VMTarget) RemoveQuickExec(new VMQuickItem(parameter as VMTarget));
            else if (parameter is VMOperation) RemoveQuickExec(new VMQuickItem(parameter as VMOperation));
            else return;         
        }

        public ICommand ToolsFast { get { return new Command(ToolsFastExec); } }
        private void ToolsFastExec(object parameter)
        {
            if (parameter != null && parameter is string)
            {
                QuickPanelType_t key = ((string)parameter).ToEnum<QuickPanelType_t>();
                if (OnOperated != null) OnOperated(new OperatedEventArgs(OperateType_t.OpenNewFastWindow, key));
            };
        }


        private List<VMQuickItem> addlist;
        private List<VMQuickItem> removelist;

        public ICommand Add { get { return new Command(AddExec); } }
        private void AddExec(object parameter)
        {
            if (parameter == null)return;
            if (addlist == null) addlist = new List<VMQuickItem>();
            if (removelist == null) removelist = new List<VMQuickItem>();


            if (parameter is VMTarget)
            {
                VMQuickItem quick = new VMQuickItem(parameter as VMTarget);
                //this.RegisterLoggerEvents(quick);
                quick.Closed += new EventHandler(OnClosed);
                AddExec(quick);
            }
            else if (parameter is VMOperation)
            {
                VMQuickItem quick = new VMQuickItem(parameter as VMOperation);
                //this.RegisterLoggerEvents(quick);
                quick.Closed += new EventHandler(OnClosed);
                AddExec(quick);
            }
            else if (parameter is VMQuickItem)
            {
                VMQuickItem willadd = parameter as VMQuickItem;
                if (removelist.RemoveAll(p => p.Equal(willadd)) <= 0)
                {
                    if (addlist.Find(p => p.Equal(willadd)) == null && quicklist.Find(p => p.Equal(willadd)) == null) addlist.Add(willadd);
                }

                NotifyPropertyChanged("QuickTargetList");
                NotifyPropertyChanged("QuickOperationList");

                Operation = new VMOperation(new COperation((TaskType_t)OperateTypeIndex), Operation.Targets);
            }  
        }

        public ICommand Remove { get { return new Command(RemoveExec); } }
        private void RemoveExec(object parameter)
        {
            if (addlist == null) addlist = new List<VMQuickItem>();
            if (removelist == null) removelist = new List<VMQuickItem>();

            if (parameter == null || !(parameter is VMQuickItem)) return;

            VMQuickItem willremove = parameter as VMQuickItem;

            if (addlist.RemoveAll(p => p.Equal(willremove)) <= 0)
            {
                removelist.Add(willremove);
            }

            NotifyPropertyChanged("QuickTargetList");
            NotifyPropertyChanged("QuickOperationList");

            Operation = new VMOperation(new COperation((TaskType_t)OperateTypeIndex), Operation.Targets);
        }

        public ICommand Save { get { return new Command(SaveExec); } }
        private void SaveExec(object parameter)
        {
            if (removelist != null)
            {
                RemoveQuickExec(removelist);
                removelist.Clear();
            }
            if (addlist != null)
            {
                AddQuickExec(addlist);
                addlist.Clear();
            }



            if (parameter != null && parameter is NewQuick)(parameter as NewQuick).Close();
        }


        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion
    }
}
