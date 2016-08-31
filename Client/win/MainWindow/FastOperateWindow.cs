using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

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
            m_mainWin.lst_dispatch.Items.Insert(0,item);
        }

        public void Remove(FastOperate item)
        {
            m_mainWin.lst_dispatch.Items.Remove(item);
            
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
