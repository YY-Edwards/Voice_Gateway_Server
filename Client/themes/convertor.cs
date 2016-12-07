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


    public class TypeToOP : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
           if((value[0] == DependencyProperty.UnsetValue)||( value[0] == DependencyProperty.UnsetValue)||( value[0] == DependencyProperty.UnsetValue))return 1;

            switch ((FastType)value[0])
            {
                case FastType.FastType_Contact:
                    CMultMember member = (CMultMember)value[1];
                    if ((null != value[1]) && (0 < member.Target.Count))
                    {
                        if ((1 < member.Target.Count) || (SelectionType.Single != member.Type)) return 1;
                        else
                            switch (((CMultMember)value[1]).Target[0].Type)
                            {
                                case MemberType.Group:return 1;
                                case MemberType.Staff:
                                    if (null != member.Target[0].Staff)
                                    {
                                        if (StaffType.Staff == member.Target[0].Staff.Type) return
                                            (member.Target[0].Radio == null || !member.Target[0].Radio.IsOnline) ? 0.5 : 1;
                                        if (StaffType.Vehicle == member.Target[0].Staff.Type)  return
                                            (member.Target[0].Radio == null || !member.Target[0].Radio.IsOnline) ? 0.5 : 1;
                                    }
                                    return null;
                                case MemberType.Radio:
                                    if (null != member.Target[0].Radio)
                                    {
                                        if (RadioType.Radio == member.Target[0].Radio.Type) return
                                            (member.Target[0].Radio == null || !member.Target[0].Radio.IsOnline) ? 0.5 : 1;
                                        if (RadioType.Ride == member.Target[0].Radio.Type) return
                                            (member.Target[0].Radio == null || !member.Target[0].Radio.IsOnline) ? 0.5 : 1;
                                    }
                                    return null;
                                default:
                                    return null;
                            }
                    }
                    break;
                case FastType.FastType_Operate:
                    return 1;
            }

            return 1;
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }

    public class ItemIconConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
           if((value[0] == DependencyProperty.UnsetValue)||( value[0] == DependencyProperty.UnsetValue)||( value[0] == DependencyProperty.UnsetValue))return null;

            switch ((FastType)value[0])
            {
                case FastType.FastType_Contact:
                    CMultMember member = (CMultMember)value[1];
                    if ((null != value[1]) && (0 < member.Target.Count))
                    {
                        if ((1 < member.Target.Count) || (SelectionType.Single != member.Type))
                            return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_37_32_on.png"));
                        else
                            switch (((CMultMember)value[1]).Target[0].Type)
                            {
                                case MemberType.Group:
                                    return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_37_32_on.png"));
                                case MemberType.Staff:
                                    if (null != member.Target[0].Staff)
                                    {
                                        if (StaffType.Staff == member.Target[0].Staff.Type) return new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_32_32_on.png"));
                                        if (StaffType.Vehicle == member.Target[0].Staff.Type)  return new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_35_29_on.png"));
                                    }
                                    return null;
                                case MemberType.Radio:
                                    if (null != member.Target[0].Radio)
                                    {
                                        if (RadioType.Radio == member.Target[0].Radio.Type) return new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_23_47_on.png"));
                                        if (RadioType.Ride == member.Target[0].Radio.Type) return new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_34_40_on.png"));
                                    }
                                    return null;
                                default:
                                    return null;
                            }
                    }
                    break;
                case FastType.FastType_Operate:
                    if (null != value[2])
                        switch (((COperate)value[2]).Type)
                        {
                            case OPType.Dispatch:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/call_33_34.png"));
                            case OPType.ShortMessage:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/message_34_34.png"));
                            case OPType.Position:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/positon_29_38.png"));
                            case OPType.Control:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/control_43_43.png"));
                            case OPType.JobTicker:
                                return new BitmapImage(new Uri("pack://application:,,,/resource/images/job_tickets_29_36.png"));
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

    public class NullShowConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value is string)
            {
                if ("" != (string)value)
                {
                    return Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Visible;
                }
            }

            if (value is bool)
            {
                if (false != (bool)value)
                {
                    return Visibility.Collapsed;
                }
                else
                {
                    return Visibility.Visible;
                }
            }

            if (null != value)
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
            if (null == value) return null;
             CMultMember member = null;
             if (value is CMember) member = ((CMember)value).SingleToMult();
             else if (value is CMultMember) member = (CMultMember)value;
             else return null;
             
            if ((null == member) || (null == member.Target) || (member.Target.Count == 0)) return null;

            if ((1 < member.Target.Count) || (SelectionType.Single != member.Type))
                return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_18_18_on.png"));
            else 
                switch (member.Target[0].Type)
                {
                    case MemberType.Group:
                        return new BitmapImage(new Uri("pack://application:,,,/resource/images/group_18_18_on.png"));
                    case MemberType.Staff:
                        if (null != member.Target[0].Staff)
                        {
                            if (StaffType.Staff == member.Target[0].Staff.Type) 
                                return (member.Target[0].Radio != null &&  member.Target[0].Radio.IsOnline == true)
                                    ? new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_18_18_on.png"))
                                    : new BitmapImage(new Uri("pack://application:,,,/resource/images/staff_18_18_off.png"));
                            if (StaffType.Vehicle == member.Target[0].Staff.Type)
                                return (member.Target[0].Radio != null && member.Target[0].Radio.IsOnline == true)
                                    ? new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_18_18_on.png"))
                                    : new BitmapImage(new Uri("pack://application:,,,/resource/images/vehicle_18_18_off.png"));
                        }
                        break;
                    case MemberType.Radio:
                        if (null != member.Target[0].Radio)
                        {
                            if (RadioType.Radio == member.Target[0].Radio.Type)
                                return (member.Target[0].Radio.IsOnline == true)
                                    ? new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_18_18_on.png"))
                                    : new BitmapImage(new Uri("pack://application:,,,/resource/images/radio_18_18_off.png"));
                            if (RadioType.Ride == member.Target[0].Radio.Type)
                                return (member.Target[0].Radio.IsOnline == true)
                                    ? new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_18_18_on.png"))
                                    : new BitmapImage(new Uri("pack://application:,,,/resource/images/ride_18_18_off.png"));
                        }
                        break;
                    default: return null;
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
            if (null == value) return 0.5;

            if(value is bool)
            {
                if (false == (bool)value) return 0.5;
                return 1;
            }
            

            CMember target = null;


            if (value is CMember) target = value as CMember;
            if (value is CMultMember) target = ((CMultMember)value).MultToSingle();

            if(target == null) return 0.5;
           
            if (MemberType.Group == target.Type) return 1;

            if (null != target.Radio)
                if (true == target.Radio.IsOnline) return 1;

            return 0.5;
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

            CMember target = value as CMember;
            if (null != target.Radio)
                if (true == target.Radio.IsOnline) return true;

            return false;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }


    public class IconFormNotifyConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (null == value) return false;

            switch ((NotifyType)value)
            {
                case NotifyType.Alarm:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/alarm.png"));
                case NotifyType.Call:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/rx.png"));
                case NotifyType.Message:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/msg.png"));
                case NotifyType.JobTicker:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/job.png"));
                case NotifyType.Tracker:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/tracker.png"));
                case NotifyType.Position:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/position.png"));
                case NotifyType.Control:
                    return new BitmapImage(new Uri("pack://application:,,,/themes/resource/control.png"));
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value;
        }
    }


    public class ContentFormNotifyConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            if (2 != value.Count()) return "";

            switch ((NotifyType)value[0])
            {
                case NotifyType.Alarm:
                case NotifyType.Call:
                    return "语音呼叫"; 
                case NotifyType.Message:
                    return (string)value[1];
            }

            return "";
        }

        public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
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


    public class StatucToColorConverter : IMultiValueConverter
    {
        public object Convert(object[] value, Type targetType, object parameter, CultureInfo culture)
        {
            //return MyWindow.InCallBrush;

            try
            {
                if ((value[0] == DependencyProperty.UnsetValue) || (value[0] == DependencyProperty.UnsetValue) || (value[0] == DependencyProperty.UnsetValue))
                {
                    return MyWindow.TargetNormalBrush;
                }

                if ((FastType)value[0] == FastType.FastType_Contact)
                {
                    CMultMember target =  (CMultMember)value[1];
                    if(target != null)
                    {
                        if (target.Type == SelectionType.All)
                        {
                            if (TargetMgr.IsTx || TargetMgr.IsRx)
                            {
                                return MyWindow.TargetInCallBrush;
                            }
                        }
                        else if (target.Type != SelectionType.Null)
                        {

                            foreach (CMember mem in target.Target)
                            {
                                if (mem.Type == MemberType.Group)
                                {
                                    if (mem.Group.IsTx || mem.Group.IsRx )
                                    {
                                        return MyWindow.TargetInCallBrush;
                                    }
                                }
                                else
                                {
                                    if (mem.Radio == null || !mem.Radio.IsOnline)
                                    {
                                        return MyWindow.TargetOfflineBrush;
                                    }                                   
                                    else if (mem.Radio.IsTx || mem.Radio.IsRx)
                                    {
                                        return MyWindow.TargetInCallBrush;
                                    }
                                }
                            }
                        }
                    }

                    return MyWindow.TargetNormalBrush;
                }
                else if ((FastType)value[0] == FastType.FastType_Operate)
                { 
                    COperate operate = (COperate)value[2];
                    if(operate != null && operate.Type == OPType.Dispatch)
                    {
                        CMultMember target = operate.Target;
                        if (target != null)
                        {
                            if (target.Type == SelectionType.All)
                            {
                                if (TargetMgr.IsTx || TargetMgr.IsRx)
                                {
                                    return MyWindow.OperateInCallBrush;
                                }
                            }
                            else if (target.Type != SelectionType.Null)
                            {

                                foreach (CMember mem in target.Target)
                                {
                                    if (mem.Type == MemberType.Group)
                                    {
                                        if (mem.Group.IsTx || mem.Group.IsRx)
                                        {
                                            return MyWindow.OperateInCallBrush;
                                        }
                                    }
                                    else
                                    {
                                        if (mem.Radio.IsTx || mem.Radio.IsRx)
                                        {
                                            return MyWindow.OperateInCallBrush;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    return MyWindow.OperateNormalBrush;
                }

                return MyWindow.TargetNormalBrush;
            }
            catch (Exception e){
                DataBase.InsertLog("Convert Error" + e.Message);
                return MyWindow.TargetNormalBrush;
            }
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
            if(value != null && (string)value != "")
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
}




