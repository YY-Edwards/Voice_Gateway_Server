﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Dispatcher;

namespace Dispatcher.Service
{
    public class CImgServer
    {
        private volatile static CImgServer _instance = null;
        private static readonly object lockHelper = new object();

        public static CImgServer Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new CImgServer();
                }
            }
            return _instance;
        }

        public void Upload(string name, string local)
        {
            if (!System.IO.File.Exists(local)) return;
        }
    }
}
