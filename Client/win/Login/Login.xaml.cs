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
    /// Login.xaml 的交互逻辑
    /// </summary>
    public partial class Login : MyWindow
    {
        Main MainWindow = new Main();
        public Login()
        {
            InitializeComponent();


           // Location loc = new Location()
           // {
           //     Radio = new CMember() {Type = MemberType.Radio, Radio = new Radio() { RadioID = 4 }, Group = new Department() { Name = "cet", GroupID = 12} },
           //     Long = 104.0557511303,
           //     Lat = 30.6049425958
           // };

           //             double mLat, mLon;
           // EvilTransform.transform(loc.Lat, loc.Long, out mLat, out mLon);
           // string paramformat = "DisPosPoint({0},{1},{2},{3}, {4}, {5}, '{6}', '{7}',{8},{9},'{10}', {11},{12})";
           // //string paramformat = "DisPosPoint({0},{1},{2},{3},{4},{5},'{6}','{7}')";

           // string param = String.Format(paramformat,
           //     loc.Radio.Radio.RadioID,//radioid 
           //     (int)loc.Radio.Radio.Type,//raido type radio 0. ride 1
           //     loc.Long,//long
           //     loc.Lat,//lat
           //     loc.Alt,//alt
           //     loc.Speed,//speed
           //     DateTime.Now.ToShortDateString(),
           //     DateTime.Now.ToLongTimeString(),
           //     mLon,//middle long
           //     mLat,//middle lat
           //     loc.Radio.Name,
           //     loc.Radio.Group != null ? loc.Radio.Group.GroupID : -1,
           //     loc.Radio.Group != null ? loc.Radio.Group.Name : "未分组"
           //     );
           //if(param == "") param = "0";

        }
       private void MyWindow_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void btn_Login_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.Show();
            this.Hide();
        }

        private void btn_cancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }


    }
}
