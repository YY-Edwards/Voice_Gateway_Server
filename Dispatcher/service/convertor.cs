using System;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Linq;

using Sigmar.Extension;
using Dispatcher.Views;
using Dispatcher.ViewsModules;
using Dispatcher;

namespace Dispatcher.Service
{
    public class CNVBoolVisible : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && value is bool)
            {
                if (parameter != null)
                {
                    string tog = parameter as string;
                    if (tog == "true")
                    {
                        if ((bool)value) return Visibility.Collapsed;
                        else return Visibility.Visible;
                    }
                }

                if ((bool)value) return Visibility.Visible;
                else return Visibility.Collapsed;
            }
            else
            {
                return Visibility.Collapsed;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool res = false;
            if (parameter != null && parameter is bool && (bool)parameter)
            {
                res = true;
            }

            if (value != null && value is Visibility)
            {
                if ((Visibility)value == Visibility.Visible) return !res;
                else return res;
            }
            else
            {
                return res;
            }
        }
    }
    public class MultiParameter :  IMultiValueConverter
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

    public class SearchTxtConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && (string)value != "")
            {
                return "";
            }
            else
            {
                return "请输入您的关键字";
            }

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class UserInputConverter :  IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && (string)value != "")
            {
                return "";
            }
            else
            {
                return "请输入您的用户名";
            }

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class UserPasswordConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && (string)value != "")
            {
                return "";
            }
            else
            {
                return "请输入您的密码";
            }

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class EnableTargetMultipleConverter :  IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null) return false;
            int index = (int)value;
            if (index == 0 )return false;
            else return true;
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class LocationCycleVisibleConverter :  IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
           if(value == null)return Visibility.Collapsed;
            int index = (int)value;
            if(index == 1 || index == 3 || index == 5)return Visibility.Visible;
            else return Visibility.Collapsed;
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class ShortMessageArgsConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && value.Length >= 1 && value[0] is string)
            {
                return new ShortMessageArgs(value[0] as string);
            }

            return null;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class LocationArgsConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && value.Length >= 2 && value[0] is int && value[1] is string)
            {
                return new LocationArgs((LocationType_t)value[0], (value[1] as string).ToInt());
            }

            return null;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class NewFastOperationParameterConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && value.Length >= 1 && value[0] is string)
            {
                return new ShortMessageArgs(value[0] as string);
            }

            return null;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class NotifyContentWidthValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if ((double)value > 100) return (double)value - 100;
            else return (double)value;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class NotifyInOperationWidthValueConverter :  IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if ((double)value > 200) return (double)value - 200;
            else return (double)value;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class TabHeaderMaxWidth : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || value.Length < 2) return 0;
            if (value[0] is double && value[1] is double)
            {
                return (double)value[0] - (double)value[1] - 100 > 0 ? (double)value[0] - (double)value[1] -100 : double.NaN;
            }

            return 0;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class PositionConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || value.Length < 3) return 0;
            if (value[0] is double && value[1] is double && value[2] is double)
            {
                return (double)value[0] * (double)value[1] - (double)value[2] / 2;
            }
            
            return 0;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class TargetPositionConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || value.Length < 4) return 0;
            if (value[0] is double && value[1] is double && value[2] is int && value[3] is double)
            {
                double val = (double)value[0] * (double)value[1] - (parameter as string == "x" ? ((double)value[3] / 2) : (double)value[3]);
                int i = (int)value[2];
                if (i > 0)
                {
                    int layer = i % 3 == 0 ? (i / 3) : (i / 3 + 1);
                    
                    switch (i % 3)
                    {
                        case 1://lefttop y -10;x - 5
                            val -= layer * 5;
                            break;
                        case 2: //top y-20
                            if (parameter as string != "x") val -= layer * 10;                           
                            break;
                        case 0: //right top y - 10 x + 5
                            if (parameter as string == "x") val += layer * 5;
                            else val -= layer * 5;  
                            break;
                    }                  
                }
                return val;                
            }

            return 0;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class LoginWarningConverter :  IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || !(value is string)) return Visibility.Collapsed;
            if ((value as string) == "") return Visibility.Collapsed;
            else return Visibility.Visible;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

}
