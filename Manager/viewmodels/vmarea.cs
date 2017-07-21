using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;
using System.Windows.Media.Imaging;

namespace Manager
{
    public class CVMArea:CVMManager, INotifyPropertyChanged
    {

        public CVMArea()
        {
            if (m_Area == null)
            {
                m_Area = new CAreaMgr();
                m_Area.OnResourceChanged += OnResourceChanged;
            }

            if (m_EditArea == null) m_EditArea = new CArea();           
        }


        private CAreaMgr m_Area;
        public ObservableCollection<CRElement> Areas { get { return new ObservableCollection<CRElement>(m_Area.List); } }
      

        private CArea m_EditArea;
        public CArea EditArea
        {
            get { return m_EditArea; }
            set
            {
                if (value == null)
                {
                    m_EditArea = new CArea();
                    m_Area.IsNew = true;
                }
                else
                {
                    m_EditArea = value;
                    m_Area.IsNew = false;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                    PropertyChanged(this, new PropertyChangedEventArgs("Map"));
                }
            }
        }

        public string Name { set { m_EditArea.Name = value; } get { if (m_EditArea == null)return null; return m_EditArea.Name; } }
        public string Map
        {
            set
            {
                if (System.IO.File.Exists(value))
                {
                    m_EditArea.LocalPath = value;

                   try
                   {
                        BitmapImage mapimage = new BitmapImage(new Uri(m_EditArea.LocalPath));
                        m_EditArea.Width = mapimage.Width;
                        m_EditArea.Height = mapimage.Height;
                    }
                   catch
                   {

                   }

                    System.IO.FileInfo map = new System.IO.FileInfo(value);
                    m_EditArea.Map = map.Name;

                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("Map"));
                    }
                }
            }
            get
            {

                if (m_EditArea == null) return "";
                if (System.IO.File.Exists(m_EditArea.LocalPath))
                {
                    return m_EditArea.LocalPath;
                }
                else
                {
                    return  (m_EditArea.Map == null ||  m_EditArea.Map ==string.Empty )? "" : ("TServer://" + m_EditArea.Map);
                }
            }
        }

        private void OnResourceChanged(RequestOpcode type, List<CRElement> res)
        {
            if (type == RequestOpcode.area && PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Areas"));
            }
        }


        //command
        public ICommand New { get { return new CDelegateCommand(NewEle); } }
        public ICommand Delete { get { return new CDelegateCommand(DeleteEle); } }
        public ICommand Save { get { return new CDelegateCommand(SaveEle); } }
        public ICommand Browse { get { return new CDelegateCommand(BrowseMapMethod); } }


        private void NewEle()
        {
            m_Area.IsNew = true;
            m_EditArea = new CArea();
            PropertyChanged(this, new PropertyChangedEventArgs("EditArea"));
            PropertyChanged(this, new PropertyChangedEventArgs("Name"));
        }

        private void DeleteEle()
        {
            if (m_EditArea == null) return;
            m_Area.Delete(m_EditArea.ID);
            PropertyChanged(this, new PropertyChangedEventArgs("Areas"));
        }

        private void SaveEle(object parameter)
        {
            if (m_EditArea == null) return;
            try
            {
                if (parameter == null ||!(parameter is ListView)) return;


                ListView lst = parameter as ListView;

                if (m_Area.IsNew)
                {                    
                    m_Area.Add(m_EditArea);
                }
                else
                {
                    m_Area.Modify(m_EditArea.ID, m_EditArea);
                }


                PropertyChanged(this, new PropertyChangedEventArgs("Areas"));
                lst.ScrollIntoView(lst.SelectedItem);

            }
            catch
            {

            }

            m_Area.IsNew = false;
        }

        private void BrowseMapMethod()
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog();
            openFileDialog.Title = "选择文件";
            openFileDialog.Filter = "图片文件|*.jpg;*.bmp|所有文件|*.*";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;
            System.Windows.Forms.DialogResult result = openFileDialog.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.Cancel)
            {
                return;
            }

            m_EditArea.LocalPath = openFileDialog.FileName;

            try
            {
                BitmapImage mapimage = new BitmapImage(new Uri(m_EditArea.LocalPath));
                m_EditArea.Width = mapimage.Width;
                m_EditArea.Height = mapimage.Height;
            }
            catch
            {

            }

            System.IO.FileInfo map = new System.IO.FileInfo(openFileDialog.FileName);
            m_EditArea.Map = map.Name;

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("Map"));
            }
        }


        //base interface
        public event PropertyChangedEventHandler PropertyChanged;
        public void Set()
        {
            m_Area.UploadMap();       
            m_Area.Save();
        }

        public void Get()
        {
            m_Area.Query();
        }

    }
}
