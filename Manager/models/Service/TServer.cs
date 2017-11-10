namespace Manager.Models
{
    public class TServer : Server
    {
        #region Singleton

        private volatile static TServer _instance = null;
        private static readonly object singletonLockHelper = new object();

        public static TServer Instance()
        {
            if (_instance == null)
            {
                lock (singletonLockHelper)
                {
                    if (_instance == null)
                        _instance = new TServer();
                }
            }

            return _instance;
        }

        #endregion Singleton

        public TServer()
        {
            Host = Utility.TServerHost;
            Port = Utility.TServerPort;
        }
    }
}