using System;
using System.ComponentModel;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Interop;
using System.Collections.Generic;
using System.Linq;

namespace Manager.Models
{
    public class Utility
    {
        //private static string Host = "192.168.2.122";
        private static string Host = "127.0.0.1";

        public static string TServerHost{get{return Host;} }
        public static string LogServerHost { get { return Host; } }
        public static string HttpServerHost { get { return Host; } }


       
        public static int TServerPort = 9000;       
        public static int LogServerPort = 9003;     
        public static int HttpServerPort = 8001;


        public static List<double> GeneralQueryIntervals = new List<double>() { 30, 60, 120, 240 };
        public static List<double> CSBKQueryIntervals = new List<double>() { 7.5, 20, 30, 40, 60 };
        public static List<double> EnhCSBKQueryIntervals = new List<double>() { 7.5, 15, 30, 60, 120, 240, 480 };   
    }

    public static class Extension
    {
        public static bool Contains<T>(this List<T> list, Predicate<T> match)
        {
            return list.Find(match) != null;
        }

    }
}
