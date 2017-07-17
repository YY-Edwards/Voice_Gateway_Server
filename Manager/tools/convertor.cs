using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;


namespace Manager
{
    //connect status
    public class ServerStatusConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                if (value == null || value.Length < 2) throw new NotSupportedException();

                if (!(value[0] is bool) || !(value[1] is bool)) throw new NotSupportedException();

                if (!(bool)value[0] || !(bool)value[1])
                {
                    return "服务未连接";
                }
                else
                {
                    return "已连接服务";
                }
            }
            catch
            {
                return "服务未连接";
            }
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class ConnectBtnConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                if (value == null || value.Length < 2) throw new NotSupportedException();

                if (!(value[0] is bool) || !(value[1] is bool)) throw new NotSupportedException();

                if (!(bool)value[0] || !(bool)value[1])
                {
                    return Visibility.Visible;
                }
                else
                {
                    return Visibility.Collapsed;
                }
            }
            catch
            {
                return Visibility.Collapsed;
            }
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class MultiParameter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            return value.ToArray();
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class TrueVisable : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                bool tmp = false;
                if(value != null)
                 foreach(object item in value)
                 {
                     if(item is bool)
                     {
                         if((bool)item)
                         {
                             tmp = true; break;
                         }
                     }
                 }



                if(parameter != null && parameter is bool)
                {
                    tmp = (bool)parameter ?  tmp : !tmp;
                }

                return tmp ? Visibility.Visible : Visibility.Collapsed;

            }
            catch
            {
                if(parameter != null && parameter is bool)
                {
                    return (bool)parameter ? Visibility.Visible : Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Visible;
                }
            }
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FalseHide : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                bool tmp = true;
                if (value != null)
                    foreach (object item in value)
                    {
                        if (item is bool)
                        {
                            if (!(bool)item)
                            {
                                tmp = false; break;
                            }
                        }
                    }



                if (parameter != null && parameter is bool)
                {
                    tmp = (bool)parameter ? tmp : !tmp;
                }

                return tmp ? Visibility.Visible : Visibility.Collapsed;

            }
            catch
            {
                if (parameter != null && parameter is bool)
                {
                    return (bool)parameter ? Visibility.Visible : Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Collapsed;
                }
            }
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }



    public class IDToIndex : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                long id = (long)value[0];
                List<CRElement> list = new List<CRElement>(((ObservableCollection<CRElement>)value[1]).ToList());

                int index = list.FindIndex(p => p.ID == id);

                return index;
            }
            catch
            {
                return -1;
            }
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {                       
            return null;
        }
    }


}
