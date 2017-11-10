using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Manager.Models
{
    public class Message
    {
        #region Singleton

        private volatile static Message _instance = null;
        private static readonly object singletonLockHelper = new object();

        public static Message Instance()
        {
            if (_instance == null)
            {
                lock (singletonLockHelper)
                {
                    if (_instance == null)
                        _instance = new Message();
                }
            }

            return _instance;
        }

        #endregion Singleton


        private Queue<CustomMessage_t> _customMessages;
        private Semaphore _waitACustomMessage;
        private int _maxCustomMessageCount = 100;

        private Message()
        {
            _customMessages = new Queue<CustomMessage_t>();
            _waitACustomMessage = new Semaphore(0, _maxCustomMessageCount);
            new Task(BeginInvokeCustomMessage).Start();
        }

        
        public event CustomMessageHandler CustomMessageReceived;


        public void PostCustomMessage(object sender, CustomMessageArgs parameter)
        {
            lock (_customMessages)
            {
                try
                {
                    _customMessages.Enqueue(new CustomMessage_t(sender, parameter));
                    _waitACustomMessage.Release();
                }
                catch
                {

                }
            }
        }

        private void BeginInvokeCustomMessage()
        {
            while(true)
            {
                _waitACustomMessage.WaitOne();
                lock(_customMessages)
                {
                    if(_customMessages.Count > 0)
                    {
                        CustomMessage_t message = _customMessages.Dequeue();
                        if (CustomMessageReceived != null) CustomMessageReceived(message.Sender, message.Parameter);
                    }                  
                }
            }
        }

        public void SendCustomMessage(object sender, CustomMessageArgs parameter)
        {
            if (CustomMessageReceived != null) CustomMessageReceived(sender, parameter);
        }

        private struct CustomMessage_t
        {
            public object Sender;
            public CustomMessageArgs Parameter;
            public CustomMessage_t(object sender, CustomMessageArgs e)
            {
                Sender = sender;
                Parameter = e;
            }
        }
    }

    public delegate void CustomMessageHandler(object sender, CustomMessageArgs e);
    public class CustomMessageArgs
    {
        public Messages Message;
        public object Parameter;

        public CustomMessageArgs(Messages messages, object parameter = null)
        {
            Message = messages;
            Parameter = parameter;
        }
    }


    public enum Messages
    {
        OpenNotifyWindow,
        SetNotifyEnable,
        AddNotifyLine,
        ClearNotify,
    }
}
