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
        private CRelationShipObj Target = null;
        private OPType Operate = OPType.Dispatch;

        public OperateWin(CRelationShipObj target)
            : base()
        {
            InitializeComponent();

            if(null == target)return;
            Target = target;

            this.Loaded += delegate
            {           
                Title = Target.KeyName;
                SubTitle = Target.HeaderWithoutKey;

                m_View = new OpView(this);

                OnChangeOperateType();
            };
        }

        public override void OnMyWindow_Btn_Close_Click()
        {
            this.Hide();
        }

        public void SetOperateType(OPType type)
        {
            Operate = type;
            OnChangeOperateType();
        }

        private void OnChangeOperateType()
        {
            switch (Operate)
            {
                case OPType.Dispatch:
                case OPType.ShortMessage:
                case OPType.JobTicker:
                case OPType.Position:
                case OPType.Tracker:
                default:
                    break;

            };
        }
    }
}
