using SQLite;
using System;
using System.Collections.Generic;
using System.Text;

namespace WBA
{
    [Table("User")]
    public class UserCacheData
    {
        public UserCacheData()
        {
            Id = 1;
        }
        //[PrimaryKey, AutoIncrement, Column("_id")]
        [PrimaryKey, Column("_id")]
        [System.ComponentModel.DefaultValue(1)]
        public int Id { get; set; }

        [System.ComponentModel.DefaultValue(20)]
        public int FontSize { get; set; }

        [System.ComponentModel.DefaultValue("창세기")]
        public string BibleName { get; set; } //디폴트는 창세기
        [System.ComponentModel.DefaultValue(1)]
        public int Chapter { get; set; }    //디폴트 1장
        [System.ComponentModel.DefaultValue(1)]
        public int Verse { get; set; }     //디폴트 1절

        public bool EnalbeKJV { get; set; }

        public string UserName { get; set; }
        public string Passwd { get; set; }
    }

    public class UserScheduleData
    {
        [PrimaryKey, AutoIncrement, Column("_id")]
        public int Id { get; set; }

        public DateTime Time { get; set; }
        public string Message { get; set; }
        public bool IsNotify { get; set; }
    }

    public class ScheduleLog
    {
        [PrimaryKey, AutoIncrement, Column("_id")]
        public int Id { get; set; }

        public DateTime Time { get; set; }
    }

    static public class SQLLiteDB
    {
        static public UserCacheData CacheData = new UserCacheData();

        static public void InitDB()
        {
        }

        static public bool InsertScheduleLog(DateTime Time)
        {
            ScheduleLog Data = new ScheduleLog();
            Data.Time = Time;

            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<ScheduleLog>();

            db.Insert(Data);

            return true;
        }

        static public List<ScheduleLog> ReadScheduleLog()
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<ScheduleLog>();

            var table = db.Table<ScheduleLog>();

            List<ScheduleLog> list = new List<ScheduleLog>();

            foreach (var s in table)
            {
                list.Add(s);
            }

            return list;
        }

        static public void LoadCacheData()
        {
            var UserCacheData = SQLLiteDB.ReadUserCache();
            if (UserCacheData != null)
            {
                SQLLiteDB.CacheData.FontSize = UserCacheData.FontSize;
                SQLLiteDB.CacheData.BibleName = UserCacheData.BibleName;
                SQLLiteDB.CacheData.Chapter = UserCacheData.Chapter;
                SQLLiteDB.CacheData.Verse = UserCacheData.Verse;
                SQLLiteDB.CacheData.UserName = UserCacheData.UserName;
                SQLLiteDB.CacheData.Passwd = UserCacheData.Passwd;

            }
            else
            {
                SQLLiteDB.CacheData.BibleName = "창세기";
                SQLLiteDB.CacheData.Chapter = 1;
                SQLLiteDB.CacheData.Verse = 1;
                SQLLiteDB.CacheData.FontSize = 20;
                SQLLiteDB.CacheData.EnalbeKJV = true;
            }
        }

        static public bool Upsert(int FontSize, string BibleName, int Chapter, int Verse, string UserName, string Passwd,int id = 1)
        {
            UserCacheData data = new UserCacheData();
            data.FontSize = FontSize;
            data.Id = id;
            data.BibleName = BibleName;
            data.Chapter = Chapter;
            data.Verse = Verse;
            data.UserName = UserName;
            data.Passwd = Passwd;

            return Upsert(data);
        }
        static public bool Upsert(UserCacheData Data)
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<UserCacheData>();


            var list = db.Query<UserCacheData>("select * from User where _id = ?", Data.Id);

            if (list.Count == 0)
            {
                // only insert the data if it doesn't already exist
                db.Insert(Data);

            }
            else
                db.Update(Data);

            return true;
        }

        static public UserCacheData ReadUserCache(int id = 1)
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<UserCacheData>();


            var list = db.Query<UserCacheData>("select * from User where _id = ?", id);

            if (list.Count == 0)
                return null;
            else
                return list[0];
        }


        static public bool Upsert(UserScheduleData Data, bool isInsert = true)
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<UserScheduleData>();

            var table = db.Table<UserScheduleData>();
            if (isInsert == true)
                db.Insert(Data);
            else
                db.Update(Data);

            return true;
        }

        static public List<UserScheduleData> ReadUserScheduleData()
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<UserScheduleData>();

            var table = db.Table<UserScheduleData>();

            List<UserScheduleData> list = new List<UserScheduleData>();

            foreach (var s in table)
            {
                list.Add(s);
            }

            return list;
        }
    }

}
