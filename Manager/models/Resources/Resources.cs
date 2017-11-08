using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.ComponentModel;

using System.Threading;

namespace Manager.Models
{
    public class ResourcesManage
    { 
        public event Action<object, object> ResourceChanged;
        public event Action<object> Timeout;
        private LogServer _LogServer{get{return LogServer.Instance();}}

        public RequestOpcode ResourceOpcode;
        public string RecordsName;

        public ResourcesManage()
        {
            ResourceOpcode = RequestOpcode.None;
            RecordsName = string.Empty;

            _LogServer.Timeout += new Action<object, Server.Request>(OnLogServerTimeout);
        }

        private void OnLogServerTimeout(object sender, Server.Request request)
        {
            if (Timeout != null) Timeout(this);
        }

        public List<T> Read<T>(int offset, int count, List<string[]> condition) where T : class, new()
        {
            List<T> elements = ReadRElements<T>(offset, count, condition);
            if(elements != null)
            {
               if(ResourceChanged != null)ResourceChanged(this, elements);
            }
             
            return elements;
        }
        public List<T> Read<T>() where T : class, new()
        {
            List<T> elements = ReadRElements<T>(0, ReadCount(), null);
            if(elements != null)
            {
               if(ResourceChanged != null)ResourceChanged(this, elements);
            }
             
            return elements; 
        }

        public bool Save()
        {        
            Delete();
            Update();
            Insert();

            return true;
        }

        private int ReadCount()
        {
            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.count.ToString());
            param.Add("critera", new Critera()
            {
                offset = 0,
                count = 1,
            });
          
            Server.Response reponse = _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);

            if (reponse != null && reponse.status.ToLower() == "success")
            {
                ElementCount_t countInfo = null;
                try
                {
                     string contents = JsonConvert.SerializeObject(reponse.contents);
                     countInfo = JsonConvert.DeserializeObject<ElementCount_t>(contents);
                }
                catch
                {

                }

                if(countInfo != null)return countInfo.Count;
            }

            return 0;
        }

        private List<T> ReadRElements<T>(int offset, int count, List<string[]> condition) where T : class, new()
        {
            if(count <= 0)return null;
            int elementFrame = 0;
            int frameLength = 10;

            List<T> elements = new List<T>();

            for (; elementFrame < count / 10; elementFrame++)
            {
                Critera critera = new Critera();
                critera.offset = offset +  elementFrame * 10;
                critera.count = frameLength;
                critera.condition = condition;

                List<T> result = Query<T>(critera);
                if(result != null)elements.AddRange(result);               
            }

            if (count % 10 > 0)
            {
                Critera critera = new Critera();
                critera.offset = offset +  elementFrame * 10;
                critera.count = count % frameLength;
                critera.condition = condition;

                List<T> result = Query<T>(critera);
                if(result != null)elements.AddRange(result);
            }



            return elements;
        }


        private List<T> Query<T>(Critera critera) where T : class, new()
        {
            if (critera == null || critera.count <= 0) return null;

            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.list.ToString());
            param.Add("critera", critera); 
         
            Server.Response reponse = _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);          
            if (reponse.status.ToLower() == "success")
            {
                Dictionary<string, List<JObject>> records = null;
                try
                {
                    string contents = JsonConvert.SerializeObject(reponse.contents);
                    records = JsonConvert.DeserializeObject<Dictionary<string, List<JObject>>>(contents);
                }
                catch
                {

                }

                if (records == null || !records.ContainsKey("records")) return null;
                

                List<T> elements = new List<T>();
                foreach (var record in records["records"])
                {
                    T element = null;
                    try
                    {
                        string elementJson = JsonConvert.SerializeObject(record);
                        element = JsonConvert.DeserializeObject<T>(elementJson);
                    }
                    catch
                    {

                    }

                    if (element != null)
                    {
                        elements.Add(element);
                    }

                }

                return elements;
            }


            return null;
        }

        private List<long> _DeleteElements;
        private List<RElement> _UpdateElements;
        private List<RElement> _InsertElements;
       
        public void Delete(RElement element)
        {
            if (_InsertElements != null)
            {
                int index = _InsertElements.FindIndex(p => p.ID == element.ID);
                if (index >= 0 && index < _InsertElements.Count)
                {
                    _InsertElements.RemoveAt(index);
                    return;
                }
            }

            if (_DeleteElements == null) _DeleteElements = new List<long>();
            _DeleteElements.Add(element.ID);
        }

        private void Delete()
        {
            if(_DeleteElements == null || _DeleteElements.Count <= 0)return;

            int elementFrame = 0;
            int frameLength = 10;

            for (; elementFrame < _DeleteElements.Count / 10; elementFrame++)
            {
                Delete(_DeleteElements.Skip(elementFrame * 10).Take(frameLength).ToList<long>());                  
            }

            if (_DeleteElements.Count % 10 > 0)
            {
                Delete(_DeleteElements.Skip(elementFrame * 10).ToList<long>());    
            }

            _DeleteElements.Clear();
        }

        private void Delete(List<long> elements)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.del.ToString());
            param.Add(RecordsName, elements);
   
            _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);        
        }
       
        public void Update(RElement element)
        {
            if (_InsertElements != null)
            {
                int index = _InsertElements.FindIndex(p => p.ID == element.ID);
                if (index >= 0 && index < _InsertElements.Count)
                {
                    _InsertElements[index] = element;
                    return;
                }
            }

            if (_UpdateElements == null) _UpdateElements = new List<RElement>();

            int updadeIndex = _UpdateElements.FindIndex(p => p.ID == element.ID);
            if (updadeIndex >= 0 && updadeIndex < _UpdateElements.Count)
            {
                _UpdateElements[updadeIndex] = element;
            }
            else
            {
                _UpdateElements.Add(element);
            }
        }

        private void Update()
        {
            if(_UpdateElements == null || _UpdateElements.Count <= 0)return;

            int elementFrame = 0;
            int frameLength = 10;

            for (; elementFrame < _UpdateElements.Count / 10; elementFrame++)
            {
                Update(_UpdateElements.Skip(elementFrame * 10).Take(frameLength).ToList<RElement>());                  
            }

            if (_UpdateElements.Count % 10 > 0)
            {
                Update(_UpdateElements.Skip(elementFrame * 10).ToList<RElement>());    
            }

            _UpdateElements.Clear();
        }

        private void Update(List<RElement> elements)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.update.ToString());

            List<Dictionary<string, object>> updates = new List<Dictionary<string, object>>();

            foreach (var element in elements)
            {
                Dictionary<string, object> item = new Dictionary<string, object>();
                item.Add("id", element.ID);
                item.Add(ResourceOpcode.ToString(), element);
                updates.Add(item);
            }

            param.Add(RecordsName, updates);

           _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);        
        }


        public void Insert(RElement element)
        {
            if (_InsertElements == null) _InsertElements = new List<RElement>();
            _InsertElements.Add(element);           
        }

        private void Insert()
        {
            if(_InsertElements == null || _InsertElements.Count <= 0)return;

            int elementFrame = 0;
            int frameLength = 10;

            for (; elementFrame < _InsertElements.Count / 10; elementFrame++)
            {
                Insert(_InsertElements.Skip(elementFrame * 10).Take(frameLength).ToList<RElement>());                  
            }

            if (_InsertElements.Count % 10 > 0)
            {
                Insert(_InsertElements.Skip(elementFrame * 10).ToList<RElement>());    
            }

            _InsertElements.Clear();
        }


        private void Insert(List<RElement> elements)
        {
            Dictionary<string, object> param = new Dictionary<string, object>();
            param.Add("operation", OperateType.add.ToString());
            param.Add(RecordsName, elements);


            //List<Dictionary<string, object>> updates = new List<Dictionary<string, object>>();

            //foreach (var element in elements)
            //{
            //    Dictionary<string, object> item = new Dictionary<string, object>();
            //    item.Add("id", element.ID);
            //    item.Add(RecordsName, element);
            //    updates.Add(item);
            //}

            //param.Add(RecordsName, updates);

           _LogServer.SendRequest<Server.Response>(ResourceOpcode, RequestType.radio, param);        
        }

        public enum OperateType
        {
            add,
            list,
            count,
            del,
            update,

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
    
        public class ElementCount_t
        {
            [DefaultValue((long)0), JsonProperty(PropertyName = "count")]
            public int Count{get;set;}
        }
    }
}
