using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

using System.Configuration.Install;
using System.ServiceProcess;
using System.Diagnostics;

namespace ServerManager
{
    class Program
    {
        private static CommandLineManager _commandLineManager;

        private static SvrMgr TServer, LogServer,Dispatcher, Wirelan;

        static void Main(string[] args)
        {
           
            _commandLineManager = new CommandLineManager();

             Console.WriteLine("Register Install Method");
             _commandLineManager.AddCommand(new CommandLine("install", new Dictionary<string, CommandParameter>() { { "-t", new CommandParameter("-t", "target", ParameterType.String) }, { "-a", new CommandParameter("-a", "y|n", ParameterType.String) } }, Install, "Install Server"));
            
             Console.WriteLine("Register Uninstall Method");
            _commandLineManager.AddCommand(new CommandLine("uninstall", new Dictionary<string, CommandParameter>() { { "-t", new CommandParameter("-t", "target", ParameterType.String) } }, Uninstall, "Uninstall Server"));

            Console.WriteLine("Register Update Method");
            _commandLineManager.AddCommand(new CommandLine("update", new Dictionary<string, CommandParameter>() { { "-t", new CommandParameter("-t", "target", ParameterType.String) }, { "-a", new CommandParameter("-a", "y|n", ParameterType.String) } }, Update, "Update Server"));

            Console.WriteLine("Register Start Method");
            _commandLineManager.AddCommand(new CommandLine("start", new Dictionary<string, CommandParameter>() { { "-t", new CommandParameter("-t", "target", ParameterType.String) } }, Start, "Start Server"));

            Console.WriteLine("Register Stop Method");
            _commandLineManager.AddCommand(new CommandLine("stop", new Dictionary<string, CommandParameter>() { { "-t", new CommandParameter("-t", "target", ParameterType.String) } }, Stop, "Stop Server"));


            string SvrPath = Directory.GetParent(Directory.GetParent(AppDomain.CurrentDomain.BaseDirectory).FullName) + @"\Svr\";
            string DevicePath = Directory.GetParent(Directory.GetParent(AppDomain.CurrentDomain.BaseDirectory).FullName) + @"\Device\";

            Console.WriteLine(string.Format("Svr Path:{0}", SvrPath));
            Console.WriteLine(string.Format("Device Path Path:{0}", DevicePath));

            Console.WriteLine("Initialize TServer");
            TServer = new SvrMgr("Trbox.TServer", SvrPath + @"TServer.exe");
            if (!File.Exists(SvrPath + @"TServer.exe")) Console.WriteLine("Warning: Can not Found " + SvrPath + @"TServer.exe");

            Console.WriteLine("Initialize LogServer");
            LogServer = new SvrMgr("Trbox.Log", SvrPath + @"LogServer.exe");
            if (!File.Exists(SvrPath + @"LogServer.exe")) Console.WriteLine("Warning: Can not Found " + SvrPath + @"LogServer.exe");
            else LogServer = new SvrMgr("Trbox.Log", SvrPath + @"LogServer.exe");

            Console.WriteLine("Initialize Dispatcher");
            Dispatcher = new SvrMgr("Trbox.Dispatch", DevicePath + @"Dispatch.exe");
            if (!File.Exists(DevicePath + @"Dispatch.exe")) Console.WriteLine("Warning: Can not Found " + DevicePath + @"Dispatch.exe");
            else Console.WriteLine("Can not Found " + DevicePath + @"Dispatch.exe");

            Console.WriteLine(string.Format("Initialize Wirelan", SvrPath));
            Wirelan = new SvrMgr("Trbox.Wirelan", DevicePath + @"WirelineSchedule.exe");
            if (!File.Exists(DevicePath + @"WirelineSchedule.exe")) Console.WriteLine("Warning: Can not Found " + DevicePath + @"WirelineSchedule.exe");
            

            Console.WriteLine("");

            try
            {
                File.Copy(AppDomain.CurrentDomain.BaseDirectory + "ServerManager.exe", Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "/ServerManager.exe", true);
            }
            catch
            {

            }

            if (args.Length <= 0)
            {
                new Task(WaitConsoleInput).Start();
                while (true) System.Threading.Thread.Sleep(10000);
            }
            else
            {
                _commandLineManager.FindCommandAndExcute(args);
            }  
        }


        private static void Install(Dictionary<string, CommandParameter> args)
        {
            List<SvrMgr> svrs = GetSvr(args);
            bool autoStart = false;
            if (args.ContainsKey("-a") && args["-a"].IsVaild && args["-a"].Type == ParameterType.String && args["-a"].Value as string != null && (args["-a"].Value as string).ToLower() == "y")
            {
                autoStart = true;
            }

            if(svrs == null)return;
            foreach (SvrMgr svr in svrs)
                if (svr != null)
                {
                    svr.Install();
                    if (autoStart)
                    {
                        System.Threading.Thread.Sleep(300);
                        svr.Start();
                    }
                }

        }

        private static void Uninstall(Dictionary<string, CommandParameter> args)
        {
            List<SvrMgr> svrs = GetSvr(args);
            if (svrs == null) return;
            foreach (SvrMgr svr in svrs) if (svr != null) svr.Uninstall();
        }

        private static void Update(Dictionary<string, CommandParameter> args)
        {
            List<SvrMgr> svrs = GetSvr(args);
            if (svrs == null) return;

            bool autoStart = false;
            if (args.ContainsKey("-a") && args["-a"].IsVaild && args["-a"].Type == ParameterType.String && args["-a"].Value as string != null && (args["-a"].Value as string).ToLower() == "y")
            {
                autoStart = true;
            }

            foreach (SvrMgr svr in svrs) if (svr != null)
                {
                    svr.Update();
                    if (autoStart)
                    {
                        System.Threading.Thread.Sleep(300);
                        svr.Start();
                    }
                }
        }
        private static void Start(Dictionary<string, CommandParameter> args)
        {
            List<SvrMgr> svrs = GetSvr(args);
            if (svrs == null) return;
            foreach (SvrMgr svr in svrs) if (svr != null) svr.Start();
        }
        private static void Stop(Dictionary<string, CommandParameter> args)
        {
            List<SvrMgr> svrs = GetSvr(args);
            if (svrs == null) return;
            foreach (SvrMgr svr in svrs) if (svr != null) svr.Stop();
        }

        private static List<SvrMgr> GetSvr(Dictionary<string, CommandParameter> args)
        {
            if (args == null || !args.ContainsKey("-t") || args["-t"] == null || !args["-t"].IsVaild || args["-t"].Value == null)
            {
               Console.WriteLine("Please specify the service -t[all|tserver|logserver|dispatch|wirelan]");
               return null;
            }

            List<SvrMgr>  svrs = new List<SvrMgr>();
            switch (args["-t"].Value as string)
            {
                case "all":
                    svrs.Add(TServer);
                    svrs.Add(LogServer);
                    svrs.Add(Dispatcher);
                    svrs.Add(Wirelan);
                    break;
                case "tserver":
                    svrs.Add(TServer);
                    break;
                case "logserver":
                 svrs.Add(LogServer);
                    break;
                case "dispatch":
                    svrs.Add(Dispatcher);
                    break;
                case "wirelan":
                    svrs.Add(Wirelan);
                    break;
                default:
                    Console.WriteLine("Please specify the service -t[all|tserver|logserver|dispatch|wirelan]");
                    break;
            }

            return svrs;
        }


        private static void WaitConsoleInput()
        {
            while (true)
            {
                Console.Write("\r\nINPUT>");
                string input = Console.ReadLine();
                _commandLineManager.FindCommandAndExcute(input);
            }
        }
    }
}
