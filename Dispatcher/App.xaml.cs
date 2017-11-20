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
using Sigmar.Logger;

using Dispatcher.Views;

namespace Dispatcher
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            CheckAdministrator();

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
