using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using Newtonsoft.Json;
using System.IO;

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
        public static string NotifyTempFile = "tmp.notify";
        public static string WorkSpaceTempFile = "tmp.workspace";
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
        public static string NotifyTempPath
        {
            get
            {
                return TmpDirectory + NotifyTempFile;
            }
        }
        public static string WorkSpaceTempPath
        {
            get
            {
                return TmpDirectory + WorkSpaceTempFile;
            }
        }

    }

    public class Trbox
    {
        public static bool IsNumber(string str)
        {
            return ((str != null) && (str != "") && System.Text.RegularExpressions.Regex.IsMatch(str, @"^-*\d+$"));
        }

        public static bool Compare(object src, object dest)
        {
            return JsonConvert.SerializeObject(src) == JsonConvert.SerializeObject(dest);
        }
    }
}
