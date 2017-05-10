using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Data;
using Newtonsoft.Json;
using System.Threading;

namespace TrboX
{

    public class ReportItem
    {
        public string Type { set; get; }
        public string Date { set; get; }
        public int TargetID { set; get; }
        public string Content { set; get; }
    }


    public class ReportSms
    {
        public string Time{ set; get; }
        public int SourceID { set; get; }
        public int TargetID { set; get; }
        public string Content { set; get; }               
    }

    public class ReportGps
    {
        public string Time { set; get; }
        public int TargetID { set; get; }
        public string Lat { set; get; }
        public string Log { set; get; }
        public string Vel { set; get; }
        public string Alt { set; get; }
    }

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


        public string TotalpageStr { get { return "页 / 共 " + TotalPage.ToString() + " 页（共 " + TotalRecord.ToString() + " 条记录）"; } }
    }
    public class Report
    { 
        private Main m_Main;
        private static int ReportIndex = 1;

        public static int PageCount = 2;
        public Report(Main main)
        {
            if (main != null) m_Main = main;

            m_Main.btn_NewReportView.Click += delegate { NewReportView(); };
            m_Main.tab_Report.SelectionChanged += delegate { FillReportInfo(); };
            m_Main.btn_UpdateReport.Click += delegate { UpdateReportView(); };
       }
        public void PageChange()
        {
            m_Main.Dispatcher.Invoke(new Action(() =>
            {
                try
                {
                    int pgnum = int.Parse(m_Main.txt_CurrentPage.Text);


                    if (((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).CurrentPage == pgnum) return;

                    if (pgnum <= 0) return;
                    if (((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).TotalPage < pgnum) return;

                    try
                    {

                        Grid gd = ((TabItem)m_Main.tab_Report.SelectedItem).Content as Grid;

                        if (gd.Children.Count <= 0)
                        {
                            gd.Children.Insert(0, FillReportPage(null, pgnum, ((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).TotalRecord));
                        }
                        else
                        {
                            FillReportPage(gd.Children[0] as DataGrid, pgnum, ((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).TotalRecord);
                        }

                        ((ReportInfo)((TabItem)m_Main.tab_Report.SelectedItem).Tag).CurrentPage = pgnum;
                    }
                    catch
                    {

                    }
                    
                }
                catch
                {
                    return;
                }
            }));  
        }

        private void NewReportView()
        {
            try
            {
                m_Main.dck_ReportList.Visibility = Visibility.Visible;

                m_Main.dam_Start.Value = DateTime.Now.AddMonths(-1);
                m_Main.dam_End.Value = DateTime.Now;
                m_Main.txt_ReportName.Text = "报表-" + ReportIndex.ToString();

                m_Main.cmb_ReportType.SelectedIndex = 1;
                m_Main.txt_ReportTraget.Text = string.Empty;

                TabItem reportItem = new TabItem();
                reportItem.Header = m_Main.txt_ReportName.Text + ".log";
                reportItem.Style = m_Main.Resources["TabItemReportStyle"] as Style;

                int reporttarget = -1;

                string type = ((ComboBoxItem)m_Main.cmb_ReportType.SelectedItem).Content as string;
                DateTime? strart = m_Main.dam_Start.Value;
                DateTime? end = m_Main.dam_End.Value;

                int record_type = 79;
                int call_type = 40;

                try
                {
                    reporttarget = int.Parse(m_Main.txt_ReportTraget.Text);
                }
                catch
                {
                }

                int count = 0;

                if (type == "通话")
                {

                }
                else if (type == "短消息")
                {
                    count = SmsReportCount(strart, end, reporttarget);
                }
                else if (type == "位置信息")
                {
                    count = GpsReportCount(strart, end, reporttarget);
                }       


                reportItem.Tag = new ReportInfo()
                {
                    Name = m_Main.txt_ReportName.Text,
                    Type = m_Main.cmb_ReportType.SelectedIndex,
                    Start = m_Main.dam_Start.Value,
                    End = m_Main.dam_End.Value,
                    Traget = m_Main.txt_ReportTraget.Text,
                    RecordType = record_type,
                    CallType = call_type,  
                    TotalRecord = count,
                    CurrentPage = 1
                };

                Grid ConnectGrid = new Grid();
                ConnectGrid.Children.Insert(0, FillReportPage(null, 1, count));
                reportItem.Content = ConnectGrid;
                m_Main.tab_Report.Items.Insert(m_Main.tab_Report.Items.Count, reportItem);
                m_Main.tab_Report.SelectedIndex = m_Main.tab_Report.Items.Count - 1;

                ReportIndex++;
            }
            catch
            {
                
            }
        }

        private void FillReportInfo()
        {
            try
            {
                if (m_Main.tab_Report.SelectedItem == null) return;

                ReportInfo info = ((TabItem)m_Main.tab_Report.SelectedItem).Tag as ReportInfo;

                m_Main.dam_Start.Value = info.Start;
                m_Main.dam_End.Value = info.End;
            }
            catch
            { }
        }
       
        private DataGrid FillReportPage( DataGrid reportdata, int page, int total)
        {         
            int reporttarget = -1;

            string type = ((ComboBoxItem)m_Main.cmb_ReportType.SelectedItem).Content as string;

            DateTime? strart = m_Main.dam_Start.Value;
            DateTime? end = m_Main.dam_End.Value;

            int record_type = 79;
            int call_type = 40;

            try
            {
                reporttarget = int.Parse(m_Main.txt_ReportTraget.Text);
            }
            catch
            {
            }

            if (reportdata == null || reportdata != null && (string)reportdata.Tag != type)
            {
                reportdata = new DataGrid() { Style = m_Main.Resources["DataGridReportStyle"] as Style, AutoGenerateColumns = false };

                if (type == "通话")
                {
                    reportdata.Tag = "通话";
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "时间", Binding = new Binding("Date") });
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "工作模式", Binding = new Binding("Type") });
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "通话类型", Binding = new Binding("Type") });
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "源ID", Binding = new Binding("Date") });
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "目标ID", Binding = new Binding("TargetID") });
                    reportdata.Columns.Add(new DataGridTextColumn() { Header = "通话时长", Binding = new Binding("Content") });
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

            }
            
            new Thread(new ThreadStart(delegate()
            {
                if (type == "通话")
                { 

                }
                else if (type == "短消息")
                {
                    List<ReportSms> reportitems = GetMessageReport(strart, end, reporttarget, page, total);
                    m_Main.Dispatcher.Invoke(new Action(() =>
                    {
                        reportdata.ItemsSource = reportitems;
                    })); 
                   
                }
                else if (type == "位置信息")
                {
                    List<ReportGps> reportitems = GetPositionReport(strart, end, reporttarget, page, total);
                    m_Main.Dispatcher.Invoke(new Action(() =>
                    {
                        reportdata.ItemsSource = reportitems;
                    })); 
                }          
            })).Start();

            return reportdata;
        }

        private void UpdateReportView()
        {
            try
            {
                int reporttarget = -1;

                string type = ((ComboBoxItem)m_Main.cmb_ReportType.SelectedItem).Content as string;

                DateTime? strart = m_Main.dam_Start.Value;
                DateTime? end = m_Main.dam_End.Value;

                int record_type = 79;
                int call_type = 40;

                try
                {
                    reporttarget = int.Parse(m_Main.txt_ReportTraget.Text);
                }
                catch
                {
                }

                int count = 0;

                if (type == "通话")
                {

                }
                else if (type == "短消息")
                {
                    count = SmsReportCount(strart, end, reporttarget);

                }
                else if (type == "位置信息")
                {
                     count = GpsReportCount(strart, end, reporttarget);
                }       


                ((TabItem)m_Main.tab_Report.SelectedItem).Tag = new ReportInfo()
                {
                    Name = m_Main.txt_ReportName.Text,
                    Type = m_Main.cmb_ReportType.SelectedIndex,
                    Start = m_Main.dam_Start.Value,
                    End = m_Main.dam_End.Value,
                    Traget = m_Main.txt_ReportTraget.Text,
                    RecordType = record_type,
                    CallType = call_type,
                    TotalRecord = count,
                    CurrentPage = 1
                };

                ((TabItem)m_Main.tab_Report.SelectedItem).Header = m_Main.txt_ReportName.Text + ".log";
                Grid gd = ((TabItem)m_Main.tab_Report.SelectedItem).Content as Grid;

                if (gd.Children.Count <= 0 || (string)(gd.Children[0] as DataGrid).Tag != type)
                {
                    gd.Children.Clear();
                    gd.Children.Insert(0, FillReportPage(null, 1, count) );
                }
                else
                { 
                    FillReportPage(gd.Children[0] as DataGrid, 1, count);
                }
 
               
            }
            catch
            {

            }
        }

        private void NewReport(int page)
        {
            TabItem reportItem = new TabItem();
            reportItem.Header = m_Main.txt_ReportName.Text + ".log";
            reportItem.Style = m_Main.Resources["TabItemReportStyle"] as Style;

            Grid ConnectGrid = new Grid();

            DataGrid reportdata = new DataGrid() { Style = m_Main.Resources["DataGridReportStyle"] as Style , AutoGenerateColumns = false};

            reportdata.Columns.Add(new DataGridTextColumn() { Header = "类型", Binding = new Binding("Type") });
            reportdata.Columns.Add(new DataGridTextColumn() { Header = "日期", Binding = new Binding("Date") });
            reportdata.Columns.Add(new DataGridTextColumn() { Header = "对讲机", Binding = new Binding("TargetID") });
            reportdata.Columns.Add(new DataGridTextColumn() { Header = "内容", Binding = new Binding("Content") });


            ConnectGrid.Children.Insert(0, reportdata);
            reportItem.Content = ConnectGrid;
            m_Main.tab_Report.Items.Insert(m_Main.tab_Report.Items.Count - 1, reportItem);
            m_Main.tab_Report.SelectedIndex = m_Main.tab_Report.Items.Count - 2;
            ReportIndex++;


            string reporttype = "全部";
            if( null!= m_Main.cmb_ReportType.SelectedItem)reporttype = ((ComboBoxItem)m_Main.cmb_ReportType.SelectedItem).Content as  string;
            int reporttarget = -1;

            DateTime ? strart = m_Main.dam_Start.Value;
            DateTime ? end = m_Main.dam_End.Value;

            try{
                reporttarget = int.Parse(m_Main.txt_ReportTraget.Text);
            }
            catch{
            }

            new Thread(new ThreadStart(delegate()
            {
                List<ReportItem> reportitems = GetReport(reporttype, strart, end, reporttarget, page);

                m_Main.Dispatcher.Invoke(new Action(() =>
                {
                    reportdata.ItemsSource = reportitems; 
                }));
            })).Start();
        }

        private List<ReportItem> GetReport(string Type, DateTime ? Start, DateTime ? End, int Target, int page)
        {
            List<ReportItem> reportdata = new List<ReportItem>();


            //if (Type == "通话")
            //{
            //}
            //else if ( Type == "短消息")
            //{
            //    List<object> reports = GetMessageReport(Start, End, Target, page);
            //    if (null != reports) reportdata.AddRange(reports);
            //}
            //else if (Type == "位置信息")
            //{
            //    List<ReportItem> reports = GetPositionReport(Start, End, Target,page);
            //    if (null != reports) reportdata.AddRange(reports);
            //}

            //reportdata.OrderBy(p => p.Date);

            return reportdata;
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

        public List<ReportSms> GetMessageReport(DateTime? Start, DateTime? End, int Target, int page, int count)
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
                return LogServer.Call(str, ParseSms) as List<ReportSms>;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        private List<ReportSms> ParseSms(object obj)
        {
            try
            {
                if (obj == null) return null;

                List<ReportSms> reports = new List<ReportSms>();

                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<object>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<object>>>(JsonConvert.SerializeObject(rep.contents));

                foreach (object item in Dic["records"])
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

        public List<ReportGps> GetPositionReport(DateTime? Start, DateTime? End, int Target, int page, int count)
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
                return LogServer.Call(str, ParseGps) as List<ReportGps>;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        private object ParseGps(object obj)
        {
            try
            {
                if (obj == null) return null;

                List<ReportGps> reports = new List<ReportGps>();

                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<object>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<object>>>(JsonConvert.SerializeObject(rep.contents));

                foreach (object item in Dic["records"])
                {
                    Dictionary<string, string> gps = JsonConvert.DeserializeObject<Dictionary<string, string>>(JsonConvert.SerializeObject(item));

                    reports.Add(new ReportGps()
                    {
                        Time = gps["created_at"],
                        TargetID = int.Parse(gps["radio"]),
                        Lat = gps["latitude"],
                        Log  =  gps["logitude"],
                        Vel  = gps["velocity"],
                        Alt  = gps["altitude"],
                    });

                }

                return reports;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }
    }
}
