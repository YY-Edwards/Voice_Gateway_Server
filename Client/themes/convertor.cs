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
            return (double)value - 100;
           
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


    public class FastPanelTypeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value)
            {
                case FastType.FastType_Contact:
                    return "#FF97C5F7";
                case FastType.FastType_Operate:
                    return "#FFD3EA84";
            }
            return "#FFFFFFFF";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((string)value)
            {
                case "#FF97C5F7":
                    return FastType.FastType_Contact;
                case "#FFD3EA84":
                    return FastType.FastType_Operate;
            }
            return null;
        }
    }

    public class FastPanelTitleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value)
            {
                case FastType.FastType_Contact:
                    return "常用联系人";
                case FastType.FastType_Operate:
                    return "快速操作";
            }
            return "未定义面板";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((string)value)
            {
                case "常用联系人":
                    return FastType.FastType_Contact;
                case "快速操作":
                    return FastType.FastType_Operate;
            }
            return null;
        }
    }

    public class ItemIconConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value[0])
            {
                case FastType.FastType_Contact:
                    if (null != value[1])
                        switch ((OrgItemType)value[1])
                        {
                            case OrgItemType.Type_Group:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/group.png"));
                            case OrgItemType.Type_Employee:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/contact.png"));
                            case OrgItemType.Type_Vehicle:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/vehicle.png"));
                            case OrgItemType.Type_Ride:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/ride.png"));
                            case OrgItemType.Type_Radio:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/radio.png"));
                        }
                    break;
                case FastType.FastType_Operate:
                    if (null != value[2])
                        switch ((OPType)value[2])
                        {
                            case OPType.Dispatch:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/dispatch.png"));
                            case OPType.ShortMessage:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/shortmessage.png"));
                            case OPType.Position:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/position.png"));
                            case OPType.Control:
                                return new BitmapImage(new Uri("pack://application:,,,/themes/resource/control.png"));
                        }
                    break;
            }

            return null;
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
            CRelationShipObj contact = value as CRelationShipObj;
            if (null == contact) return Visibility.Collapsed;

            if ((OrgItemType.Type_Group != contact.key) && (null == contact.radio))
            {
                return Visibility.Collapsed;
            }

            return Visibility.Visible;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }


    public class FastPanelItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((FastType)value)
            {
                case FastType.FastType_Contact:
                    return 0;
                case FastType.FastType_Operate:
                    return 1;
            }
            return -1;
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
            switch ((OrgItemType)value)
            {
                case OrgItemType.Type_Group:
                    return Visibility.Collapsed;
            }

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
            switch ((OrgItemType)value)
            {
                case OrgItemType.Type_Group:
                    return 0;
                case OrgItemType.Type_Employee:
                    return 1;
                case OrgItemType.Type_Vehicle:
                    return 2;
                case OrgItemType.Type_Ride:
                    return 3;
                case OrgItemType.Type_Radio:
                    return 4;
            }
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class OperateTypeItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch ((OPType)value)
            {
                case OPType.Dispatch:
                    return 0;
                case OPType.ShortMessage:
                    return 1;
                case OPType.Position:
                    return 2;
                case OPType.Control:
                    return 3;
                case OPType.JobTicker:
                    return 4;
            }
            return -1;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class IconFormOrgConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return " ";

            CRelationShipObj target = value as CRelationShipObj;

            switch (target.key)
            {
                case OrgItemType.Type_Group:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/group.png"));
                case OrgItemType.Type_Employee:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/contact.png"));
                case OrgItemType.Type_Vehicle:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/vehicle.png"));
                case OrgItemType.Type_Ride:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/ride.png"));
                case OrgItemType.Type_Radio:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/radio.png"));
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class IconOpFormOrgConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return 0.4;

            CRelationShipObj target = value as CRelationShipObj;
            if (null != target.radio)
            if (true == target.radio.is_online) return 1;

            return 0.4;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }

    public class IconEnableFormOrgConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return false;

            CRelationShipObj target = value as CRelationShipObj;
            if (null != target.radio)
                if (true == target.radio.is_online) return true;

            return false;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }
}
