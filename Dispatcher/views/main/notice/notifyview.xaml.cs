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

using Sigmar.Controls;
using Dispatcher.ViewsModules;
using Dispatcher.Service;
using Dispatcher;

namespace Dispatcher.Views
{
    /// <summary>
    /// NotifyView.xaml 的交互逻辑
    /// </summary>
    public partial class NotifyView : UserControl
    {
        private List<Border> BorderList;
        private List<GridSplitter>SplitterList;
        private List<bool> EnableList;
        public NotifyView()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(control_loaded);
        }

        private void control_loaded(object sender, RoutedEventArgs e)
        {
            if (this.DataContext != null)
            {
                (this.DataContext as VMNotify).OnViewModulesOperated += new OperatedEventHandler(OnViewChanged);               
            }
           

            BorderList = new List<Border> { border1, border2, border3, border4, border5 };
            SplitterList = new List<GridSplitter>{ spl1, spl2, spl3, spl4, spl5 };
            EnableList = new List<bool>{
                FunctionConfigure._enableAlarm,
                FunctionConfigure._enableSchedule,
                FunctionConfigure._enableShortMessage,
                FunctionConfigure._enableJobTicket,
                FunctionConfigure._enablePatrol,
            };

            for (int i = 0; i < 5; i++)if (!EnableList[i]) UpdateNotify(OperateType_t.CloseNotify, i);

            Log.Info("Notify List is Loaded");
        }

        private void OnViewChanged(OperatedEventArgs e)
        {
            this.Dispatcher.BeginInvoke((Action)delegate()
            {
                UpdateNotify(e.Operate, (int)e.parameter);
            });          
        }


        private void UpdateNotify(OperateType_t op, int index)
        {           
            if (op == OperateType_t.OpenNotify)
            {
                if (!EnableList[index] || BorderList[index].Visibility == System.Windows.Visibility.Visible) return;
                BorderList[index].Visibility = System.Windows.Visibility.Visible;
                grid.RowDefinitions.Add(new RowDefinition() { MinHeight = 44 });
                AddRowNumber(index, 1);
            }
            else
            {
                if (BorderList[index].Visibility != System.Windows.Visibility.Visible) return;
                BorderList[index].Visibility = System.Windows.Visibility.Collapsed;
                AddRowNumber(index, -1);
                grid.RowDefinitions.RemoveAt(0);
            }

            UpdateSpl();
        }

        private void CloseNotify(int index)
        {
            BorderList[index].Visibility = System.Windows.Visibility.Collapsed;
            AddRowNumber(index, -1);
            grid.RowDefinitions.RemoveAt(index);
            UpdateSpl();
        }
       
        private void UpdateSpl()
        {
            bool isfirt = true;
            for (int i = 4; i >= 0; i--)
            {
                if (BorderList[i].Visibility == Visibility.Visible)
                {
                    if (isfirt)
                    {
                        isfirt = false;
                        SplitterList[i].Visibility = System.Windows.Visibility.Collapsed;
                    }
                    SplitterList[i].Visibility = System.Windows.Visibility.Visible;

                    Grid.SetRow(SplitterList[i], Grid.GetRow(BorderList[i]));
                    Grid.SetRowSpan(SplitterList[i], Grid.GetRowSpan(BorderList[i]));
                }
                else
                {
                    SplitterList[i].Visibility = System.Windows.Visibility.Collapsed;
                }
            }

            if (isfirt) (this.DataContext as VMNotify).OnNotifyHidden();
        }  
        private Border AddRowNumber(int index, int d = 1)
        {
            for (int i = index + 1; i <5; i++)
            {
                Grid.SetRow(BorderList[i], Grid.GetRow(BorderList[i]) + d);
                Grid.SetRow(SplitterList[i], Grid.GetRow(SplitterList[i]) + d);
            }

            return null;
        }

    }
}
