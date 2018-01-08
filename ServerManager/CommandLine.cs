using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerManager
{

    internal enum ParameterType
    {
        Numerical,
        Boolean,
        String,
        //File,
    }
    internal class CommandParameter
    {
        public string Name;
        public string longName;
        public ParameterType Type;
        public object Value;
        public bool IsVaild;

        public CommandParameter(string name, string longname, ParameterType type)
        {
            Name = name.ToLower();
            longName = longname;
            Type = type;
            IsVaild = false;
        }

        public CommandParameter(string name, ParameterType type, object value)
        {
            Name = name.ToLower();
            Type = type;
            Value = value;
            IsVaild = true;
        }

        public new string ToString()
        {
            return string.Format("{0}<{1}> {2}", Name, Type.ToString().ToLower(), longName);
        }
    }

    internal class CommandLine
    {
        public string Command { get; set; }
        public string Dept { get; set; }
        public Dictionary<string, CommandParameter> Parameters { set; get; }
        public Action<Dictionary<string, CommandParameter>> WhatExecute;

        public CommandLine(string command, Dictionary<string, CommandParameter> parameters, Action<Dictionary<string, CommandParameter>> action, string dept)
        {
            Command = command.ToLower();
            Parameters = parameters;
            WhatExecute = action;
            Dept = dept;
        }

        public CommandLine()
        {

        }

        public bool Execute(string commandline)
        {         
            if (commandline == null || WhatExecute == null) return false;

            if (Parameters == null)
            {
                WhatExecute(null);
                return true;
            }

            string[] args = MergeSpace(commandline).Trim().Split(new char[1] { ' ' });        
            if (args == null || args.Length <= 0)
            {
                WhatExecute(null);
                return true;
            }

            Dictionary<string, CommandParameter> parameters = PraseParameter(args);
            if(parameters == null)return false;
            WhatExecute(parameters);
            return true;
        }


        private static string MergeSpace(string str)
        {
            if (str != string.Empty &&
            str != null &&
            str.Length > 0
            )
            {
                str = new System.Text.RegularExpressions.Regex("[\\s]+").Replace(str, " ");
            }
            return str;
        }

        private Dictionary<string, CommandParameter> PraseParameter(string[] args)
        {         
            Dictionary<string, CommandParameter> parameters = new Dictionary<string, CommandParameter>();
            
            for (int i = 1; i < args.Length; i += 2)
            {
                if(Parameters.ContainsKey(args[i]))
                {
                    string argName = args[i].ToLower();                                  
                    if (parameters.ContainsKey(argName)) continue;
                    try
                    {
                    switch(Parameters[argName].Type)
                    {
                        case ParameterType.Numerical:
                            parameters.Add(argName, new CommandParameter(argName, ParameterType.Numerical, int.Parse(args[i + 1])));
                            break;
                        case ParameterType.Boolean:
                            parameters.Add(argName, new CommandParameter(argName, ParameterType.Boolean, bool.Parse(args[i + 1])));
                            break;
                       default:
                            parameters.Add(argName, new CommandParameter(argName, Parameters[argName].Type, args[i + 1]));
                            break;
                    }
                    }
                    catch
                    {
                        return null;
                    }
                }
            }

            return parameters;
        }

        public new string ToString()
        {
            string parameters = string.Empty;
            if(Parameters != null)
            {
                int count = 0;
                foreach(var parmaeter in Parameters)
                { 
                    count++;
                    parameters += string.Format("{0}{1}", parmaeter.Value.ToString(), count == Parameters.Count ? "" : ",");                   
                }
            }

            string empty = "                ";

            return string.Format("{0}{1}{2}{3}",
                Command,
                empty.Substring(0, empty.Length - Command.Length),
                Dept,
                parameters == string.Empty ? "" : string.Format("\r\n{0}Parameters:{1}",empty, parameters));
        }
    }

    internal class CommandLineManager
    {
        private Dictionary<string, CommandLine> CommandList;

        public CommandLineManager()
        {
            CommandList = new Dictionary<string, CommandLine>() { 
            {"help", new CommandLine("help", null, HelpText, "Display Trbox 3.1 Server Manager Help Information")}
            };           
        }

        private void HelpText(Dictionary<string, CommandParameter> args)
        {
            Console.WriteLine("[COMMAND LIST]");
            foreach(var cmd in CommandList.OrderBy(p=>p.Key))
            {
                Console.WriteLine(cmd.Value.ToString());
            }
        }

        public void AddCommand(CommandLine commandline)
        {
            if(CommandList != null)
            {
                CommandList.Add(commandline.Command, commandline);
            }
        }

        public CommandLine FindCommandAndExcute(string[] args)
        {
            string command = string.Empty;
            foreach (string s in args) command += s + " ";
            Console.WriteLine(command);
            
            if (args == null || args.Length <= 0)
            {
                Console.WriteLine("Syntax error");
            }
            else
            {
                if (CommandList.ContainsKey(args[0]) && CommandList[args[0]] != null)
                {
                    CommandList[args[0]].Execute(command);
                    return CommandList[args[0]];
                }
                else
                {
                    Console.WriteLine(string.Format("{0}is not a valid command,please input help to get command list.\r\n", args[0]));
                }
            }

            return null;
        }

        public CommandLine FindCommandAndExcute(string command)
        {
            if (command == null) return null;
            string[] args = command.Trim().Split(new char[1] { ' ' });

            if (args == null || args.Length <= 0)
            {
                Console.WriteLine("Syntax error");
            }
            else
            {
                if (CommandList.ContainsKey(args[0]) && CommandList[args[0]] != null)
                {
                    CommandList[args[0]].Execute(command);
                    return CommandList[args[0]];
                }
                else
                {
                    Console.WriteLine(string.Format("{0}is not a valid command,please input help to get command list.\r\n", args[0]));
                }
            }
            return null;
        }
    }
}
