using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TrboX
{
    public class TileView : ViewBase
    {
        private DataTemplate itemTemplate;
        public DataTemplate ItemTemplate
        {
            set { itemTemplate = value; }
            get { return itemTemplate; }
        }

        protected override object DefaultStyleKey
        {
            get
            {
                return new ComponentResourceKey(GetType(), "TileView");
            }
        }

        protected override object ItemContainerDefaultStyleKey
        {
            get
            {
                return new ComponentResourceKey(GetType(), "TileViewItem");
            }
        }
    }

    class CLayout
    {
        //private MainWindow m_mainWindow;
        //public CLayout(MainWindow win)
        //{
        //    m_mainWindow = win;
        //}

        //public void Exit()
        //{
        //    Environment.Exit(0);
        //}

    }
}
