using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.IO;

using System.Diagnostics;
using System.Reflection;

using System.Threading;

namespace TrboX
{
    public class DataBase
    {
        private static SQLiteConnection conn = null;
        private static SQLiteCommand cmd;

        public static bool open(string dbpath)
        {
            bool shouldCreateFile = !File.Exists(App.runTimeDirectory + dbpath);
            try
            {
                conn = new SQLiteConnection("Data Source=" + App.runTimeDirectory + dbpath);
                conn.Open();

                if (shouldCreateFile)
                {
                    createTable("CREATE TABLE runtime ( "
                        + "id       INTEGER   PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"
                        + " time     TIMESTAMP DEFAULT ( datetime( 'now', 'localtime' )  ),"
                        + "source   CHAR,"
                        + "contents TEXT "
                        + ");");
                }

                ThreadStart threadStart = new ThreadStart(delegate() { InsertLogThread(); });
                Thread th = new Thread(threadStart);
                th.Start();
            }
            catch (SQLiteException e)
            {
                conn = null;
                //Console.WriteLine(e.Message);
            }

            return true;
        }

        private static  bool createTable(string command)
        {
            if (null != conn)
            {
                try
                {
                    cmd = conn.CreateCommand();
                    cmd.CommandText = command;
                    cmd.ExecuteNonQuery();
                    return true;
                }
                catch (SQLiteException e)
                {
                   // Console.WriteLine(e.Message);
                    return false;
                }

            }
            return false;
        }

        public static void close()
        {
            if (null != conn)
            {
                conn.Close();
            }
        }


        private static Queue<Dictionary<string, string>> log = new Queue<Dictionary<string, string>>();
        private  static void InsertLogThread()
        {
            while(true)
            {
                try
                {
                    lock (log)
                    {
                        if (log.Count > 0)
                        {
                            InsertRecord(log.Dequeue());
                        }
                        else
                        {
                            Thread.Sleep(1000);
                        }
                    }
                }
                catch { }
            }
        }

        public static bool InsertLog(string str)
        {
            string source = "undefined";
            try{
                source = (new StackTrace()).GetFrame(1).GetMethod().DeclaringType.FullName + " >> " + (new StackTrace()).GetFrame(1).GetMethod().Name;
            }
            catch{

            }

            try
            {               
                    new Thread(new ThreadStart(delegate() {

                        Dictionary<string, string> value = new Dictionary<string, string>();
                        value.Add("contents", str);
                        value.Add("source", source);

                        lock (log)
                        {
                            log.Enqueue(value);
                        }
                    })).Start();
                    return true;
              
            }
            catch
            {

            }
            return false;
        }

        private static bool InsertRecord(Dictionary<string, string> value)
        {

            if (null != conn)
            {
                try
                {
                    string sql = "INSERT INTO runtime (";
                    foreach (var item in value.Keys)
                    {
                        sql += item + ",";
                    }
                    sql = sql.Substring(0, sql.Length - 1); // remove last ','
                    sql += ") values(";
                    foreach (var item in value.Values)
                    {
                        sql += "'" + item + "',";
                    }
                    sql = sql.Substring(0, sql.Length - 1); // remove last ','
                    sql += ")";

                    cmd = conn.CreateCommand();
                    cmd.CommandText = sql;
                    cmd.ExecuteNonQuery();
                    return true;
                }
                catch (SQLiteException e)
                {
                    //Console.WriteLine(e.Message);
                    return false;
                }
            }


            return false;
        }
    }
}
