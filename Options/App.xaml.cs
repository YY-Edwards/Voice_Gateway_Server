using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace TrboX
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        public static string Company = "JiHua Information";
        public static string Name = "TrboX";
        public static string Version = "3.0";
        public static string SettingTempFile = "tmp.setting.xml";
        public static string ResourceTempFile = "tmp.Resource.db";

        public static string Directory
        {    get
            {                    
                return Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\" + Company + "\\" + Name + "\\" + Version + "\\";
            }
        }
        public static string SettingTempPath
        {
            get
            {
                return Directory + SettingTempFile;
            }
        }
        public static string ResourceTempPath
        {
            get
            {
                return Directory + ResourceTempFile;
            }
        }

    }
}
