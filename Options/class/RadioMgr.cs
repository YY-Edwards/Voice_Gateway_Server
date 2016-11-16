using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

namespace TrboX
{
    public enum RadioType
    {
        Radio,
        Ride
    };

    public class Radio
    {
        [DefaultValue((long)0), JsonProperty(PropertyName = "id")]
        public int ID;
        [JsonProperty(PropertyName = "radio_id")]
        public long RadioID { set; get; }
        [JsonProperty(PropertyName = "type")]
        public RadioType Type { set; get; }

        [JsonProperty(PropertyName = "screen")]
        public bool HasScreen;

        [JsonProperty(PropertyName = "gps")]
        public bool HasGPS;

        [JsonProperty(PropertyName = "keyboard")]
        public bool HasKeyboard;

        [JsonProperty(PropertyName = "sn")]
        public string SN;

        [JsonProperty(PropertyName = "valid")]
        public bool IsValid;

         [JsonIgnore]
        public bool IsOnline { set; get; }

         [JsonIgnore]
        public bool IsGPS { set; get; }
         [JsonIgnore]
        public bool IsTx { set; get; }

         [JsonIgnore]
        public bool IsRx { set; get; }


         [JsonIgnore]
        public string Name
        {
            get
            { return (Type == RadioType.Radio ? "手持台" : "车载台") + "(ID:" + RadioID.ToString() + ")"; }
        }

        public Radio()
        { }

        public long Add()
        {
            return RadioMgr.Add(this);
        }

        public void Modify()
        {
            RadioMgr.Modify(ID, this);
        }

        public void Delete()
        {
            RadioMgr.Delete(ID);
        }

    }

    public class UpdatesRadio
    {
        public long id;
        public Radio radio;
    }
    class RadioMgr
    {
        private static long OrginIndex = 0;
        private static long CurrentIndex = 0;

        private static Dictionary<long, Radio> s_Add = new Dictionary<long, Radio>();
        private static List<long> s_Del = new List<long>();
        private static List<UpdatesRadio> s_Update = new List<UpdatesRadio>();


        public static int Count()
        {
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.count.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = -1,
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.department.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return 0;
            }

            try
            {
                return (int)LogServer.Call(str, ParseCount);
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

        public static object ParseCount(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, int> Dic = JsonConvert.DeserializeObject<Dictionary<string, int>>(JsonConvert.SerializeObject(rep.contents));
                return Dic["count"];
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return 0;
            }
        }

        public static List<Radio> List()
        {
            int count = Count();
            if (count <= 1) return null;
            Dictionary<string, object> param = new Dictionary<string, object>();

            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = count
            });

            LogServerRequest req = new LogServerRequest()
            {
                call = RequestType.department.ToString(),
                callId = LogServer.CallId,
                param = param
            };

            string str = "";
            try
            {
                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                str = JsonConvert.SerializeObject(req, Formatting.Indented, jsetting);
            }
            catch (Exception e)
            {
                DataBase.InsertLog("Build Josn Error" + e.Message);
                return null;
            }


            try
            {

                List<Radio> s_List = LogServer.Call(str, ParseList) as List<Radio>;

                if (s_List == null) return null;
                OrginIndex = s_List.Select(w => w.ID).Max();
                CurrentIndex = OrginIndex;

                s_Add.Clear();
                s_Del.Clear();
                s_Update.Clear();

                return s_List;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }
        public static object ParseList(object obj)
        {
            try
            {
                if (obj == null) return null;
                LogServerResponse rep = obj as LogServerResponse;
                Dictionary<string, List<Radio>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<Radio>>>(JsonConvert.SerializeObject(rep.contents));
                List<Radio> res = Dic["records"];

                //for (int i = 0; i < res.Count; i++)
                //{
                //    res[i].Auth = res[i].parseauth();
                //}

                return res;
            }
            catch (Exception e)
            {
                DataBase.InsertLog(e.Message);
                return null;
            }
        }

        public static long Add(Radio radio)
        {
            radio.ID = 0;
            s_Add.Add(++CurrentIndex, radio);
            return CurrentIndex;
        }

        public static void Delete(long Id)
        {
            try
            {

                if (Id > OrginIndex)
                {
                    s_Add.Remove(Id);
                }
                else
                {
                    if (!s_Del.Contains(Id)) s_Del.Add(Id);
                }
            }
            catch
            {

            }
        }

        public static void Modify(long Id, Radio radio)
        {
            try
            {
                if (Id > OrginIndex)
                {
                    radio.ID = 0;
                    s_Add[Id] = radio;
                }
                else
                {
                    radio.ID = 0;
                    s_Update.Add(new UpdatesRadio() { id = Id, radio = radio });
                }
            }
            catch
            {

            }
        }

        public static void Save()
        {
            if (s_Del.Count > 0)
            {
                Dictionary<string, object> delparam = new Dictionary<string, object>();
                delparam.Add("operation", OperateType.del.ToString());
                delparam.Add("radios", s_Del);

                LogServerRequest delreq = new LogServerRequest()
                {
                    call = RequestType.radio.ToString(),
                    callId = LogServer.CallId,
                    param = delparam
                };
                string delstr = JsonConvert.SerializeObject(delreq);
                LogServer.Call(delstr);
            }

            if (s_Update.Count > 0)
            {
                Dictionary<string, object> updateparam = new Dictionary<string, object>();
                updateparam.Add("operation", OperateType.update.ToString());
                updateparam.Add("radios", s_Update);

                LogServerRequest updatereq = new LogServerRequest()
                {
                    call = RequestType.radio.ToString(),
                    callId = LogServer.CallId,
                    param = updateparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string updatestr = JsonConvert.SerializeObject(updatereq, Formatting.Indented, jsetting);

                LogServer.Call(updatestr);
            }


            if (s_Add.Count > 0)
            {
                List<Radio> addlist = new List<Radio>();
                foreach (var item in s_Add)
                {
                    addlist.Add(item.Value);
                }

                Dictionary<string, object> addparam = new Dictionary<string, object>();
                addparam.Add("operation", OperateType.add.ToString());
                addparam.Add("radios", addlist);

                LogServerRequest addreq = new LogServerRequest()
                {
                    call = RequestType.radio.ToString(),
                    callId = LogServer.CallId,
                    param = addparam
                };

                JsonSerializerSettings jsetting = new JsonSerializerSettings();
                jsetting.DefaultValueHandling = DefaultValueHandling.Ignore;
                string addstr = JsonConvert.SerializeObject(addreq, Formatting.Indented, jsetting);

                LogServer.Call(addstr);
            }


            OrginIndex = CurrentIndex;
            s_Add.Clear();
            s_Del.Clear();
            s_Update.Clear();
        }
    }
}
