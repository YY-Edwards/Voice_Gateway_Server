using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using Dispatcher.Service;

namespace Dispatcher.Modules
{
    public class CUserMgr : CResource
    {

        private volatile static CUserMgr _instance = null;
        private static readonly object lockHelper = new object();

        public static CUserMgr Instance()
        {
            if (_instance == null)
            {
                lock (lockHelper)
                {
                    if (_instance == null)
                        _instance = new CUserMgr();
                }
            }
            return _instance;
        }


        public event LoginResultHandler OnLoginResult;

        public CUserMgr():base
        (RequestOpcode.user)
        {
            List.Add(new CUser() { UserName = "test"});
        }

        public override CElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CUser>(json);
            }
            catch
            {
                return null;
            }
        }


        public override void CustomTypeReply(OperateType type, bool issuccess, string reply, object attach)
        {
            if (type == OperateType.auth && OnLoginResult != null) OnLoginResult(new LoginResultArgs(issuccess));
        }
        public void Auth(string user, string password)
        {
            Dictionary<string, object> authparam = new Dictionary<string, object>();
            authparam.Add("operation", OperateType.auth.ToString());

            Dictionary<string, string> authuserparam = new Dictionary<string, string>();
            authuserparam.Add("username", user);
            authuserparam.Add("password", password);

            authparam.Add("users", authuserparam);

            Request(RequestOpcode.user, OperateType.auth, authparam);
        }
    }


    public enum UserType
    {
        Admin,
        Guest
    }

    [Serializable]
    public class CAuthority
    {
        public string Type;
        public string Dept { set; get; }
        public string Icon{ set; get; }
    }

    [Serializable]
    public class CUser : CElement
    {
        [JsonProperty(PropertyName = "username")]
        public string UserName { set; get; }

        [DefaultValue(""), JsonProperty(PropertyName = "password")]
        public string Password;

     
        [JsonIgnore]
        public UserType Type { set; get; }

        [JsonIgnore]
        public List<CAuthority> Authority { set; get; }


        [JsonProperty(PropertyName = "type")]
        public string TypeStr { set { Type = (UserType)Enum.Parse(typeof(UserType), value); } get { return Type.ToString(); } }

        [JsonProperty(PropertyName = "authority")]
        public string Func { set; get; }

        public CUser()
        {
            Type = UserType.Guest;
            Func = "";
        }

        public CUser Copy()
        {
            MemoryStream stream = new MemoryStream();
            new BinaryFormatter().Serialize(stream, this);
            stream.Seek(0, SeekOrigin.Begin);
            return (CUser)new BinaryFormatter().Deserialize(stream);
        }
    }
}
