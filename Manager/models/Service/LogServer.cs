namespace Manager.Models
{
    public class LogServer : Server
    {
        #region Singleton

        private volatile static LogServer _instance = null;
        private static readonly object singletonLockHelper = new object();

        public static LogServer Instance()
        {
            if (_instance == null)
            {
                lock (singletonLockHelper)
                {
                    if (_instance == null)
                        _instance = new LogServer();
                }
            }

            return _instance;
        }

        #endregion Singleton

        public LogServer()
        {
            Host = Utility.LogServerHost;
            Port = Utility.LogServerPort;
        }
    }
}