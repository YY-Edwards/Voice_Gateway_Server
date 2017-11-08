using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.ComponentModel;
using Newtonsoft.Json;
using System.Reflection;

namespace Manager.Models
{
    public class RElement
    {
        [DefaultValue((long)0), JsonProperty(PropertyName = "id")]
        public long ID { set; get; }

        public RElement Copy()
        {
            Type t = this.GetType();
            var element = Activator.CreateInstance(t, true);
            CloneHelper.Copy(element, this, t);
            return element as RElement;
        }
    }


    public class CloneHelper
    {
        public static object Clone(object obj)
        {
            Type type = obj.GetType();
            object clone_obj = System.Activator.CreateInstance(type);
            Copy(clone_obj, obj, type);
            return clone_obj;
        }

        public static void Copy<T>(T dst, T src)
        {
            Copy(dst, src, typeof(T));
        }

        public static void Copy(object dst, object src, Type type)
        {
            if (type.IsGenericType)
            {
                if (type.GetInterface("IList") != null)
                {
                    IList list_obj = src as IList;
                    IList list_clone_obj = dst as IList;
                    foreach (object ele in list_obj)
                    {
                        Type ele_type = ele.GetType();
                        if (ele_type.IsPrimitive || ele_type.IsValueType || ele_type == typeof(String))
                        {
                            list_clone_obj.Add(ele);
                        }
                        else
                        {
                            list_clone_obj.Add(Clone(ele));
                        }
                    }
                    return;
                }
                else if (type.GetInterface("IDictionary") != null)
                {
                    IDictionary dict_obj = src as IDictionary;
                    IDictionary dict_clone_obj = dst as IDictionary;
                    foreach (object key in dict_obj.Keys)
                    {
                        object value = dict_obj[key];
                        Type key_type = key.GetType();
                        if (key_type.IsPrimitive || key_type.IsValueType || key_type == typeof(String))
                        {
                            if (key_type.IsPrimitive || key_type.IsValueType || key_type == typeof(String))
                            {
                                dict_clone_obj.Add(key, value);
                            }
                            else
                            {
                                dict_clone_obj.Add(key, Clone(value));
                            }
                        }
                        else
                        {
                            if (key_type.IsPrimitive || key_type.IsValueType || key_type == typeof(String))
                            {
                                dict_clone_obj.Add(Clone(key), value);
                            }
                            else
                            {
                                dict_clone_obj.Add(Clone(key), Clone(value));
                            }
                        }

                    }
                    return;
                }
            }

            PropertyInfo[] properties = type.GetProperties();
            foreach (PropertyInfo property in properties)
            {
                if (property.PropertyType.IsPrimitive || property.PropertyType.IsValueType || property.PropertyType == typeof(String))
                {
                    property.SetValue(dst, property.GetValue(src,null));
                }
                else
                {
                    object property_value = property.GetValue(src, null);
                    if (property_value != null)
                    {
                        property.SetValue(dst, Clone(property_value));
                    }
                }
            }


            FieldInfo[] fields = type.GetFields();
            foreach (FieldInfo field in fields)
            {
                if (field.IsStatic)
                {
                    continue;
                }
                if (field.FieldType.IsPrimitive || field.FieldType.IsValueType || field.FieldType == typeof(String))
                {
                    field.SetValue(dst, field.GetValue(src));
                }
                else
                {
                    object field_value = field.GetValue(src);
                    if (field_value != null)
                    {
                        field.SetValue(dst, Clone(field_value));
                    }
                }
            }


        }
    }
}
