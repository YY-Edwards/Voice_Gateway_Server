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
    public class Staff : RElement
    {
        public enum StaffType
        {
            Staff = 0,
            Vehicle = 1,
        };

        [JsonProperty(PropertyName = "name")]
        public string Name { get; set; }

        [JsonProperty(PropertyName = "type")]
        public StaffType Type
        {
            get
            {
                return IsValid ? StaffType.Staff : StaffType.Vehicle;
            }
            set
            {
                IsValid = value == StaffType.Staff ? true : false;
            }
        }

        [JsonProperty(PropertyName = "phone")]
        public string PhoneNumber { get; set; }

        [JsonProperty(PropertyName = "valid")]
        public int Valid;


        [JsonIgnore]
        public bool IsValid
        {
            set { Valid = value ? 1 : 0; }
            get { return Valid == 0 ? false : true; }
        }

        [JsonProperty(PropertyName = "department")]
        public long DepartmentID{get;set;}


        public Staff()
        {
            Type = StaffType.Staff;
            Name = string.Empty;
            PhoneNumber = string.Empty;
            DepartmentID = 0;
        }
    }



    
}
