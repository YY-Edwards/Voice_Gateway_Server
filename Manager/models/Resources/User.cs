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

using Sigmar.Extension;

namespace Manager.Models
{
    public class User : RElement
    {
        public enum UserType
        {
            admin,
            guest
        }

        [Serializable]
        public class CAuthority
        {
            public string Type;
            public string Dept { set; get; }
            public string Icon { set; get; }
        }


        [JsonProperty(PropertyName = "username")]
        public string UserName { set; get; }

        [DefaultValue(""), JsonProperty(PropertyName = "password")]
        public string Password { set; get; }

        [JsonProperty(PropertyName = "type")]
        public string TypeStr { set; get; }

        [JsonProperty(PropertyName = "authority")]
        public string Func { set; get; }


        [JsonIgnore]
        public UserType Type
        {
            get { return TypeStr.ToEnum<UserType>(); }
            set { TypeStr = value.ToString(); }            
        }

        [JsonIgnore]
        public List<CAuthority> Authority { set; get; }
       
        public User()
        {
            Type = UserType.guest;
            UserName = string.Empty;
            Password = string.Empty;
            Func = string.Empty;
        }
    }
}
