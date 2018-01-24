using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading;
using System.Threading.Tasks;

namespace Sigmar
{
    public abstract class Transceiver
    {
        protected event Action<long> WaitResponseTimeout;
        protected event Action<long> WaitReplyTimeout;

        private int _replyTimeout;
        private int _responseTimeout;
        private int _sendTimes;

        protected Transceiver()
            : this(60000, 1000, 3)
        {
        }

        protected Transceiver(int replytimeout, int responsetimeout, int sendtimes)
        {
            _replyTimeout = replytimeout;
            _responseTimeout = responsetimeout;
            _sendTimes = sendtimes;

            new Task(CheckProcess).Start();
        }


        protected void OnReceiveResponse(long seq, object response)
        {
            if (response == null) return;

            if (_currentTxContent != null && _currentTxContent.Sequence == seq)
            {
                try
                {
                    _currentTxContent.SetResponse(response);
                    _waitcurrentResponse.Release();
                }
                catch
                {

                }
            }

            lock (_listHelper)
            {
                if (_sentList.ContainsKey(seq) && _sentList[seq] != null && _sentList[seq].Sequence == seq)
                {
                    try
                    {
                        _sentList[seq].SetResponse(response);
                    }
                    catch
                    {

                    }
                }
            }
        }

        protected void OnReceiveReply(long seq, object reply)
        {
            if (reply == null) return;

            if (_currentTxContent != null && _currentTxContent.Sequence == seq)
            {
                try
                {
                    _currentTxContent.SetReply(reply);
                    _waitcurrentReply.Release();
                }
                catch
                {

                }
            }


            lock (_listHelper)
            {
                if (_sentList.ContainsKey(seq) && _sentList[seq] != null && _sentList[seq].Sequence == seq)
                {
                    try
                    {
                        _sentList[seq].SetReply(reply);
                    }
                    catch
                    {

                    }
                }
            }
        }
        protected abstract bool SendBytes(byte[] bytes);

        private readonly object _currentHelper = new object();
        private readonly object _currentTxContentLockHelper = new object();

        private TxContent _currentTxContentObj = null;


        private TxContent _currentTxContent { get { lock (_currentTxContentLockHelper)return _currentTxContentObj; } set { lock (_currentTxContentLockHelper)_currentTxContentObj = value; } }
        private Semaphore _waitcurrentResponse = null;
        private Semaphore _waitcurrentReply = null;

        protected T Request<T>(long req, byte[] bytes)
            where T : class, new()
        {
            return Request<T>(req, bytes, _replyTimeout, _responseTimeout, _sendTimes);
        }

        protected T Request<T>(long req, byte[] bytes, int replytimeout)
           where T : class, new()
        {
            return Request<T>(req, bytes, replytimeout, _responseTimeout, _sendTimes);
        }

        protected T Request<T>(long req, byte[] bytes, int replytimeout, int responsetimeout, int sendtimes)
            where T : class, new()
        {
            lock (_currentHelper)
            {
                try
                {
                    _currentTxContent = new TxContent(TxMode_t.Request, req, bytes, replytimeout, responsetimeout, sendtimes);
                    if (_currentTxContent.Send(SendBytes))
                    {
                        _waitcurrentResponse = new Semaphore(0, 1);
                        _waitcurrentReply = new Semaphore(0, 1);

                        //Wait response;
                        _waitcurrentResponse.WaitOne(_responseTimeout * sendtimes);

                        //wait reply
                        if (!_currentTxContent.IsCompeleted()) _waitcurrentReply.WaitOne(_replyTimeout * sendtimes);

                        return _currentTxContent.GetReply<T>();
                    }
                    else
                    {
                        return null;
                    }
                }
                catch
                {
                    return null;
                }
            }
        }

        protected T RequestWithoutReply<T>(long req, byte[] bytes)
            where T : class, new()
        {
            return RequestWithoutReply<T>(req, bytes, _responseTimeout, _sendTimes);
        }

        protected T RequestWithoutReply<T>(long req, byte[] bytes, int responsetimeout, int sendtimes)
           where T : class, new()
        {
            lock (_currentHelper)
            {
                try
                {
                    _currentTxContent = new TxContent(TxMode_t.RequestWithoutReply, req, bytes, _replyTimeout, responsetimeout, sendtimes);
                    if (_currentTxContent.Send(SendBytes))
                    {
                        _waitcurrentResponse = new Semaphore(0, 1);

                        //Wait response;
                        _waitcurrentResponse.WaitOne(_responseTimeout * sendtimes);

                        return _currentTxContent.GetResponse<T>();
                    }
                    else
                    {
                        return null;
                    }
                }
                catch
                {
                    return null;
                }
            }
        }

        protected T Reply<T>(long req, byte[] bytes)
            where T : class, new()
        {
            return Reply<T>(req, bytes, _responseTimeout, _sendTimes);
        }

        protected T Reply<T>(long req, byte[] bytes, int responsetimeout, int sendtimes)
           where T : class, new()
        {
            lock (_currentHelper)
            {
                try
                {
                    _currentTxContent = new TxContent(TxMode_t.Reply, req, bytes, _replyTimeout, responsetimeout, sendtimes);
                    if (_currentTxContent.Send(SendBytes))
                    {
                        _waitcurrentResponse = new Semaphore(0, 1);

                        //Wait response;
                        _waitcurrentResponse.WaitOne(_responseTimeout * sendtimes);

                        return _currentTxContent.GetResponse<T>();
                    }
                    else
                    {
                        return null;
                    }
                }
                catch
                {
                    return null;
                }
            }
        }

        protected T Broadcast<T>(long req, byte[] bytes)
            where T : class, new()
        {
            return Broadcast<T>(req, bytes, _responseTimeout, _sendTimes);
        }

        protected T Broadcast<T>(long req, byte[] bytes, int responsetimeout, int sendtimes)
           where T : class, new()
        {
            lock (_currentHelper)
            {
                try
                {
                    _currentTxContent = new TxContent(TxMode_t.Broadcast, req, bytes, _replyTimeout, responsetimeout, sendtimes);
                    if (_currentTxContent.Send(SendBytes))
                    {
                        _waitcurrentResponse = new Semaphore(0, 1);

                        //Wait response;
                        _waitcurrentResponse.WaitOne(_responseTimeout * sendtimes);

                        return _currentTxContent.GetResponse<T>();
                    }
                    else
                    {
                        return null;
                    }
                }
                catch
                {
                    return null;
                }
            }
        }

        private Dictionary<long, TxContent> _sentList = new Dictionary<long, TxContent>();
        private readonly object _listHelper = new object();


        private bool SendContent(TxMode_t mode, long req, byte[] bytes, int replytimeout, int responsetimeout, int sendtimes)
        {
            lock (_listHelper)
            {
                try
                {
                    TxContent _txcontent = new TxContent(mode, req, bytes, replytimeout, responsetimeout, sendtimes);
                    if (_txcontent.Send(SendBytes))
                    {
                        if (!_sentList.ContainsKey(req))
                        {
                            _sentList.Add(req, _txcontent);
                            return true;
                        }
                    }
                }
                catch
                {

                }
                return false;
            }
        }

        protected bool Request(long req, byte[] bytes)
        {
            return Request(req, bytes, _replyTimeout, _responseTimeout, _sendTimes);
        }

        protected bool Request(long req, byte[] bytes, int replytimeout)
        {
            return Request(req, bytes, replytimeout, _responseTimeout, _sendTimes);
        }
        protected bool Request(long req, byte[] bytes, int replytimeout, int responsetimeout, int sendtimes)
        {
            return SendContent(TxMode_t.Request, req, bytes, replytimeout, responsetimeout, sendtimes);
        }

        protected bool RequestWithoutReply(long req, byte[] bytes)
        {
            return RequestWithoutReply(req, bytes, _responseTimeout, _sendTimes);
        }

        protected bool RequestWithoutReply(long req, byte[] bytes, int responsetimeout, int sendtimes)
        {
            return SendContent(TxMode_t.RequestWithoutReply, req, bytes, _replyTimeout, responsetimeout, sendtimes);
        }

        protected bool Reply(long req, byte[] bytes)
        {
            return Reply(req, bytes, _responseTimeout, _sendTimes);
        }

        protected bool Reply(long req, byte[] bytes, int responsetimeout, int sendtimes)
        {
            return SendContent(TxMode_t.Reply, req, bytes, _replyTimeout, responsetimeout, sendtimes);
        }

        protected bool Broadcast(long req, byte[] bytes)
        {
            return Broadcast(req, bytes, _responseTimeout, _sendTimes);
        }

        protected bool Broadcast(long req, byte[] bytes, int responsetimeout, int sendtimes)
        {
            return SendContent(TxMode_t.Broadcast, req, bytes, _replyTimeout, responsetimeout, sendtimes);
        }

        protected bool Response(long req, byte[] bytes)
        {
            return SendContent(TxMode_t.Response, req, bytes, _replyTimeout, _responseTimeout, _sendTimes);
        }


        protected void CheckProcess()
        {
            while (true)
            {
                try
                {
                    CheckReceive();
                    Thread.Sleep(100);
                }
                catch
                {
                    continue;
                }
            }
        }

        private void CheckReceive()
        {
            //execute peer 100ms

            if (_currentTxContent != null)
            {
                if (_currentTxContent.IsTimeout())
                {
                    if (!_currentTxContent.IsCompeleted() && !_currentTxContent.Resend(SendBytes))
                    {
                        try
                        {
                            if (_currentTxContent.State == Status_t.WaitResponseTimeout) _waitcurrentResponse.Release();
                        }
                        catch
                        {

                        }

                        try
                        {
                            if (_currentTxContent.State == Status_t.WaitReplyTimeout) _waitcurrentReply.Release();
                        }
                        catch
                        {

                        }

                        if (_currentTxContent.State == Status_t.WaitResponseTimeout && WaitResponseTimeout != null) WaitResponseTimeout(_currentTxContent.Sequence);
                        else if (_currentTxContent.State == Status_t.WaitReplyTimeout && WaitReplyTimeout != null) WaitReplyTimeout(_currentTxContent.Sequence);


                    }

                }
            }


            lock (_listHelper)
            {
                foreach (var txcontent in _sentList)
                {
                    if (txcontent.Value != null)
                    {
                        if (txcontent.Value.IsTimeout())
                        {
                            if (!txcontent.Value.IsCompeleted() && !txcontent.Value.Resend(SendBytes))
                            {
                                if (txcontent.Value.State == Status_t.WaitResponseTimeout && WaitResponseTimeout != null) WaitResponseTimeout(txcontent.Value.Sequence);
                                else if (txcontent.Value.State == Status_t.WaitReplyTimeout && WaitReplyTimeout != null) WaitReplyTimeout(txcontent.Value.Sequence);

                            }
                        }
                    }
                }
            }
        }


        private enum Status_t
        {
            Sending,
            WaitResponse,
            WaitReply,
            Completed,
            WaitResponseTimeout,
            WaitReplyTimeout,
        }

        private enum TxMode_t
        {
            Request,
            RequestWithoutReply,
            Reply,
            Broadcast,
            Response,
        }

        private class TxContent
        {
            public long Sequence;

            private TxMode_t _mode;
            private byte[] _request;
            private object _response;
            private object _reply;

            private int _responseTimeout;
            private int _replyTimeout;

            private int _rmainingSendTimes;
            private long _timeoutTicks;
            public Status_t State;

            public TxContent(TxMode_t mode, long seq, byte[] tx, int replytimeout, int responsetimeout, int sendtimes)
            {
                _mode = mode;
                _request = tx;
                Sequence = seq;
                _replyTimeout = replytimeout;
                _responseTimeout = responsetimeout;
                _rmainingSendTimes = sendtimes;
            }


            public byte[] GetRequest() { return _request; }
            public T GetResponse<T>() where T : class { return _response as T; }
            public T GetReply<T>() where T : class { return _reply as T; }
            public bool Send(Func<byte[], bool> action)
            {
                State = Status_t.Sending;

                bool result = action(_request);
                if (result)
                {
                    InitializeCheckTimeout();
                }
                return result;
            }

            public bool Resend(Func<byte[], bool> action)
            {
                if (_rmainingSendTimes <= 0)
                {
                    if (State == Status_t.WaitResponse) State = Status_t.WaitResponseTimeout;
                    else if (State == Status_t.WaitReply) State = Status_t.WaitReplyTimeout;
                    return false;
                }
                Send(action);
                return true;
            }



            public void InitializeCheckTimeout()
            {
                if (_mode == TxMode_t.Response)
                {
                    State = Status_t.Completed;
                }
                else
                {
                    _rmainingSendTimes--;
                    _timeoutTicks = DateTime.Now.AddMilliseconds(_responseTimeout).Ticks;
                    State = Status_t.WaitResponse;
                }
            }

            public TxContent SetResponse(object res)
            {
                _response = res;

                if (_mode == TxMode_t.Request)
                {
                    _rmainingSendTimes--;
                    _timeoutTicks = DateTime.Now.AddMilliseconds(_replyTimeout).Ticks;
                    State = Status_t.WaitReply;
                }
                else
                {
                    State = Status_t.Completed;
                }

                return this;
            }

            public TxContent SetReply(object res)
            {
                _reply = res;
                State = Status_t.Completed;
                return this;
            }

            public bool IsCompeleted()
            {
                return State == Status_t.Completed || State == Status_t.WaitResponseTimeout || State == Status_t.WaitReplyTimeout;
            }

            public bool IsTimeout()
            {
                return DateTime.Now.Ticks > _timeoutTicks;
            }
        }
    }
}