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

namespace Manager
{
    public class CUserMgr : CResource
    {
        public CUserMgr():base
        (RequestOpcode.user)
        {
            List.Add(new CUser() { UserName = "test"});
        }

        public override CRElement Parse(string json)
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
    public class CUser : CRElement
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
