using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.IO;

using System.Security.Principal;
using System.Diagnostics;
using System.Reflection;
using Dispatcher;

using Dispatcher.Views;

namespace Dispatcher
{

    public class Log
    {
        public static void Initialize(string dir, string title, int buffersize = 100)
        {
            //Log.Initialize(dir,name);
        }
        private static Action<LogContent> Onmessage;
        public static void BindingMessage(Action<LogContent> onnotify)
        {
            Onmessage = onnotify;
        }

        public static void Message(string message)
        {
            if (Onmessage != null) Onmessage(new  LogContent(LogMode_t.MESSAGE, message));
        }

        public static void Info(string info)
        {
        }

        public static void Debug(string debug)
        {
        }


        public static void Warning(Exception ex)
        {
        }


        public static void Error(Exception ex)
        {
        }


        public static void Fatal(Exception ex)
        {
        }
        public static void Warning(string warning = null, Exception ex = null)
        {
        }


        public static void Error(string error, Exception ex = null)
        {
        }


        public static void Fatal(string fatal, Exception ex = null)
        {
        }

        public static void Report(string str, bool istx = true)
        {
        }

        public static void Report(byte[] bytes, bool istx = true)
        {
        }
        public static void Report(ReportType_t type, byte[] bytes, bool istx, string source, string target, string parse = null)
        {
        }

        public static void Report(ReportType_t type, string str, bool istx, string source, string target, string parse = null)
        {
        }
    }

    public enum LogMode_t
    {
        None = 0,
        ALL = 0xFFFF,
        INFO = 0x0001,
        DEBUG = 0x0002,
        WARNING = 0x0004,
        ERROR = 0x0008,
        FATAL = 0x0010,
        REPORT = 0x0011,
        MESSAGE = 0x0012,
    }
    public enum ReportType_t
    {
        UnKnown,
        Text,
        Json,
        Hex,
        Dec,
    }

    public class LogContent
    {
        public DateTime Time;
        private LogMode_t Mode;
        public string Message;
        private string Method;
        private Exception Ex;
        private int Code;
        private Report Reports;

        public LogContent(LogMode_t mode, string message, string method = "", Exception ex = null, int code = 0)
        {
            Mode = mode;
            Message = message;
            Method = method;
            Ex = ex;
            Code = 0;
            Reports = null;
        }
        public LogContent(Report report)
        {
            Mode = LogMode_t.REPORT;
            Reports = report;
        }


        public LogMode_t GetMode()
        {
            return Mode;
        }

        public bool HasException()
        {
            return Ex != null;
        }

        private string ExMessage { get { return Ex == null ? "" : Ex.Message; } }



        private static long lastticks = 0;
        private static int ticksappend = 0;
        public string ToHtml()
        {
            return "";
        }


        public string ToString()
        {
            return "";
        }
        public string ToSimpleString()
        {
            return "";
        }

        public string ToString(string format)
        {
            return "";
        }

    }
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            CheckAdministrator();


            this.DispatcherUnhandledException += (sender, args) =>
            {
                args.Handled = true;
            };

            AppDomain.CurrentDomain.UnhandledException += (sender, args) =>
            {
            };


            Logs.SetStartUpWindow(typeof(Login));
            Log.Initialize(RuntimeDir, ProjectName + Version);
            StartupUri = new Uri("views/logger/logger.xaml", UriKind.RelativeOrAbsolute);
            Log.Info("Startup application in Administrator.");
           
        }


        private void CheckAdministrator()
        {
            var wi = WindowsIdentity.GetCurrent();
            var wp = new WindowsPrincipal(wi);

            bool runAsAdmin = wp.IsInRole(WindowsBuiltInRole.Administrator);

            if (!runAsAdmin)
            {
                Restart();
            }
        }  

        public static void Restart()
        {
            // It is not possible to launch a ClickOnce app as administrator directly,  
            // so instead we launch the app as administrator in a new process.  
            var processInfo = new ProcessStartInfo(Assembly.GetExecutingAssembly().CodeBase);

            // The following properties run the new process as administrator  
            processInfo.UseShellExecute = true;
            processInfo.Verb = "runas";

            // Start the new process  
            try
            {
                Process.Start(processInfo);
            }
            catch (Exception ex)
            {
                //ex.WriteLog();
            }

            // Shut down the current process  
            Environment.Exit(0);
        }
        
        private static string Company = "JiHua Information";

        private static string ProjectName = "TrboX";
        private static string Version = "3.0";
        private static string Name = "";

        private static string RuntimeFolder = "runtime";

        private static string DatabaseFolder = "database";
        private static string DatabaseFile = "resource.db";

        private static string ConfigFolder = "config";
        private static string ConfigFile = "localconfig.json";


        public static string CurrentDir
        {
            get
            {
                string currentdir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + ProjectName + "\\" + Name + "" + Version + "\\";
                if (!Directory.Exists(currentdir))
                {
                    Directory.CreateDirectory(currentdir);
                }
                return currentdir;
            }
        }

        public static string RuntimeDir
        {
            get
            {
                if (!Directory.Exists(CurrentDir + RuntimeFolder + "\\"))
                {
                    Directory.CreateDirectory(CurrentDir + RuntimeFolder + "\\");
                }
                return CurrentDir + RuntimeFolder + "\\";
            }
        }

        public static string DatabaseDir
        {
            get
            {
                if (!Directory.Exists(CurrentDir + DatabaseFolder + "\\"))
                {
                    Directory.CreateDirectory(CurrentDir + DatabaseFolder + "\\");
                }
                return CurrentDir + DatabaseFolder + "\\";
            }
        }

        public static string ConfigDir
        {
            get
            {
                if (!Directory.Exists(CurrentDir + ConfigFolder + "\\"))
                {
                    Directory.CreateDirectory(CurrentDir + ConfigFolder + "\\");
                }
                return CurrentDir + ConfigFolder + "\\";
            }
        }



        public static string ConfigPath
        {
            get
            {
                return ConfigDir + ConfigFile;
            }
        }
        public static string DatabasePath
        {
            get
            {
                return DatabaseDir + DatabaseFile;
            }
        }
        public static string RuntimePath
        {
            get
            {
                DateTime dt = DateTime.Now;
                DateTime startWeek = dt.AddDays(1 - Convert.ToInt32(dt.DayOfWeek.ToString("d")));
                DateTime endWeek = startWeek.AddDays(6);

                string runtimefile = startWeek.ToString("yyyy-MM-dd-") + endWeek.ToString("yyyy-MM-dd") + ".lg";
                return RuntimeDir + runtimefile;
            }
        }

        public static string QuickListPath
        {
            get
            {
                return RuntimeDir + "trbox._quicklist";
            }
        }



        //public static string Company = "JiHua Information";
        //public static string Name = "TrboX";
        //public static string Version = "3.1";
        //public static string NotifyTempFile = "tmp.notify";
        //public static string WorkSpaceTempFile = "tmp.workspace";
        //public static string SettingTempFile = "tmp.setting.xml";
        //public static string ResourceTempFile = "tmp.Resource.db";
        //public static string ReportTempFile = "tmp.report";

        //public static string WorkDirectory
        //{
        //    get
        //    {
        //        if (!Directory.Exists(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\"))
        //        {
        //            Directory.CreateDirectory(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\");
        //        }
        //        return Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\";
        //    }
        //}

        //public static string runTimeDirectory
        //{
        //    get
        //    {
        //        if (!Directory.Exists(TmpDirectory + "runtime\\"))
        //        {
        //            Directory.CreateDirectory(TmpDirectory + "runtime\\");
        //        }
        //        return TmpDirectory + "runtime\\";

        //    }
        //}
        //public static string SettingTempPath
        //{
        //    get
        //    {
        //        return TmpDirectory + SettingTempFile;
        //    }
        //}
        //public static string ResourceTempPath
        //{
        //    get
        //    {
        //        return TmpDirectory + ResourceTempFile;
        //    }
        //}
        //public static string NotifyTempPath
        //{
        //    get
        //    {
        //        return TmpDirectory + NotifyTempFile;
        //    }
        //}
        //public static string WorkSpaceTempPath
        //{
        //    get
        //    {
        //        return TmpDirectory + WorkSpaceTempFile;
        //    }
        //}

        //public static string ReportTempPath
        //{
        //    get
        //    {
        //        return TmpDirectory + ReportTempFile;
        //    }
        //}

    }
}
