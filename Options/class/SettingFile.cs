using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Newtonsoft.Json;

namespace TrboX
{
    class SettingFile
    {
        public static Setting ImportSetting(string file)
        {
            FileStream fs = new FileStream(file,FileMode.Open);
            byte[] rx = new byte[10240];
            fs.Read(rx, 0, (int)fs.Length);
            string str = Encoding.ASCII.GetString(rx);
            str = str.Substring(str.IndexOf("{"), str.LastIndexOf("}") - str.IndexOf("{") + 1);
            return JsonConvert.DeserializeObject<Setting>(str);
        }

        //public static string ImportResource(string file)
        //{
        //    return "";
        //}

        public static void ExportSetting(string path, Setting setting)
        {
            System.DateTime startTime = TimeZone.CurrentTimeZone.ToLocalTime(new System.DateTime(1970,1,1,0,0,0,0));
            long timeticks = (DateTime.Now.Ticks - startTime.Ticks);
            string file = path + "\\Trbox3.0 Setting " + (timeticks / 10000000).ToString() + ".trbox.cfg";
            StreamWriter sw = new StreamWriter(file);
            sw.Write(JsonConvert.SerializeObject(setting));
            sw.Flush();
            sw.Close();
        }

    }
}
