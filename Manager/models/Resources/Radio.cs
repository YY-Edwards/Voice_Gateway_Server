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
    public class Radio : RElement
    {
        public enum RadioType
        {
            Radio = 0,
            Ride = 1
        };


        [JsonProperty(PropertyName = "radio_id")]
        public long RadioID { set; get; }

        [JsonProperty(PropertyName = "type")]
        public int Typestr { set; get; }

        [JsonProperty(PropertyName = "screen")]
        public int Screen;

        [JsonProperty(PropertyName = "gps")]
        public int GPS;

        [JsonProperty(PropertyName = "keyboard")]
        public int Keyboard;


        [JsonProperty(PropertyName = "sn")]
        public string SN { set; get; }

        [JsonProperty(PropertyName = "valid")]
        public int Valid;

        [JsonProperty(PropertyName = "department")]
        public long DepartmentID { get; set; }

        [JsonProperty(PropertyName = "staff")]
        public long StaffID { get; set; }

        [JsonIgnore]
        public RadioType Type
        {
            set { Typestr = (int)value; }
            get 
            {
                try
                {
                    return (RadioType)Typestr;
                }
                catch               
                {
                    return RadioType.Radio;
                }
            }
        }
        

        [JsonIgnore]
        public bool HasScreen
        {
            set { Screen = value ? 1 : 0; }
            get { return Screen == 0 ? false : true; }
        }

        [JsonIgnore]
        public bool HasGPS
        {
            set
            {
                if (value) GPS |= 1;
                else GPS &= ~1;
            }
            get { return (GPS & 1) == 1 ? true : false ; }
        }

        [JsonIgnore]
        public bool HasLocationInDoor
        {
            set {
                if (value) GPS |= 2;
                else GPS &= ~2;           
            }
            get { return (GPS & 2) == 2 ? true : false; }
        }

       

        [JsonIgnore]
        public bool HasKeyboard
        {
            set { Keyboard = value ? 1 : 0; }
            get { return Keyboard == 0 ? false : true; }
        }

        
        [JsonIgnore]
        public bool IsValid
        {
            set { Valid = value ? 1 : 0; }
            get { return Valid == 0 ? false : true; }
        }

        public Radio()
        {
            Type = RadioType.Radio;
            RadioID = 0;
            SN = string.Empty;

            HasScreen = true;
            HasKeyboard = true;
            HasGPS = false;
            HasLocationInDoor = false;

            IsValid = true;
            DepartmentID = 0;
            StaffID = 0;
        }
    } 
}
