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

namespace TrboX
{
    /// <summary>
    /// operate.xaml 的交互逻辑
    /// </summary>
    public partial class OperateWin : MyWindow
    {
        OpView m_View;
        public OperateWin():base()
        {
            InitializeComponent();
            

            this.Loaded += delegate
            {           
                Title = "张三";
                SubTitle = "车辆：川A12345  组ID:10（地勤） 设备ID：123";

                m_View = new OpView(this);
            };
        }


    }
}
