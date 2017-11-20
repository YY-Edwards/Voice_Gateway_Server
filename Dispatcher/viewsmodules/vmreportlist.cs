using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Controls;

using Sigmar;
using Sigmar.Logger;
using Dispatcher.Service;
using System.Threading;
using Dispatcher.Modules;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace Dispatcher.ViewsModules
{
    public class VMReportList : INotifyPropertyChanged
    {
        private List<VMReport> _reportlist = new List<VMReport>();
        private ICollectionView _reportlistview;
        public ICollectionView ReportList { get { return _reportlistview; } private set { _reportlistview = value; NotifyPropertyChanged("ReportList"); NotifyPropertyChanged("ReportListMore"); } }
     

        private int _reportindex = -1;
        public int ReportIndex { get { return _reportindex; } set { _reportindex = value; NotifyPropertyChanged("ReportIndex"); } }


        public VMReportList()
        {
            _reportlist.Add(new VMReport("报表" + (ReportNumber++).ToString()));
            ReportList = new ListCollectionView(_reportlist);
            ReportIndex = _reportlist.Count - 1;
        }

        public ICommand Add { get { return new Command(AddExec); } }

        private static int ReportNumber = 1;
        private void AddExec()
        {
            _reportlist.Add(new VMReport("报表" + (ReportNumber++).ToString()) );
            ReportList = new ListCollectionView(_reportlist);
            ReportIndex = _reportlist.Count - 1;
            Log.Info("ADD a REPORT");
        }

        public ICommand Remove { get { return new Command(RemoveExec); } }
        private void RemoveExec(object parameter)
        {
            if (parameter == null || !(parameter is VMReport)) return;
            _reportlist.Remove(parameter as VMReport);
            ReportList = new ListCollectionView(_reportlist);
            ReportIndex = ReportIndex - 1;
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

    public class VMReport :  INotifyPropertyChanged
    {
        private string _name = string.Empty;
        public string Name { get { return _name; } set { _name = value; NotifyPropertyChanged("Name"); } }

        VMReportCondition _search;
        public VMReportCondition Search { get { return _search; } set { _search = value;  } }

        public VMReport(string name)
        {
            
            if(_search == null)
            {
                _search = new VMReportCondition(name);
            }
            Name = name;
        }

        private List<object> _reports = new List<object>();
        public ICollectionView Reports { get { return new ListCollectionView(_reports); } }


        private int _page = 0;
        public int Page { get { return _page; } private set { _page = value; NotifyPropertyChanged("Page"); } }
        public int RecordCount { get { return _search.Count; } }
        public int PageCount { get { return _search.Count % VMReportCondition.PageCount == 0 ? (_search.Count / VMReportCondition.PageCount) : (_search.Count / VMReportCondition.PageCount + 1); } }

        public ICommand Generate { get { return new Command(GenerateExec); } }
        private void GenerateExec(object parameter)
        {
            Name = _search.Name;

            if (parameter == null || !(parameter is DataGrid)) return;
            GenerateColumHeader((parameter as DataGrid));

            new Thread(new ThreadStart(delegate()
            {
                Page = 1;
                _lastpage = Page;
                _reports = _search.GetReport(_page, true);
                NotifyPropertyChanged("RecordCount");
                NotifyPropertyChanged("PageCount");
                NotifyPropertyChanged("Reports");

            })).Start();
        }

        public ICommand PageDown { get { return new Command(PageDownExec); } }
        private void PageDownExec()
        {
            PageChangedExec(Page++);
        }
        public ICommand PageUp { get { return new Command(PageUpExec); } }
        private void PageUpExec()
        {
            PageChangedExec(Page--);
        }

        public ICommand PageChanged { get { return new Command(PageChangedExec); } }
        private int _lastpage = 0;
        private void PageChangedExec(object parameter)
        {
            if (parameter == null || !(parameter is int) || _lastpage == (int)parameter || (int)parameter > PageCount) return;
            _lastpage = (int)parameter;
            new Thread(new ThreadStart(delegate()
            {
                _reports = _search.GetReport((int)parameter);
                NotifyPropertyChanged("Reports");
            })).Start();
        }

        private void GenerateColumHeader(DataGrid dt)
        {
            dt.Columns.Clear();
            switch (_search.Type)
            {
                case NotifyKey_t.Called:
                    dt.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "工作模式", Binding = new Binding("RecordTypeStr") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "通话类型", Binding = new Binding("CallTypeStr") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "源ID", Binding = new Binding("SourceID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "通话时长", Binding = new Binding("CallLength") });
                    break;
                case NotifyKey_t.ShortMessage:
                    dt.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "源ID", Binding = new Binding("SourceID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "内容", Binding = new Binding("Content") });
                    break;
                case NotifyKey_t.Location:
                    dt.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "经度", Binding = new Binding("Lat") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "纬度", Binding = new Binding("Log") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "高度", Binding = new Binding("Vel") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "速度", Binding = new Binding("Alt") });
                    break;
                case NotifyKey_t.LocationInDoor:
                    dt.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "区域", Binding = new Binding("AreaName") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "主修（Major）", Binding = new Binding("Major") });
                    dt.Columns.Add(new DataGridTextColumn() { Header = "辅修（Minor）", Binding = new Binding("Minor") });                                       
                    break;

            }
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


        public class VMReportCondition
        {
            public string Name { get; set; }
            private NotifyKey_t _type = NotifyKey_t.ShortMessage;
            public NotifyKey_t Type { get { return _type; } }
            public int TypeIndex
            {
                get { 
                    switch (_type)
                    {
                        case NotifyKey_t.ShortMessage:return 2;
                        case NotifyKey_t.Location:return 4;
                        case NotifyKey_t.LocationInDoor:return 5;
                    }
                    return -1;
                }
                set
                {
                    switch (value)
                    {
                        case 2:_type = NotifyKey_t.ShortMessage; break;
                        case 4: _type = NotifyKey_t.Location;break;
                        case 5: _type = NotifyKey_t.LocationInDoor;break;
                    }
                }
            }
                

            public DateTime? FirstTime { get; set; }
            public DateTime? LastTime { get; set; }

            public int TargetID { get; set; }

            private Mode_t _mode = Mode_t.All;
            private Call_t _call = Call_t.All;

            public int ModeIndex { get { return (int)_mode; } set { _mode = (Mode_t)value; } }
            public int CallIndex { get { return (int)_call; } set { _call = (Call_t)value; } }
            public VMReportCondition(string name)
            {
                Name = name;
                FirstTime = null;
                LastTime = DateTime.Now;
                _type = NotifyKey_t.ShortMessage;

                if (_logserver == null)
                {
                    _logserver = CLogServer.Instance();
                    _logserver.OnReceiveRequest += new CLogServer.ReceiveRequestHandele(ReceiveRequest);
                    _logserver.Timeout += new EventHandler(Timeout);
                }
            }

            public enum Mode_t
            {
                All,
                IPSC,
                CPC,
                LCP,
            }


            private CLogServer _logserver;

            private int _count = 0;
            public int Count { get { return _count; } }
            public static int PageCount = 50;

            public List<object> GetReport(int page, bool updatecount = false)
            {
                List<string[]> condition = GetCondition();
                
                if(updatecount)
                {
                    _count = GetReportCount(condition);
                }

                if (_count  <= 0|| (page - 1) * PageCount > _count) return new List<object>();

                return GetReport(condition, page);
            }

            public int GetReportCount(List<string[]> condition)
            {
                Dictionary<string, object> param = new Dictionary<string, object>();

                param.Add("operation", OperateType.count.ToString());
                if (null != condition && condition.Count > 0 && null != condition[0] && condition[0][0] == "empty") return 0;

                param.Add("critera", new Critera()
                {
                    offset = 0,
                    count = 1,
                    condition = condition
                });


                try
                {
                    switch (_type)
                    {
                        case NotifyKey_t.Called:
                            return (int)Request<int>(RequestOpcode.voicelog, OperateType.count, param);
                        case NotifyKey_t.ShortMessage:
                            return (int)Request<int>(RequestOpcode.smslog, OperateType.count, param);
                        case NotifyKey_t.Location:
                            return (int)Request<int>(RequestOpcode.gpslog, OperateType.count, param);
                        case NotifyKey_t.LocationInDoor:
                            return (int)Request<int>(RequestOpcode.locationLog, OperateType.count, param);
                    }
                }
                catch
                {

                }
                return 0;
            }

            private List<object> GetReport(List<string[]> condition, int page)
            {
                Dictionary<string, object> param = new Dictionary<string, object>();

                param.Add("operation", OperateType.list.ToString());
                if (null != condition && condition.Count > 0 && null != condition[0] && condition[0][0] == "empty") return null;

                param.Add("critera", new Critera()
                {
                    offset = (page - 1) * PageCount,
                    count = PageCount,
                    condition = condition
                });


                try
                {
                    switch (_type)
                    {
                        case NotifyKey_t.Called:
                            return (List<object>)Request<ReportCall>(RequestOpcode.voicelog, OperateType.list, param, null);
                        case NotifyKey_t.ShortMessage:
                            return (List<object>)Request<ReportSms>(RequestOpcode.smslog, OperateType.list, param, PraseSms);
                        case NotifyKey_t.Location:
                            return (List<object>)Request<ReportGps>(RequestOpcode.gpslog, OperateType.list, param, PraseGps);
                        case NotifyKey_t.LocationInDoor:
                            return (List<object>)Request<LocationInDoorLog>(RequestOpcode.locationLog, OperateType.list, param, PraseLocationInDoor);
                    }
                }
                catch
                {

                }
                return new List<object>();

            }

            private ReportSms PraseSms(string json)
            {
                try
                {
                    return JsonConvert.DeserializeObject<ReportSms>(json);
                }
                catch
                {
                    return null;
                }
            }


            private ReportGps PraseGps(string json)
            {
                try
                {
                    return JsonConvert.DeserializeObject<ReportGps>(json);
                }
                catch
                {
                    return null;
                }
            }

            private LocationInDoorLog PraseLocationInDoor(string json)
            {
                try
                {
                    return JsonConvert.DeserializeObject<LocationInDoorLog>(json);
                }
                catch
                {
                    return null;
                }
            }


            public enum Call_t
            {
                All,
                Broadcast,
                Group,
                Private,
            }

            private int _callint
            {
                get {
                    if (_call == Call_t.Group || _call == Call_t.All) return 79;
                    if (_call == Call_t.Private || _call == Call_t.All) return 80;
                    if (_call == Call_t.Broadcast) return 80;
                    else return 0;
                }
            }


            private List<string[]> NoRecord(List<string[]> lst)
            {
                lst.Add(new string[1] { "empty" });
                return lst;
            }

            private List<string[]> GetCondition()
            {

                switch(_type)
                {
                    case NotifyKey_t.Called:
                        return BuildCallCondition(FirstTime, LastTime, TargetID, ModeIndex, _callint);
                    case NotifyKey_t.ShortMessage:
                        return BuildSmsCondition(FirstTime, LastTime, TargetID);
                    case NotifyKey_t.Location:
                        return BuildGpsCondition(FirstTime, LastTime, TargetID);
                    case NotifyKey_t.LocationInDoor:
                        return BuildLocationInDoorCondition(FirstTime, LastTime, TargetID);
                } 
                return null;
            }

            private List<string[]> AddCondition(List<string[]> lst, string rel, string cmp, string trg, string val)
            {
                if (lst.Count <= 0) lst.Add(new string[3] { cmp, trg, val });
                else lst.Add(new string[4] { rel, cmp, trg, val });
                return lst;
            }
            private List<string[]> BuildCallCondition(DateTime? Start, DateTime? End, int Target, int RecType, int CallType)
            {

                List<string[]> res = new List<string[]>();
                if (Start == null) Start = default(DateTime);
                if (End == null) End = DateTime.Now;
                if (Start.Value <= End.Value)
                {
                    if (Target <= 0)
                    {
                        res = AddCondition(res, "", ">", "time", Start.Value.ToString("yyyy-MM-dd HH:mm:ss"));
                        res = AddCondition(res, "and", "<", "time", End.Value.ToString("yyyy-MM-dd HH:mm:ss"));

                        if (RecType >= 1 && RecType <= 3)
                        {
                            res = AddCondition(res, "and", "=", "record_type", RecType.ToString());
                        }

                        if (CallType == 79 || CallType == 80 || CallType == 83)
                        {
                            res = AddCondition(res, "and", "=", "call_type", CallType.ToString());
                        }
                    }
                    else
                    {
                        res = AddCondition(res, "", ">", "time", Start.Value.ToString("yyyy-MM-dd HH:mm:ss"));
                        res = AddCondition(res, "and", "<", "time", End.Value.ToString("yyyy-MM-dd HH:mm:ss"));

                        if (RecType >= 1 && RecType <= 3)
                        {
                            res = AddCondition(res, "and", "=", "record_type", RecType.ToString());
                        }

                        if (CallType == 79 || CallType == 80 || CallType == 83)
                        {
                            res = AddCondition(res, "and", "=", "call_type", CallType.ToString());
                        }

                        res = AddCondition(res, "and", "=", "src_radio", Target.ToString());

                        res = AddCondition(res, "or", ">", "createdf_at", Start.Value.ToString("yyyy-MM-dd HH:mm:ss"));
                        res = AddCondition(res, "and", "<", "createdf_at", End.Value.ToString("yyyy-MM-dd HH:mm:ss"));

                        if (RecType >= 1 && RecType <= 3)
                        {
                            res = AddCondition(res, "and", "=", "record_type", RecType.ToString());
                        }

                        if (CallType == 79 || CallType == 80 || CallType == 83)
                        {
                            res = AddCondition(res, "and", "=", "call_type", CallType.ToString());
                        }

                        res = AddCondition(res, "and", "=", "target_radio", Target.ToString());

                    }
                }
                else
                {
                    res = NoRecord(res);
                }


                return res;
                //string[][] constr = new string[res.Count][];
                //for (int i = 0; i < res.Count; i++)
                //{
                //    constr[i] = res[i];
                //}
                //return constr;
            }
            private List<string[]> BuildSmsCondition(DateTime? Start, DateTime? End, int Target)
            {
                if (Start == null) Start = default(DateTime);
                if (End == null) End = DateTime.Now;
                if (Start.Value >= End.Value)
                {
                    return new List<string[]>(){new string[1]{ "empty"}};                   
                }

                if (Target == -1)
                {
                    return new List<string[]>() { 
                            new string[3]{ ">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                            new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                        } ;
                   
                }

                 return new List<string[]>() 
                 { 
                    new string[3]{ ">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","=","source",Target.ToString()},
                    new string[4]{"or",">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","=","destination",Target.ToString()},
                };
            }
            private List<string[]> BuildGpsCondition(DateTime? Start, DateTime? End, int Target)
            {
                if (Start == null) Start = default(DateTime);
                if (End == null) End = DateTime.Now;
                if (Start.Value >= End.Value)
                {
                    return new List<string[]>() { new string[1] { "empty" } };        
                }

                if (Target == -1)
                {
                    return new List<string[]>() 
                {
                    new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                };
                }

                return new List<string[]>() 
            {
                new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","=","radio",Target.ToString()},
            };
            }

            private List<string[]> BuildLocationInDoorCondition(DateTime? Start, DateTime? End, int Target)
            {
                if (Start == null) Start = default(DateTime);
                if (End == null) End = DateTime.Now;
                if (Start.Value >= End.Value)
                {
                    return new List<string[]>() { new string[1] { "empty" } };
                }

                if (Target == -1)
                {
                    return new List<string[]>() 
                {
                    new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                };
                }

                return new List<string[]>() 
            {
                new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","=","source",Target.ToString()},
            };
            }

            private void ReceiveRequest(RequestOpcode call, object param)
        {
            //string parameter = param == null ? null : JsonConvert.SerializeObject(param);
            //switch (call)
            //{
            //    case RequestOpcode.status:
            //    case RequestOpcode.wlInfo:
            //        OnStatusUpdate(parameter);
            //        break;
            //    case RequestOpcode.callStatus:
            //    case RequestOpcode.wlCallStatus:
            //        OnCallResponse(parameter);
            //        break;
            //    case RequestOpcode.messageStatus:
            //        OnShortMessageResponse(parameter);
            //        break;
            //    case RequestOpcode.message:
            //        OnShortMessageRequest(parameter);
            //        break;
            //    case RequestOpcode.sendArs:
            //        OnArsRequest(parameter);
            //        break;
            //    case RequestOpcode.controlStatus:
            //        OnControlResponse(parameter);
            //        break;
            //    case RequestOpcode.queryGpsStatus:
            //        OnLocationQueryResponse(parameter);
            //        break;
            //    case RequestOpcode.sendGps:
            //        OnLocationReport(parameter);
            //        break;
            //    case RequestOpcode.locationIndoor:
            //        OnLocationInDoorReport(parameter);
            //        break;
            //    default:
            //        CustomRequest(call,  parameter);
            //        break;

            //}
        }

        private void Timeout(object sender, EventArgs e)
        {
            //WARNING("Timeout");
        }


        public delegate T ParseCallBackHandler<T>(string json);
        public object Request<T>(RequestOpcode opcode, OperateType type, object parameter, ParseCallBackHandler<T> parse = null )
        {
            if (!CLogServer.Instance().IsInitialized) return null;
            try
            {
                string[] reply = CLogServer.Instance().Request(opcode, parameter);

                if (reply != null && reply.Length >= 2)
                {
                    switch (type)
                    {
                        case OperateType.list:
                            if (reply[0] == "success")
                            {
                                return ParseList(reply[1], parse);
                            }
                            break;
                        case OperateType.count:
                            if (reply[0] == "success")
                            {
                                return ParseCount(reply[1]);                                
                            }
                            break;
                        default:                     
                            break;
                    }
                }
            }
            catch
            {

            }

            return null;
        }

        private List<object> ParseList<T>(string json, ParseCallBackHandler<T> parse)
        {
            try
            {
                List<object> querylist = new List<object>();

                if (json == null || json == string.Empty) return null;

                Dictionary<string, List<JObject>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<JObject>>>(json);
                if (Dic.ContainsKey("records"))
                {
                    foreach (JObject obj in Dic["records"])
                    {
                        if (obj == null) continue;
                        var res = parse(JsonConvert.SerializeObject(obj));
                        if (res != null)
                        {
                            querylist.Add(res);
                        }

                    }
                }
                return querylist;
            }
            catch
            {
                return null;
            }
        }

        private int ParseCount(string json)
        {
            try
            {
                if (json == null || json == string.Empty) return 0;
                Dictionary<string, int> Dic = JsonConvert.DeserializeObject<Dictionary<string, int>>(json);
                return Dic["count"];
            }
            catch (Exception e)
            {
                return 0;
            }
        }

        public virtual void CustomReply(RequestOpcode opcode,bool success, string reply)
        {

        }

        public virtual void CustomRequest(RequestOpcode call, string param)
        {

        }

        }
    }
}
