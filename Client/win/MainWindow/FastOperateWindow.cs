using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Globalization;

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

            m_FastOperateListPath = App.WorkSpaceTempPath;

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
                m_mainWin.lst_dispatch.Items.Add(new ListViewItem() { Content = item, ContextMenu = new ContextMenu() { Visibility = Visibility.Collapsed } });
            }
        }

        public void Add(FastOperate item)
        {
            foreach (ListViewItem it in m_mainWin.lst_dispatch.Items)
            {
                if (JsonConvert.SerializeObject(item).Equals(JsonConvert.SerializeObject(((ListViewItem)it).Content)))
                {
                    Remove(item);
                    break;
                }
            }

            m_mainWin.lst_dispatch.Items.Insert(0, new ListViewItem() { Content = item, ContextMenu = new ContextMenu() { Visibility = Visibility.Collapsed } });
            m_mainWin.g_IsNeedSaveWorkSpace = true;
        }

        public void ChangePosition(int oldindex, int newindex)
        {
            if ((oldindex < 0) || (oldindex >= m_mainWin.lst_dispatch.Items.Count) || (newindex < 0) || (newindex >= m_mainWin.lst_dispatch.Items.Count)) return;

            FastOperate item = m_mainWin.lst_dispatch.Items[oldindex] as FastOperate;

            if(null == item)return;

            if (oldindex > newindex)
            {
                m_mainWin.lst_dispatch.Items.Insert(newindex, new ListViewItem() { Content = item, ContextMenu = new ContextMenu() { Visibility = Visibility.Collapsed } });
                m_mainWin.lst_dispatch.Items.RemoveAt(oldindex + 1);
            }
            else
            {
                m_mainWin.lst_dispatch.Items.Insert(newindex + 1, new ListViewItem() { Content = item, ContextMenu = new ContextMenu() { Visibility = Visibility.Collapsed } });
                m_mainWin.lst_dispatch.Items.RemoveAt(oldindex);
            }

            m_mainWin.g_IsNeedSaveWorkSpace = true;

        }


        public void Remove(FastOperate item)
        {           
            if(null == item)return;
            List<FastOperate> willdel = new List<FastOperate>();
            foreach (ListViewItem it in m_mainWin.lst_dispatch.Items)
            {
                if (item.IsEqual(it.Content as FastOperate))
                {
                    m_mainWin.lst_dispatch.Items.Remove(it);
                    m_mainWin.g_IsNeedSaveWorkSpace = true;
                    return;
                }
            }
        }


        public List<FastOperate> Get()
        {
            List<FastOperate> FastOperateList = new List<FastOperate>();
            foreach ( ListViewItem  item in m_mainWin.lst_dispatch.Items)
            {
                FastOperateList.Add(item.Content as FastOperate);
            }

            return FastOperateList;
        }
        public void Save()
        {
            List<FastOperate> FastOperateList = Get();

            Stream FastOperateListFile = new FileStream(m_FastOperateListPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);

            FastOperateListFile.SetLength(0);
            m_BinFormat.Serialize(FastOperateListFile, FastOperateList);
        }
    }
}
