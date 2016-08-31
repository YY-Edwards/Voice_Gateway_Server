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
    /// NewOperate.xaml 的交互逻辑
    /// </summary>
    public partial class NewOperate : MyWindow
    {

        NewOperateView m_View;
        public NewOperate()
        {
            InitializeComponent();
            this.Loaded += delegate
            {
                m_View = new NewOperateView(this);
                
            };
        }

        public override void OnMyWindow_Btn_Close_Click()
        {
            this.Hide();
        }
    }
}
