using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;
using Dispatcher.Service;

using Dispatcher;

namespace Dispatcher.Modules
{
    public class CResource
    {
        private Semaphore m_WaitReponse;

        [JsonIgnore]
        public bool IsNew = true;

        [JsonIgnore]
        public List<CElement> List = new List<CElement>();
        private bool m_IsUpdated = false;


        private int m_NewID = 0;
        private List<CElement> m_Add;
        private List<long> m_Del;
        private List<CElement> m_Update;


        public List<CElement> AddList { get { return m_Add; } }
        public List<long> DelList { get { return m_Del; } }
        public List<CElement> UpdateList { get { return m_Update; } }

        private RequestOpcode m_Opcode;

        Critera m_QueryCritera;

        public delegate void ResourceChangedHandle(RequestOpcode type, List<CElement> res);
        public event ResourceChangedHandle OnResourceChanged;

        public event EventHandler Timeout;
       
      
        public CResource(RequestOpcode op)
        {
            m_Opcode = op;
            if (m_WaitReponse == null) m_WaitReponse = new Semaphore(0, 1);

            CLogServer.Instance().Timeout += new EventHandler(OnTimeout);
        }
        private void OnTimeout(object sender, EventArgs e)
        {
            if (Timeout != null) Timeout(sender, e);
        }

        public void Request(RequestOpcode opcode, OperateType type, object parameter, object attach = null)
        {
            if (!CLogServer.Instance().IsInitialized) return;
            try
            {
                 string[] reply = CLogServer.Instance().Request(opcode, parameter);

                if (reply != null && reply.Length >= 2)
                {
                    switch (type)
                    {
                        case OperateType.list:
                            if (reply[0] == "success")
                            {
                                List.AddRange(ParseList(reply[1]));
                                List = List.OrderBy(p => p.ID).ToList<CElement>();
                                if (OnResourceChanged != null) OnResourceChanged(opcode, List);
                                ListUpdate();
                                m_IsUpdated = true;
                            }
                            break;
                        case OperateType.count:
                            if (reply[0] == "success")
                            {
                                int count = ParseCount(reply[1]);
                                if (count > 0) OnCountUpdate(count, parameter as Dictionary<string, object>);
                                else
                                {
                                    if (OnResourceChanged != null) OnResourceChanged(opcode, List);
                                }
                            }
                            break;
                        default:
                            CustomTypeReply(type, reply[0] == "success", reply[1], attach);
                            break;
                    }
                }
            }
            catch
            {

            }

        }


        public virtual void ListUpdate()
        {

        }
        public virtual void CustomTypeReply(OperateType type, bool issuccess, string reply, object attach)
        {

        }

        public virtual void CustomQuery()
        {

        }

        private int ParseCount(string json)
        {
            try
            {
                if (json == null || json == string.Empty) return 0;
                Dictionary<string, int> Dic = JsonConvert.DeserializeObject<Dictionary<string, int>>(json);
                return Dic["count"];
            }
            catch (Exception e)
            {
                return 0;
            }
        }


        private List<CElement> ParseList(string json)
        {
            try
            {
                List<CElement> querylist = new List<CElement>();

                if (json == null || json ==string.Empty) return null;

                Dictionary<string, List<JObject>> Dic = JsonConvert.DeserializeObject<Dictionary<string, List<JObject>>>(json);
                if(Dic.ContainsKey("records"))
                {
                    
                    foreach(JObject obj in Dic["records"])
                    {
                        if (obj == null) continue;

                        CElement res = Parse(JsonConvert.SerializeObject(obj));
                        if (res != null)
                        {
                            querylist.Add(res);
                        }
                        
                    }
                }
                return querylist;
            }
            catch
            {
                return null;
            }
        }


        public virtual CElement Parse(string json)
        {
            try
            {
                return JsonConvert.DeserializeObject<CElement>(json);
            }
            catch
            {
                return null;
            }            
        }
    
        private void  count()
        {
            //new Thread(new ThreadStart(delegate()
            {
                Dictionary<string, object> param = new Dictionary<string, object>();
                param.Add("operation", OperateType.count.ToString());
                param.Add("critera", new Critera()
                {
                    offset = 0,
                    count = 1,
                });
          
                Request(m_Opcode,OperateType.count, param);
            }
            //)).Start();          
        }


        public virtual List<string[]> Condition()
        {
            return null;
        }

        public void Query()
        {
            Query(0, -1);
        }

        public void Query(int offset, int length)
        {
            List.Clear();

            m_QueryCritera = new Critera()
            {
                offset = offset,
                count = length,
                condition = Condition()
            };

            count();
        }

        private void OnCountUpdate(int count, Dictionary<string, object> param)
        {
            if (m_QueryCritera == null)
            {
                m_QueryCritera = new Critera() 
                { 
                    offset = 0,
                    count = -1
                };
            }
            if (m_QueryCritera.count< 0 || m_QueryCritera.count > count) m_QueryCritera.count = count;


            //new Thread(new ThreadStart(delegate()
            {
                int i = 0;
                for (i = 0; i < m_QueryCritera.count / 10; i++)
                {
                    Critera cri = new Critera();
                    cri.offset = i * 10;
                    cri.count = 10;
                    cri.sort = m_QueryCritera.sort;
                    cri.condition = m_QueryCritera.condition;

                    query(cri);
                }

                if (m_QueryCritera.count % 10 > 0)
                {

                    Critera cri = new Critera();
                    cri.offset = i * 10;
                    cri.count = m_QueryCritera.count % 10;
                    cri.sort = m_QueryCritera.sort;
                    cri.condition = m_QueryCritera.condition;
                    query(cri);
                }

                CustomQuery();
            }
            //)).Start(); 
        }

        public void Add(CElement obj)
        {
            if (m_Add == null) m_Add = new List<CElement>();

            obj.ID= --m_NewID;
            m_Add.Add(obj);
            List.Add(obj);
        }

        public void Modify(long id, CElement obj)
        {
            if (id < 0)
            {
                if (m_Add == null) return;
                int index = m_Add.FindIndex(p => p.ID == id);
                if (index >= 0 && index < m_Add.Count)
                {
                    m_Add.RemoveAt(index);
                    m_Add.Insert(index, (CElement)obj);
                }
            }
            else
            {
                if (m_Update == null) m_Update = new List<CElement>();

                int index = m_Update.FindIndex(p => p.ID == id);
                if (index >= 0 && index < m_Update.Count)
                {
                    m_Update.RemoveAt(index);                   
                }

                m_Update.Add(obj);
            }


            int idx = List.FindIndex(p => p.ID == id);
            if (idx >= 0 && idx < List.Count)
            {
                List.RemoveAt(idx);
                List.Insert(idx, obj);
            }
        }

        public void Delete(long id)
        {

            if(id < 0)
            {
                if (m_Add == null) return;
                int index = m_Add.FindIndex(p=>p.ID == id);
                if(index >= 0 && index < m_Add.Count )
                {
                    m_Add.RemoveAt(index);
                }
            }
            else
            {
                if (m_Del == null) m_Del = new List<long>();
                m_Del.Add(id);
            }

            int idx = List.FindIndex(p => p.ID == id);
            if (idx >= 0 && idx < List.Count)
            {
                List.RemoveAt(idx);
            }
        }

        private void query(Critera cri)
        {
            if (cri == null || cri.count <= 0) return;

            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", cri); 
         
            Request(m_Opcode,OperateType.list, param);
        }
    }
    public enum OperateType
    {
        
        add,
        list,
        count,
        del,
        update,
        auth,

        listUser,
        assignUser,
        detachUser,

        listRadio,
        assignRadio,
        detachRadio,
    }

    public class Critera
    {
        [DefaultValue(null)]
        public List<string[]> condition;

        [DefaultValue(null)]
        public List<string> sort;

        [DefaultValue(-1)]
        public int offset;

        [DefaultValue(-1)]
        public int count;
    }

}
