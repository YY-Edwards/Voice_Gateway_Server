using System;
using System.Collections.Generic;
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

using System.Windows.Media.Animation;

namespace Dispatcher.Views
{
    public class LocalStyle
    {
        public static SolidColorBrush TargetPanelBackground
        {
            get
            {
                return new SolidColorBrush(Color.FromArgb(255, 80, 205, 228));
            }
        }


        public static SolidColorBrush TargetPanelOffineBackground
        {
            get
            {
                return new SolidColorBrush(Color.FromArgb(50, 80, 205, 228));
            }
        }

        public static SolidColorBrush TargetPanelInCallBackground
        {
            get
            {
                ColorAnimation colorAnm = new ColorAnimation(Color.FromArgb(127, 80, 205, 228), new Duration(TimeSpan.FromSeconds(0.1)));
                colorAnm.AutoReverse = true;
                colorAnm.From = Color.FromArgb(255, 80, 205, 228);
                colorAnm.RepeatBehavior = RepeatBehavior.Forever;

                SolidColorBrush myBrush = new SolidColorBrush();
                myBrush.BeginAnimation(SolidColorBrush.ColorProperty, colorAnm);
                return myBrush;
            }      
        }

        public static SolidColorBrush OperationPanelBackground
        {
            get
            {
                return new SolidColorBrush(Color.FromArgb(255, 120, 172, 229));
            }
        }

        public static SolidColorBrush OperationPanelOffineBackground
        {
            get
            {
                return new SolidColorBrush(Color.FromArgb(50, 120, 172, 229));
            }
        }

        public static SolidColorBrush OperationPanelInCallBackground
        {
            get
            {
                ColorAnimation colorAnm = new ColorAnimation(Color.FromArgb(127, 120, 172, 229), new Duration(TimeSpan.FromSeconds(0.1)));
                colorAnm.AutoReverse = true;
                colorAnm.From = Color.FromArgb(255, 120, 172, 229);
                colorAnm.RepeatBehavior = RepeatBehavior.Forever;

                SolidColorBrush myBrush = new SolidColorBrush();
                myBrush.BeginAnimation(SolidColorBrush.ColorProperty, colorAnm);
                return myBrush;
            }

        }

    }
}
