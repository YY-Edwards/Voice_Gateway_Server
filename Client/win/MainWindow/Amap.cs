using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrboX
{
    public class Amap
    {
        private Main m_Main;
        private MyWebBrowse Map;
        public  Amap(Main main)
        {
            if (main != null)m_Main = main;
            Map = new MyWebBrowse("file:///amap/index.html");
            try{
                if(Map != null)m_Main.MyWebGrid.Children.Insert(0, Map);
            }
            catch{

                DataBase.InsertLog("Amap Error");

            }
        }

        public void AddPoint(GPSParam gps)
        {
          // if (gps == null || gps.Source <= 0 || !gps.Gps.Valid) return;

            CMember src = new TargetSimple() { Type = TargetType.Private, ID = gps.Source }.ToMember();

            double mLat, mLon;
            EvilTransform.transform(gps.Gps.Lat, gps.Gps.Lon, out mLat, out mLon);

            string paramformat = "DisPosPoint({0},{1},{2},{3}, {4}, {5}, '{6}', '{7}',{8},{9},'{10}', {11},'{12}')";
            try
            {
                string param = String.Format(paramformat,
                    src.Radio.RadioID,//radioid 
                    (int)src.Radio.Type, //raido type radio 0. ride 1
                    gps.Gps.Lon,//long
                    gps.Gps.Lat,//lat
                    gps.Gps.Alt,//alt
                    gps.Gps.Speed,//speed
                    DateTime.Now.ToShortDateString(),
                    DateTime.Now.ToLongTimeString(),
                    mLon,//middle long
                    mLat,//middle lat
                    src.Name,
                    src.Group != null ? src.Group.GroupID : -1,
                    src.Group != null ? src.Group.Name : "未分组"
                    );


                Map.ExecJs(param);
            }
            catch {
                DataBase.InsertLog("Amap Add Point  Error");
            }
        }

        public void exec(string str)
        {
            Map.ExecJs(str);
        }
        //public void RemovePoint(Location loc)
        //{

        //}

        public void RemovePoint(CMember rad)
        {

        }

        public void ClearPoint()
        {
            Map.ExecJs("closeinfowin()");
        }
    }
}
