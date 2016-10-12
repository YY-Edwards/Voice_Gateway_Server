using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace TrboX
{
    public class IsNullValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if(null == value)return "true";
            return "false";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {           
            return value;
        }
    }

    public class MsgWidthValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            
            if((double)value > 100)return (double)value - 100;
            else return (double)value;
           
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class MainTabIndexFromRadio : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            for (int i = 0; i < value.Count(); i++)
            {
                if(true == (bool)value[i])
                {
                    return i;
                }
            }
            return 0;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class ButtonHideItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            //CRelationShipObj contact = value as CRelationShipObj;
            //if (null == contact) return Visibility.Collapsed;

            //if ((OrgItemType.Type_Group != contact.key) && (null == contact.radio))
            //{
            //    return Visibility.Collapsed;
            //}

            return Visibility.Visible;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }



    public class NullHideConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if(value is string)
            {
                if ("" == (string)value)
                {
                    return Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Visible;
                }
            }

            if(value is bool)
            {
                if (false == (bool)value)
                {
                    return Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Visible;
                }
            }

            if (null == value)
            {
                return Visibility.Collapsed;
            }
            else
            {
                return Visibility.Visible;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class FalselHideConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (false == (bool)value)
            {
                return Visibility.Collapsed;
            }
            else
            {
                return Visibility.Visible;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class MonitorConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            //switch ((OrgItemType)value)
            //{
            //    case OrgItemType.Type_Group:
            //        return Visibility.Collapsed;
            //}

            return Visibility.Visible;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class ContactTypeItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            //switch ((OrgItemType)value)
            //{
            //    case OrgItemType.Type_Group:
            //        return 0;
            //    case OrgItemType.Type_Employee:
            //        return 1;
            //    case OrgItemType.Type_Vehicle:
            //        return 2;
            //    case OrgItemType.Type_Ride:
            //        return 3;
            //    case OrgItemType.Type_Radio:
            //        return 4;
            //}
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class MsgMAxWidthConverter : IValueConverter
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

}




