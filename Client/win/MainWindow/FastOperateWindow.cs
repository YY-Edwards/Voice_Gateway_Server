using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace TrboX
{
    public class FastOperateWindow
    {
        private Main m_mainWin;

        private BinaryFormatter m_BinFormat = new BinaryFormatter();//创建二进制序列化器
        private string m_FastOperateListPath = "";

        public FastOperateWindow(Main win)
        {
            if (null == win) return;
            m_mainWin = win;

            m_FastOperateListPath = AppDomain.CurrentDomain.BaseDirectory + "SDGESRWE123SDAWE123SFREWE123RGVAWEFAW.tmp";

            Stream FastOperateListFile = new FileStream(m_FastOperateListPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            FastOperateListFile.Position = 0;
            List<FastOperate> FastOperateList = new List<FastOperate>();
            try
            {
                FastOperateList = (List<FastOperate>)m_BinFormat.Deserialize(FastOperateListFile);           
            }
            catch 
            {
            };

            foreach (FastOperate item in FastOperateList)
            {
                m_mainWin.lst_dispatch.Items.Add(item);
            }
        }

        public void Add(FastOperate item)
        {
            foreach (FastOperate it in m_mainWin.lst_dispatch.Items)
            {
                if (JsonConvert.SerializeObject(item).Equals(JsonConvert.SerializeObject(it)))
                {
                    Remove(it);
                    break;
                }
            }

            m_mainWin.lst_dispatch.Items.Insert(0,item);
            m_mainWin.g_IsNeedSaveWorkSpace = true;
        }

        public void ChangePosition(int oldindex, int newindex)
        {
            if ((oldindex < 0) || (oldindex >= m_mainWin.lst_dispatch.Items.Count) || (newindex < 0) || (newindex >= m_mainWin.lst_dispatch.Items.Count)) return;

            FastOperate item = m_mainWin.lst_dispatch.Items[oldindex] as FastOperate;

            if(null == item)return;



            if (oldindex > newindex)
            {
                m_mainWin.lst_dispatch.Items.Insert(newindex, item);
                m_mainWin.lst_dispatch.Items.RemoveAt(oldindex + 1);
            }
            else
            {
                m_mainWin.lst_dispatch.Items.Insert(newindex + 1, item);
                m_mainWin.lst_dispatch.Items.RemoveAt(oldindex);
            }

            m_mainWin.g_IsNeedSaveWorkSpace = true;

        }


        public void Remove(FastOperate item)
        {
            m_mainWin.lst_dispatch.Items.Remove(item);
            m_mainWin.g_IsNeedSaveWorkSpace = true;           
        }

        public void Save()
        {
            List<FastOperate> FastOperateList = new List<FastOperate>();
            foreach(FastOperate item in m_mainWin.lst_dispatch.Items)
            {
                FastOperateList.Add(item);
            }

            Stream FastOperateListFile = new FileStream(m_FastOperateListPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            FastOperateListFile.SetLength(0);
            m_BinFormat.Serialize(FastOperateListFile, FastOperateList);
        }
    }
}
