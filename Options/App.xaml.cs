using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using Newtonsoft.Json;
using System.IO;

using System.Security.Principal;
using System.Diagnostics;
using System.Reflection;

namespace TrboX
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
            //如果不是管理员，程序会直接退出，并使用管理员身份重新运行。  
            StartupUri = new Uri("win/Main/Main.xaml", UriKind.RelativeOrAbsolute);
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

    public class Trbox
    {
        public static bool IsNumber(string str)
        {
            return ((str != "") && System.Text.RegularExpressions.Regex.IsMatch(str, @"^\d+$"));
        }

        public static bool Compare(object src, object dest)
        {
            return JsonConvert.SerializeObject(src) == JsonConvert.SerializeObject(dest);
        }
    }

}
