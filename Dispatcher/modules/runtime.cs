using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Dispatcher.Modules
{
    public class Runtime
    {
        public static void Notify(string contents)
        {
            Runtime.Instance().Notify(new Runtime(contents));
        }

        private volatile static Runtime _instance = null;
        private static readonly object lockHelper = new object();

        public static Runtime Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new Runtime();
                }
            }
            return _instance;
        }

        public Runtime()
        {

        }
        public Runtime(string contents)
        {
            Content = contents;
            Time = DateTime.Now;
        }
        public void Notify(Runtime contents)
        {
            if (OnNotify != null) OnNotify(contents);
        }
        public event Action<Runtime> OnNotify;
        public string Content { set; get; }
        public DateTime Time { set; get; }
    }
}
