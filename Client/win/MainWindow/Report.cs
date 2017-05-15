using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using Newtonsoft.Json;
using System.Threading;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Data;

namespace TrboX
{

    public class ReportItem
    {
        public string Type { set; get; }
        public string Date { set; get; }
        public int TargetID { set; get; }
        public string Content { set; get; }
    }


    [Serializable]
    public class ReportSms
    {
        public string Time{ set; get; }
        public int SourceID { set; get; }
        public int TargetID { set; get; }
        public string Content { set; get; }               
    }

    [Serializable]
    public class ReportGps
    {
        public string Time { set; get; }
        public int TargetID { set; get; }
        public string Lat { set; get; }
        public string Log { set; get; }
        public string Vel { set; get; }
        public string Alt { set; get; }
    }

    [Serializable]
    public class ReportCall
    {
        public string Time { set; get; }
        public int SourceID { set; get; }
        public int TargetID { set; get; }
        public int RecordType { set; get; }
        public string RecordTypeStr { get {
            switch (RecordType)
            { case 0:return "IPSC";
            case 1: return "CPC";
                case 2:
                return "LCP";
                default: return "未知";
            }
        } }
        public int CallType { set; get; }
        public string CallTypeStr
        {
            get
            {
                switch (CallType)
                {
                    case 79: return "组呼";
                    case 80: return "个呼";
                    case 83:
                        return "全呼";
                    default: return "未知";
                }
            }
        }
        public int DataSize { set; get; }
        public string CallLength { get { return (((double)DataSize) / 350).ToString("0.0") + "s"; } } 
    }

    [Serializable]
    public class ReportInfo
    {
        public string Name { set; get; }
        public int Type { set; get; }
        public DateTime ? Start{ set; get; }
        public DateTime ? End{ set; get; }
        public int CallType{ set; get; }
        public int RecordType{ set; get; }
        public string Traget{ set; get; }
        public int CurrentPage { set; get; }

        public int TotalPage { get{
            return (TotalRecord % Report.PageCount == 0) ? (TotalRecord / Report.PageCount) : (TotalRecord / Report.PageCount + 1);;
        } }
        public int TotalRecord { set; get; }

        public bool ? IsCheck { set; get; }

        public string TotalpageStr { get { return "页 / 共 " + TotalPage.ToString() + " 页（共 " + TotalRecord.ToString() + " 条记录）"; } }
        public string TypeStr
        {
            get
            {
                switch (Type)
                {
                    case 0: return "通话";
                    case 1: return "短消息";
                    case 2: return "位置信息";
                    default: return "";
                }
            }
        }
    }

    [Serializable]
    public class SerializeObject
    {
        public ReportInfo info;
        public object dat;
    }

    public class FilterCon
    {
        public string _type;
        public int _target;
        public DateTime? _strart;
        public DateTime? _end;

        public int _record_type;
        public int _call_type; 
    }

    public class Report
    { 
        private Main m_Main;
        private static int ReportIndex = 1;
        public static int PageCount = 50;

        private bool IsInit = false;
     
        public Report(Main main)
        {
            if (main != null) m_Main = main;

            m_Main.tab_Report.Loaded += delegate { if (!IsInit) { ResotreReport(); IsInit = true; } };
            m_Main.btn_NewReportView.Click += delegate { NewReportView(); };
            m_Main.tab_Report.SelectionChanged += delegate { FillReportInfo(); };
            m_Main.btn_UpdateReport.Click += delegate { UpdateReportView(); };
            m_Main.chk_OnOffCon.Click += delegate { ConditionPanel(); };

            m_Main.tab_main.SelectionChanged += delegate {ExportReportAble(); };
            m_Main.tab_Report.SelectionChanged += delegate { ExportReportAble(); };

            m_Main.menu_File_Export.Click += delegate { ExportReport(); };
            m_Main.btn_Export.Click += delegate { ExportReport(); };
       }

        private void ConditionPanel()
        {
            ((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).IsCheck = m_Main.chk_OnOffCon.IsChecked;
        }

        private void FillReportInfo()
        {
            try
            {
                if (m_Main.tab_Report.SelectedItem == null) return;

                ReportInfo info = ((TabItem)m_Main.tab_Report.SelectedItem).Tag as ReportInfo;

                m_Main.dam_Start.Value = info.Start;
                m_Main.dam_End.Value = info.End;
                m_Main.chk_OnOffCon.IsChecked = info.IsCheck;
            }
            catch
            { }
        }

        private FilterCon ResetFilter()
        {
            m_Main.dam_Start.Value = DateTime.Now.AddMonths(-1);
            m_Main.dam_End.Value = DateTime.Now;
            m_Main.txt_ReportName.Text = "报表-" + ReportIndex.ToString();

            m_Main.cmb_ReportType.SelectedIndex = 0;
            m_Main.txt_ReportTraget.Text = string.Empty;


            m_Main.cmb_RecordType.SelectedIndex = 0;
            m_Main.cmb_CallType.SelectedIndex = 0;

            return GetFilter();
        }


        private FilterCon GetFilter()
        {
            try
            {
                FilterCon Con  = new FilterCon();;
                Con._type = ((ComboBoxItem)m_Main.cmb_ReportType.SelectedItem).Content as string;
                Con._target = -1;
                try
                {
                    Con._target = int.Parse(m_Main.txt_ReportTraget.Text);
                }
                catch
                {
                }

                Con._strart = m_Main.dam_Start.Value;
                Con._end = m_Main.dam_End.Value;
                Con._record_type = m_Main.cmb_RecordType.SelectedIndex;
                Con._call_type = -1;

                try
                {
                    Con._call_type = int.Parse(((ComboBoxItem)m_Main.cmb_CallType.SelectedItem).Tag as string);
                }
                catch
                {
                }

                return Con;
            }
            catch
            {
                return null;
            }
        }

        private void NewReportView()
        {
            try
            {
                m_Main.dck_ReportList.Visibility = Visibility.Visible;

                FilterCon con = ResetFilter();

                int recordcount = GetReportCount(con);

                TabItem reportItem = new TabItem();

                reportItem.Tag = new ReportInfo()
                {
                    Name = m_Main.txt_ReportName.Text,
                    Type = m_Main.cmb_ReportType.SelectedIndex,
                    Start = m_Main.dam_Start.Value,
                    End = m_Main.dam_End.Value,
                    Traget = m_Main.txt_ReportTraget.Text,
                    RecordType = con._record_type,
                    CallType = m_Main.cmb_CallType.SelectedIndex,
                    TotalRecord = recordcount,
                    CurrentPage = 1,
                    IsCheck = true
                };

                
                reportItem.Header = m_Main.txt_ReportName.Text;
                reportItem.Style = m_Main.Resources["TabItemReportStyle"] as Style;

                Grid ConnectGrid = new Grid();
                ConnectGrid.Children.Insert(0, FillReportPage(con, null, 1, recordcount));
                reportItem.Content = ConnectGrid;
                m_Main.tab_Report.Items.Insert(m_Main.tab_Report.Items.Count, reportItem);
                m_Main.tab_Report.SelectedIndex = m_Main.tab_Report.Items.Count - 1;

                ReportIndex++;

                m_Main.g_IsNeedSaveWorkSpace = true;
            }
            catch
            {
            }
        }

        private void UpdateReportView()
        {
            try
            {
                FilterCon con = GetFilter();
                int recordcount = GetReportCount(con);

                ((TabItem)m_Main.tab_Report.SelectedItem).Tag = new ReportInfo()
                {
                    Name = m_Main.txt_ReportName.Text,
                    Type = m_Main.cmb_ReportType.SelectedIndex,
                    Start = m_Main.dam_Start.Value,
                    End = m_Main.dam_End.Value,
                    Traget = m_Main.txt_ReportTraget.Text,
                    RecordType = con._record_type,
                    CallType = m_Main.cmb_CallType.SelectedIndex,
                    TotalRecord = recordcount,
                    CurrentPage = 1
                };

                ((TabItem)m_Main.tab_Report.SelectedItem).Header = m_Main.txt_ReportName.Text;
                Grid gd = ((TabItem)m_Main.tab_Report.SelectedItem).Content as Grid;

                if (gd.Children.Count <= 0 || (string)(gd.Children[0] as DataGrid).Tag != con._type)
                {
                    gd.Children.Clear();
                    gd.Children.Insert(0, FillReportPage(con, null, 1, recordcount));
                }
                else
                {
                    FillReportPage(con, gd.Children[0] as DataGrid, 1, recordcount);
                }

                m_Main.g_IsNeedSaveWorkSpace = true;
            }
            catch
            {

            }
        }

        public void PageChange()
        {
            try
            {
                int pgnum = int.Parse(m_Main.txt_CurrentPage.Text);

                ReportInfo info = ((TabItem)m_Main.tab_Report.SelectedItem).Tag as ReportInfo;
                if (pgnum <= 0 || info.CurrentPage == pgnum || info.TotalPage < pgnum) return;

                try
                {
                    Grid gd = ((TabItem)m_Main.tab_Report.SelectedItem).Content as Grid;

                    if (gd.Children.Count <= 0)
                    {
                        gd.Children.Insert(0, FillReportPage(GetFilter(), null, pgnum, info.TotalRecord));
                    }
                    else
                    {
                        FillReportPage(GetFilter(), gd.Children[0] as DataGrid, pgnum, info.TotalRecord);
                    }

                    ((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).CurrentPage = pgnum;
                }
                catch
                {
                }

                m_Main.g_IsNeedSaveWorkSpace = true;
            }
            catch
            {
                return;
            }           
        }
      

        private int GetReportCount(FilterCon con)
        {
            if (con._type == "通话")
            {
                return CallReportCount(con._strart, con._end, con._target, con._record_type, con._call_type);
            }
            else if (con._type == "短消息")
            {
                return SmsReportCount(con._strart, con._end, con._target);
            }
            else if (con._type == "位置信息")
            {
                return GpsReportCount(con._strart, con._end, con._target);
            }
            return 0;
        }


        private List<object> GetReoprt(FilterCon con, int page, int total)
        {
            if (con._type == "通话")
            {
                return GetCallReport(con._strart, con._end, con._target, con._record_type, con._call_type, page, total);
            }
            else if (con._type == "短消息")
            {
                return GetMessageReport(con._strart, con._end, con._target, page, total);
            }
            else if (con._type == "位置信息")
            {
                return GetPositionReport(con._strart, con._end, con._target, page, total);
            }

            return null;
        }

        private DataGrid FillReportPage(FilterCon con, DataGrid reportdata, int page, int total)
        {
            if (reportdata == null || reportdata != null && (string)reportdata.Tag != con._type)
            {
                reportdata = CreateNewDataGrid(con._type);
            }

            new Thread(new ThreadStart(delegate()
            {
                List<object> reportitems = GetReoprt(con, page, total);
                m_Main.Dispatcher.Invoke(new Action(() =>
                {
                    reportdata.ItemsSource = reportitems;
                }));
            })).Start();

            return reportdata;
        }


        private DataGrid CreateNewDataGrid(string type)
        {

            DataGrid reportdata = new DataGrid() { Style = m_Main.Resources["DataGridReportStyle"] as Style, AutoGenerateColumns = false };

            if (type == "通话")
            {
                reportdata.Tag = "通话";
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "工作模式", Binding = new Binding("RecordTypeStr") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "通话类型", Binding = new Binding("CallTypeStr") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "源ID", Binding = new Binding("SourceID") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "通话时长", Binding = new Binding("CallLength") });
            }
            else if (type == "短消息")
            {
                reportdata.Tag = "短消息";
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "源ID", Binding = new Binding("SourceID") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "内容", Binding = new Binding("Content") });
            }
            else if (type == "位置信息")
            {
                reportdata.Tag = "位置信息";
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Time") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "对讲机", Binding = new Binding("TargetID") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "经度", Binding = new Binding("Lat") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "纬度", Binding = new Binding("Log") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "高度", Binding = new Binding("Vel") });
                reportdata.Columns.Add(new DataGridTextColumn() { Header = "速度", Binding = new Binding("Alt") });
            }

            return reportdata;
        }

        public void SaveReport()
        {
            BinaryFormatter m_BinFormat = new BinaryFormatter();
            Stream ReportListFile = new FileStream(App.ReportTempPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
            ReportListFile.SetLength(0);

            List<SerializeObject> obj = new List<SerializeObject>();

            foreach (TabItem item in m_Main.tab_Report.Items)
            {
                SerializeObject so = new SerializeObject()
                {
                    info = item.Tag as ReportInfo
                };

                try
                {
                    DataGrid gd = ((Grid)item.Content).Children[0] as DataGrid;
                    so.dat = gd.ItemsSource;
                }
                catch
                {

                }
                obj.Add(so);
            }

            m_BinFormat.Serialize(ReportListFile, obj);
        }

        public void ResotreReport()
        {
            m_Main.dck_ReportList.Visibility = Visibility.Visible;
            BinaryFormatter m_BinFormat = new BinaryFormatter();
            Stream ReportListFile = new FileStream(App.ReportTempPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
            ReportListFile.Position = 0;


            List<SerializeObject> obj = new List<SerializeObject>();
            try
            {
                obj = (List<SerializeObject>)m_BinFormat.Deserialize(ReportListFile);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Read Fast Operation List Error" + e.Message);
            }

            foreach (SerializeObject so in obj)
            {
                DataGrid dat = CreateNewDataGrid(so.info.TypeStr);
                dat.ItemsSource = so.dat as List<object>;

                Grid grid = new Grid();
                grid.Children.Insert(0, dat);


                TabItem tb_item = new TabItem()
                {
                    Header = so.info.Name,
                    Style = m_Main.Resources["TabItemReportStyle"] as Style,
                    Tag = so.info,
                    Content = grid
                };

                m_Main.tab_Report.Items.Add(tb_item);
            }

            m_Main.tab_Report.SelectedIndex = m_Main.tab_Report.Items.Count - 1;
        }

        public void ExportReportAble()
        {
            if (m_Main.tab_main.SelectedIndex == 3 && m_Main.tab_Report.Items.Count > 0) m_Main.btn_Export.IsEnabled = true;
            else m_Main.btn_Export.IsEnabled = false;
           
        }

        private void ExportReport()
        {
            try{
                ReportInfo info = ((TabItem)m_Main.tab_Report.SelectedItem).Tag as ReportInfo;

                System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
                sfd.FileName = info.Name + ".csv";
                sfd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                sfd.Filter = "CSV文档|*.csv";
                if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    string localFilePath =  sfd.FileName;
                    SaveCSV(localFilePath);
                }
                else
                {
                }  
            }
            catch{

            }
        }
        private void SaveCSV(string fullPath)
        {
            FilterCon con = GetFilter();
            DataGrid dat = CreateNewDataGrid(con._type);
            string[] columns = new string[dat.Columns.Count];
            for (int i = 0; i < dat.Columns.Count; i++)
            {
                columns[i] = dat.Columns[i].Header as string;
            }

            ReportInfo info = ((TabItem)m_Main.tab_Report.SelectedItem).Tag as ReportInfo;
            int recordcount = info.TotalRecord;
            int pagecount = info.TotalPage;

            m_Main.dck_ProcessStatus.Visibility = Visibility.Visible;
            m_Main.lab_PrcessName.Content = "正在导出：";
           
            new Thread(new ThreadStart(delegate()
            {
                try
                {
                    FileInfo fi = new FileInfo(fullPath);
                    if (!fi.Directory.Exists)
                    {
                        fi.Directory.Create();
                    }
                    FileStream fs = new FileStream(fullPath, System.IO.FileMode.Create, System.IO.FileAccess.Write);
                    //StreamWriter sw = new StreamWriter(fs, System.Text.Encoding.Default);
                    StreamWriter sw = new StreamWriter(fs, System.Text.Encoding.UTF8);
                    string data = "";

                    for(int i = 0; i < columns.Length; i++)
                    {
                        data += columns;
                        if (i < columns.Length - 1)
                        {
                            data += ",";
                        }
                    }
                    sw.WriteLine(data);


                    for(int i = 0; i< pagecount; i++)
                    {
                         List<object> reportitems = GetReoprt(con, i+1, recordcount);

                         foreach (var item in reportitems)
                         {
                             Dictionary<string, string> row = JsonConvert.DeserializeObject<Dictionary<string, string>>(JsonConvert.SerializeObject(item));
                             data = "";
                             int j = 0;
                             foreach (var val in row)
                             {
                                 string str = val.Value;
                                 str = str.Replace("\"", "\"\"");//替换英文冒号 英文冒号需要换成两个冒号
                                 if (str.Contains(',') || str.Contains('"')
                                     || str.Contains('\r') || str.Contains('\n')) //含逗号 冒号 换行符的需要放到引号中
                                 {
                                     str = string.Format("\"{0}\"", str);
                                 }

                                 data += str;
                                 if (j < columns.Length - 1)
                                 {
                                     data += ",";
                                 }

                                 j++;

                                 //Thread.Sleep(150);
                             }

                             sw.WriteLine(data);

                         }

                         m_Main.Dispatcher.Invoke(new Action(() =>
                         {
                             double val = ((double)(i+1)) / pagecount;
                             val = val * 100;

                             Console.Write("--------------Process : " + val.ToString("0.0") + "%--------------------\r\n");

                             m_Main.pg_StstauPg.Value = val;
                         }));
                    }


                    sw.Close();
                    fs.Close();


                    m_Main.Dispatcher.Invoke(new Action(() =>
                    {
                        Console.Write("--------------Process : 100%--------------------\r\n");
                        m_Main.pg_StstauPg.Value = 100;
                        m_Main.EventList.AddEvent("导出报表：" + fullPath.Substring(fullPath.LastIndexOf("\\") + 1) + "成功.");
                    }));
                }
                catch
                {
                    m_Main.Dispatcher.Invoke(new Action(() =>
                    {
                         m_Main.EventList.AddEvent("导出报表：" + fullPath.Substring(fullPath.LastIndexOf("\\") + 1) + "失败.");
                    }));
                   
                }
                finally
                {
                    m_Main.Dispatcher.Invoke(new Action(() =>
                    {
                        m_Main.dck_ProcessStatus.Visibility = Visibility.Collapsed;
                    }));                   
                }
               
            })).Start();
        }



        public object ParseCount(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, int> Dic = JsonConvert.DeserializeObject<Dictionary<string, int>>(JsonConvert.SerializeObject(rep.contents));
                return Dic["count"];
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }


        private string[][] BuildSmsCondition(DateTime ? Start, DateTime ? End, int Target)
        {
            if(Start.Value >= End.Value)
            {
                return new string[1][]
                {
                    new string[1]{ "empty"},                  
                };
            }
            
            if(Target == -1)
            {
                return new string[2][]
                {
                    new string[3]{ ">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                };
            }

            return new string[6][]
            {
                new string[3]{ ">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","=","source",Target.ToString()},
                new string[4]{"or",">","createdf_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","<","createdf_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","=","destination",Target.ToString()},
            };
        }

        public int SmsReportCount(DateTime ? Start, DateTime ? End, int Target)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.count.ToString());

            string[][] conobj = BuildSmsCondition(Start, End, Target);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return 0;

            param.Add("critera", new Critera()
            {
                offset = 0,
                count = -1,
                condition = conobj
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.smslog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return 0;
            }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

       

        public List<object> GetMessageReport(DateTime? Start, DateTime? End, int Target, int page, int count)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());

            string[][] conobj = BuildSmsCondition(Start, End, Target);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return null;

            int tmp_offset = (page - 1) * PageCount;
            int tmp_count = (count - tmp_offset > PageCount) ? PageCount : (count - tmp_offset); 

            param.Add("critera", new Critera()
            {
                offset = tmp_offset,
                count = tmp_count,
                condition = conobj    
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.smslog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return null;
            }


            try
            {
                return LogServer.Call(str, ParseSms) as List<object>;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        private List<object> ParseSms(object obj)
        {
            try
            {
                if (obj == null) return null;

                List<object> reports = new List<object>();

                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<object>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<object>>>(JsonConvert.SerializeObject(rep.contents));

                foreach (object item in Dic["records"])
                {
                    try
                    {
                        Dictionary<string, string> sms = JsonConvert.DeserializeObject<Dictionary<string, string>>(JsonConvert.SerializeObject(item));

                        reports.Add(new ReportSms()
                        {
                            Time = sms["createdf_at"],
                            SourceID = int.Parse(sms["source"]),
                            TargetID = int.Parse(sms["destination"]),
                            Content = sms["message"]
                        });
                    }
                    catch
                    { }

                }

                return reports;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }


        private string[][] BuildGpsCondition(DateTime? Start, DateTime? End, int Target)
        {
            if (Start.Value >= End.Value)
            {
                return new string[1][]
                {
                    new string[1]{ "empty"},                  
                };
            }

            if (Target == -1)
            {
                return new string[2][]
                {
                    new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                    new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                };
            }

            return new string[3][]
            {
                new string[3]{ ">","created_at",Start.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","<","created_at",End.Value.ToString("yyyy-MM-dd HH:mm:ss")},
                new string[4]{"and","=","radio",Target.ToString()},
            };
        }

        public int GpsReportCount(DateTime? Start, DateTime? End, int Target)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.count.ToString());

            string[][] conobj = BuildGpsCondition(Start, End, Target);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return 0;

            param.Add("critera", new Critera()
            {
                offset = 0,
                count = -1,
                condition = conobj
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.gpslog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return 0;
            }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

        public List<object> GetPositionReport(DateTime? Start, DateTime? End, int Target, int page, int count)
        {

            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());

            string[][] conobj = BuildGpsCondition(Start, End, Target);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return null;

            int tmp_offset = (page - 1) * PageCount;
            int tmp_count = (count - tmp_offset > PageCount) ? PageCount :(count - tmp_offset); 

            param.Add("critera", new Critera()
            {
                offset = tmp_offset,
                count = tmp_count,
                condition = conobj    
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.gpslog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return null;
            }


            try
            {
                return LogServer.Call(str, ParseGps) as List<object>;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        private List<object> ParseGps(object obj)
        {
            try
            {
                if (obj == null) return null;

                List<object> reports = new List<object>();

                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<object>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<object>>>(JsonConvert.SerializeObject(rep.contents));

                foreach (object item in Dic["records"])
                {
                    try
                    {
                        Dictionary<string, string> gps = JsonConvert.DeserializeObject<Dictionary<string, string>>(JsonConvert.SerializeObject(item));

                        reports.Add(new ReportGps()
                        {
                            Time = gps["created_at"],
                            TargetID = int.Parse(gps["radio"]),
                            Lat = gps["latitude"],
                            Log = gps["logitude"],
                            Vel = gps["velocity"],
                            Alt = gps["altitude"],
                        });
                    }
                    catch { }

                }

                return reports;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }


        private List<string[]> AddCondition(List<string[]> lst, string rel, string cmp, string trg, string val)
        {           
            if (lst.Count <= 0) lst.Add(new string[3]{ cmp,trg,val});
            else lst.Add(new string[4] {rel, cmp, trg, val });           
            return lst;
        }

        private List<string[]> NoRecord(List<string[]> lst)
        {
            lst.Add(new string[1] { "empty" });
            return lst;
        }


        private string[][] BuildCallCondition(DateTime? Start, DateTime? End, int Target, int RecType, int CallType)
        {

            List<string[]> res = new List<string[]>();
            if (Start.Value <= End.Value)
            {
                if (Target <= 0)
                {
                    res = AddCondition(res, "",">","time",Start.Value.ToString("yyyy-MM-dd HH:mm:ss"));
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
            else{
                res = NoRecord(res);
            }

            string[][] constr = new string[res.Count][];
            for(int i = 0; i< res.Count; i++)
            {
                constr[i] = res[i];
            }
            return constr;
        }

        public int CallReportCount(DateTime? Start, DateTime? End, int Target, int RecType, int CallType)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.count.ToString());

            string[][] conobj = BuildCallCondition(Start, End, Target, RecType, CallType);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return 0;

            param.Add("critera", new Critera()
            {
                offset = 0,
                count = -1,
                condition = conobj
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.voicelog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return 0;
            }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }


        public List<object> GetCallReport(DateTime? Start, DateTime? End, int Target, int RecType, int CallType, int page, int count)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());

            string[][] conobj = BuildCallCondition(Start, End, Target, RecType, CallType);
            if (null != conobj && null != conobj[0] && conobj[0][0] == "empty") return null;

            int tmp_offset = (page - 1) * PageCount;
            int tmp_count = (count - tmp_offset > PageCount) ? PageCount : (count - tmp_offset);

            param.Add("critera", new Critera()
            {
                offset = tmp_offset,
                count = tmp_count,
                condition = conobj
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.voicelog.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return null;
            }


            try
            {
                return LogServer.Call(str, ParseCall) as List<object>;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        private List<object> ParseCall(object obj)
        {
            try
            {
                if (obj == null) return null;

                List<object> reports = new List<object>();

                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<object>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<object>>>(JsonConvert.SerializeObject(rep.contents));

                foreach (object item in Dic["records"])
                {
                    try
                    {
                        Dictionary<string, string> call = JsonConvert.DeserializeObject<Dictionary<string, string>>(JsonConvert.SerializeObject(item));

                        reports.Add(new ReportCall()
                        {
                            Time = call["time"],
                            SourceID = int.Parse(call["src_radio"]),
                            TargetID = int.Parse(call["target_radio"]),
                            RecordType = int.Parse(call["record_type"]),
                            CallType = int.Parse(call["call_type"]),
                            DataSize = int.Parse(call["length"]),
                        });
                    }
                    catch
                    {

                    }

                }

                return reports;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

    }
}
