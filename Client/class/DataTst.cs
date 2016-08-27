using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    public class child
    {
        public string v1 { set; get; }
        public bool v2 { set; get; }
    }

    public class person
    {
        public string name { set; get; }
        public int age;

        public child xx { set; get; }

    }
    class DataTst
    {

        public DataTst()
        {
            person[] tst = new person[2]
            {
            
                new person
            {
                name = "zhangsan"
                ,
                age = 10
                ,
                xx = new child
                {
                    v1 = "lisi"
                    ,
                    v2 = true
                }
            },
            new person
            {
                name = "zhangsan"
                ,
                age = 10
                ,
                xx = new child
                {
                    v1 = "lisi"
                    ,
                    v2 = true
                }
            }
            };
            string json = JsonConvert.SerializeObject(tst);
            person[] test = JsonConvert.DeserializeObject(json,typeof(person[])) as person[];

        }
       
    }
}
