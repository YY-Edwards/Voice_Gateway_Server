using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Configuration.Install;
using System.ServiceProcess;
using System.Diagnostics;

namespace ServerManager
{
    public class SvrMgr
    {
        public string Name{get; set;}
        public string Path { get; set; }

        public SvrMgr(string name, string path)
        {
            Name = name;
            Path = path;
        }

        public bool IsInstalled { get {
            try
            {
                ServiceController[] services = ServiceController.GetServices();
                foreach (ServiceController service in services)
                {
                    if (service.ServiceName == Name)
                    {
                        return true;
                    }
                }
                return false;
            }

            catch
            { 
                return false;
            }            
        }
        }

        public bool IsStarted { get {
            bool result = true;
            try
            {

                ServiceController[] services = ServiceController.GetServices();
                foreach (ServiceController service in services)
                {
                    if (service.ServiceName == Name)
                    {
                        if ((service.Status == ServiceControllerStatus.Stopped)
                            || (service.Status == ServiceControllerStatus.StopPending))
                        {
                            result = false;
                        }
                    }
                }
            }
            catch { }
            return result;  
        } }


        public bool Install()
        {
            if (!System.IO.File.Exists(Path))
            {
                Console.WriteLine(string.Format("Install {0} Error\r\n{1}", Name, "Can not Found " + Path));
                return false;
            }
            try
            {
                if (!IsInstalled)
                {
                    ProcessStartInfo a = new ProcessStartInfo(Path ,"install");                     
                    a.WindowStyle = ProcessWindowStyle.Hidden;  
                    Process process = Process.Start(a);
                   
                    Console.WriteLine(string.Format("Install {0} Sucecss ", Name));
                    return true;
                }
                else
                {
                    Console.WriteLine(string.Format("Install {0} Error,{0} Already Exists", Name));
                    return false;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(string.Format("Install {0} Error\r\n{1}",Name, ex.Message) );
                return false;
            }
        }

        public bool Uninstall()
        {
            try
            {
                if(IsStarted)
                {
                    Stop();
                    System.Threading.Thread.Sleep(300);
                }

                if (IsInstalled)
                {
                    ProcessStartInfo a = new ProcessStartInfo(@"c:/windows/system32/cmd.exe","/c  sc delete " + Name);  
                    a.WindowStyle = ProcessWindowStyle.Hidden;
                    Process process = Process.Start(a);
                    Console.WriteLine(string.Format("Uninstall {0} Sucecss ", Name));

                    return true;
                }
                else
                {
                    Console.WriteLine(string.Format("Uninstall {0} Error,{0} not Exists", Name));
                    return false;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(string.Format("Uninstall {0} Error\r\n{1}", Name, ex.Message));
                return false;
            }
        }


        public void Update()
        {
            if(IsInstalled)Uninstall();
            System.Threading.Thread.Sleep(300);
            Install();
        }

        public bool Start()
        {
            try
            {
                if (IsInstalled)
                {
                    ProcessStartInfo a = new ProcessStartInfo(@"c:/windows/system32/cmd.exe", "/c  net start " + Name);
                    a.WindowStyle = ProcessWindowStyle.Hidden;
                    Process process = Process.Start(a);
                    Console.WriteLine(string.Format("Start {0} Sucecss ", Name));
                    return true;
                }
                else
                {
                    Console.WriteLine(string.Format("Start {0} Error,{0} not Exists", Name));
                    return false;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(string.Format("Start {0} Error\r\n{1}", Name, ex.Message));
                return false;
            }
        }

        public bool Stop()
        {
            try
            {
                if (IsInstalled)
                {
                    ProcessStartInfo a = new ProcessStartInfo(@"c:/windows/system32/cmd.exe","/c  net stop " + Name);  
                    a.WindowStyle = ProcessWindowStyle.Hidden;
                    Process process = Process.Start(a);
                    Console.WriteLine(string.Format("Stop {0} Sucecss ", Name));
                    return true;
                }
                else
                {
                    Console.WriteLine(string.Format("Stop {0} Error,{0} not Exists", Name));
                    return false;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(string.Format("Stop {0} Error\r\n{1}", Name, ex.Message));
                return false;
            }
        }
    }
}
