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
            catch (Exception e)
            {
                DataBase.InsertLog("Read Fast Operation List Error" + e.Message);
            }

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

        public void Insert(int index, FastOperate item)
        {
            foreach (ListViewItem it in m_mainWin.lst_dispatch.Items)
            {
                if (JsonConvert.SerializeObject(item).Equals(JsonConvert.SerializeObject(((ListViewItem)it).Content)))
                {

                    m_mainWin.lst_dispatch.ScrollIntoView(item);
                    //Remove(item);
                    //break;
                    return;
                }
            }
            try
            {
                m_mainWin.lst_dispatch.Items.Insert(index, new ListViewItem() { Content = item, ContextMenu = new ContextMenu() { Visibility = Visibility.Collapsed } });
                m_mainWin.lst_dispatch.ScrollIntoView(m_mainWin.lst_dispatch.Items[index]);
            }
            catch (Exception e ){
                DataBase.InsertLog("Insert Fast Operation List Error" + e.Message);          
            }
            m_mainWin.g_IsNeedSaveWorkSpace = true;
        }

        public void Update(FastOperate src, FastOperate dest)
        {
            int index = -1;
            for (int i = 0; i< m_mainWin.lst_dispatch.Items.Count; i++)
            {
                if (JsonConvert.SerializeObject(src).Equals(JsonConvert.SerializeObject(((ListViewItem)m_mainWin.lst_dispatch.Items[i]).Content)))
                {
                    index = i;
                    break;
                }
            }

            if (index >= 0)
            {
                m_mainWin.lst_dispatch.Items.RemoveAt(index);
                Insert(index, dest);
            }
          
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


        public void UpdateOpWin(CMember targt, int mask, bool sta)
        {

            Dictionary<int, FastOperate> uplist =new  Dictionary<int, FastOperate>();
            m_mainWin.Dispatcher.Invoke(new Action(() =>
            {
            try{
                for(int i = 0; i< m_mainWin.lst_dispatch.Items.Count; i++)
                {
                    FastOperate dst = (FastOperate)((ListViewItem)m_mainWin.lst_dispatch.Items[i]).Content;

                    if (dst.Type == FastType.FastType_Contact)
                    {
                        try
                        {
                            if (dst.Contact.Type == SelectionType.All)
                            {
                                uplist.Add(i, dst);
                            }
                            else if (dst != null && dst.Contact != null && dst.Contact.Target != null && dst.Contact.Target.Count > 0)
                            {
                                foreach (CMember it in dst.Contact.Target)
                                {
                                    if (it.IsLike(targt))
                                    {
                                        dst.Contact.Target.Remove(it);

                                        if ((mask & 1) != 0)//olnline
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsOnline = sta;
                                        }
                                        else if ((mask & 2) != 0)//ingps
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsGPS = sta;
                                        }
                                        else if ((mask & 4) != 0)//isTx
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsTx = sta;
                                            else it.Group.IsTx = sta;
                                        }
                                        else if ((mask & 8) != 0)//isRx
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsRx = sta;
                                            else it.Group.IsRx = sta;
                                        }

                                        dst.Contact.Target.Add(it);
                                        uplist.Add(i, dst);
                                        break;
                                    }
                                }
                            }
                        }
                        catch { }
                    }
                    else if (dst.Type == FastType.FastType_Operate)
                    {
                        try
                        {
                            if (dst.Contact.Type == SelectionType.All)
                            {
                                uplist.Add(i, dst);
                            }
                            else if (dst != null && dst.Operate != null && dst.Operate.Target != null && dst.Operate.Target.Target != null && dst.Operate.Target.Target.Count > 0)
                            {
                                foreach (CMember it in dst.Operate.Target.Target)
                                {
                                    if (it.IsLike(targt))
                                    {
                                        dst.Operate.Target.Target.Remove(it);

                                        if ((mask & 1) != 0)//olnline
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsOnline = sta;
                                        }
                                        else if ((mask & 2) != 0)//ingps
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsGPS = sta;
                                        }
                                        else if ((mask & 4) != 0)//isTx
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsTx = sta;
                                            else it.Group.IsTx = sta;
                                        }
                                        else if ((mask & 8) != 0)//isRx
                                        {
                                            if (it.Type != MemberType.Group) it.Radio.IsRx = sta;
                                            else it.Group.IsRx = sta;
                                        }
                                        dst.Operate.Target.Target.Add(it);
                                        uplist.Add(i, dst);
                                        break;
                                    }
                                }
                            }
                        }
                        catch
                        { }

                    }

                }

                foreach(var it in uplist)
                {
                    if (it.Key >= 0 && it.Key < m_mainWin.lst_dispatch.Items.Count)
                     {
                         Update(((ListViewItem)m_mainWin.lst_dispatch.Items[it.Key]).Content as FastOperate, it.Value);
                     }
                }
            }
            catch{}
            }));
        }
    }
}
