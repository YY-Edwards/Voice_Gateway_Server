using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

namespace Manager.Models
{
    public abstract class SaveReadableElement
    {
        public virtual SaveStatus Save()
        {
            SaveStatus result = SaveStatus.Skip;
            if (Elements != null && Elements.Count > 0)
            {
                foreach (SaveReadableElement element in Elements)
                {
                    SaveStatus res = element.Save();

                    if (result == SaveStatus.Skip) result = res;
                    else if (result == SaveStatus.Success && res == SaveStatus.Failure) result = SaveStatus.Failure;
                    else
                    {

                    }
                }
            }

            return result;
        }


        public virtual void Read()
        {
            if (Elements != null && Elements.Count > 0)
            {
                foreach (SaveReadableElement element in Elements)
                {
                    element.Read();
                }
            }     
        }

        public List<SaveReadableElement> Elements { get; private set; }
        public void InitializeElements(Type t)
        {
            PropertyInfo[] properties= t.GetProperties();
            if(properties == null || properties.Length == 0)
            {
                Elements = null;
                return;
            }

            Elements = new List<SaveReadableElement>();
            foreach(PropertyInfo property in properties)
            {
                object value = property.GetValue(this, null);

                if (value is SaveReadableElement)
                {
                    Elements.Add(value as SaveReadableElement);
                }
            }
        }      
    }

    public enum SaveStatus
    {
        Skip,
        Success,
        Failure,
    }
}
