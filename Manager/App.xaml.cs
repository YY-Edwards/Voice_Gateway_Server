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

using Newtonsoft.Json;
//using Sigmar.Logger;

using Manager.Views;


namespace Manager
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {  protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            CheckAdministrator();

            //Logs.SetStartUpWindow(typeof(Main));
           // Log.Initialize(runTimeDirectory, Name + Version);
            //StartupUri = new Uri("Views/Logger/Logs.xaml", UriKind.RelativeOrAbsolute);
            //Log.Info("Startup application in Administrator.");

            StartupUri = new Uri("Views/Main.xaml", UriKind.RelativeOrAbsolute);

        }  
        private void CheckAdministrator()
        {
            var wi = WindowsIdentity.GetCurrent();
            var wp = new WindowsPrincipal(wi);

            bool runAsAdmin = wp.IsInRole(WindowsBuiltInRole.Administrator);

            if (!runAsAdmin)
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
        }  


        public static string Company = "JiHua Information";
        public static string Name = "TrboX";
        public static string Version = "3.0";
        public static string SettingTempFile = "tmp.setting.xml";
        public static string ResourceTempFile = "tmp.Resource.db";

        public static string TmpDirectory
        {
            get
            {
                if (!Directory.Exists(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\"))
                {
                    Directory.CreateDirectory(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\");
                }
                return Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\";
            }
        }

        public static string runTimeDirectory
        {
            get
            {
                if (!Directory.Exists(TmpDirectory + "runtime\\"))
                {
                    Directory.CreateDirectory(TmpDirectory + "runtime\\");
                }
                return TmpDirectory + "runtime\\";

            }
        }

        public static string AudioDirectory
        {
            get
            {
                if (!Directory.Exists(TmpDirectory + "voice\\"))
                {
                    Directory.CreateDirectory(TmpDirectory + "voice\\");
                }
                return TmpDirectory + "voice\\";

            }
        }
        public static string SettingTempPath
        {
            get
            {
                return TmpDirectory + SettingTempFile;
            }
        }
        public static string ResourceTempPath
        {
            get
            {
                return TmpDirectory + ResourceTempFile;
            }
        }
    }

}
