using SQLite;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Xml.Linq;
using DependencyHelper;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.PlatformConfiguration;

namespace Antioch
{
    [Table("User")]
    public class UserCacheData
    {
        public UserCacheData()
        {
            Id = 1;

#if GLOBAL
            BibleName = "Genesis";
#else
            BibleName = "창세기";
#endif

            Chapter = 1;
            Verse = 1;

        }
        //[PrimaryKey, AutoIncrement, Column("_id")]
        [PrimaryKey, Column("_id")]
        [System.ComponentModel.DefaultValue(1)]
        public int Id { get; set; }

        [System.ComponentModel.DefaultValue(20)]
        public int FontSize { get; set; }

#if GLOBAL
        [System.ComponentModel.DefaultValue("Genesis")]
#else
        [System.ComponentModel.DefaultValue("창세기")]
#endif
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

    [Table("BibleReadPlan")]
    public class BibleReadPlan
    {
        [PrimaryKey, AutoIncrement, Column("_id")]
        public int Id { get; set; }

        public string BibleName { get; set; }

        public int Count { get; set; }

        public int BeginChapter { get; set; }

        public DateTime StartTime { get; set; }
    }

    [Table("UnderliningRcord")]
    public class Underlining
    {
        [PrimaryKey, AutoIncrement, Column("_id")]
        public int Id { get; set; }

        [Column("BibleName")]
        public string BibleName { get; set; }

        public int Chapter { get; set; }

        public int Verse { get; set; }

        public string Color { get; set; }
    }

    static public class SQLLiteDB
    {
        static public void InitDB()
        {
        }

        static public string GetPath()
        {
            // Platform (Android)
            var platform = DeviceInfo.Platform;

            if (platform == DevicePlatform.UWP)
            {
                return System.IO.Path.Combine(DependencyService.Get<FlatformPath>().GetPath(), "WBA.db");
            }
            else if (platform == DevicePlatform.Android)
            {
                return System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");
            }
            else
                return System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "WBA.db");

        }
        static public bool InsertUnderlining(string BibleName, int Chapter, int Verse, string color)
        {
            Underlining Data = new Underlining();
            Data.BibleName = BibleName;
            Data.Chapter = Chapter;
            Data.Verse = Verse;
            Data.Color = color;

            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<Underlining>();

            var list = db.Query<Underlining>("select * from UnderliningRcord where BibleName = ? and Chapter = ? and Verse = ?", BibleName, Chapter, Verse);

            if (list.Count == 0)
            {
                db.Insert(Data);
            }
            else
            {
                if (list?[0] != null)
                {
                    list[0].Color = color;
                    db.Update(list[0]);
                }
            }

            return true;
        }

        static public bool DeleteUnderlining(string BibleName, int Chapter, int Verse, string color)
        {
            Underlining Data = new Underlining();
            Data.BibleName = BibleName;
            Data.Chapter = Chapter;
            Data.Verse = Verse;
            Data.Color = color;

            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<Underlining>();

            var list = db.Query<Underlining>("delete from UnderliningRcord where BibleName = ? and Chapter = ? and Verse = ?", BibleName, Chapter, Verse);

            return true;
        }

        static public List<Underlining> ReadUnderlining()
        {
            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<Underlining>();

            var table = db.Table<Underlining>();

            var list = db.Query<Underlining>("select * from UnderliningRcord");

            return list;
        }

        static public bool InsertBibleReadPlan(DateTime Time, string Name, int Count,int BeginChapter)
        {
            BibleReadPlan Data = new BibleReadPlan();
            Data.Id = 1;
            Data.StartTime = Time;
            Data.BibleName = Name;
            Data.Count = Count;
            Data.BeginChapter = BeginChapter;

            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<BibleReadPlan>();

            var list = db.Query<BibleReadPlan>("select * from BibleReadPlan where _id = ?", Data.Id);

            if (list.Count == 0)
            {
                db.Insert(Data);
            }
            else
                db.Update(Data);

            return true;
        }

        static public BibleReadPlan ReadBibleReadPlan()
        {
            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<BibleReadPlan>();

            var table = db.Table<BibleReadPlan>();

            var list = db.Query<BibleReadPlan>("select * from BibleReadPlan where _id = ?", 1);

            foreach (var s in list)
            {
                return s;
            }

            return null;
        }

        static public bool InsertScheduleLog(DateTime Time)
        {
            ScheduleLog Data = new ScheduleLog();
            Data.Time = Time;

            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<ScheduleLog>();

            db.Insert(Data);

            return true;
        }

        static public List<ScheduleLog> ReadScheduleLog()
        {
            string DBPath = GetPath();

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
                User.CacheData.FontSize = UserCacheData.FontSize;
                User.CacheData.BibleName = UserCacheData.BibleName;
                User.CacheData.Chapter = UserCacheData.Chapter;
                User.CacheData.Verse = UserCacheData.Verse;
                User.CacheData.UserName = UserCacheData.UserName;
                User.CacheData.Passwd = UserCacheData.Passwd;
                User.CacheData.EnalbeKJV = UserCacheData.EnalbeKJV;
            }
            else
            {
#if GLOBAL
                User.CacheData.BibleName = "Genesis";
#else
                User.CacheData.BibleName = "창세기";
#endif
                User.CacheData.Chapter = 1;
                User.CacheData.Verse = 1;
                User.CacheData.FontSize = 20;
                User.CacheData.EnalbeKJV = true;
            }
        }

        static public bool Upsert(int FontSize, string BibleName, int Chapter, int Verse, string UserName, string Passwd, int id = 1)
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
            string DBPath = GetPath();

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
            string DBPath = GetPath();

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
            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<UserScheduleData>();

            var table = db.Table<UserScheduleData>();
            if (isInsert == true)
                db.Insert(Data);
            else
                db.Update(Data);

            return true;
        }

        static public bool DelItem(long id)
        {
            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<UserScheduleData>();

            var table = db.Table<UserScheduleData>();

            var list = db.Query<UserCacheData>("delete from UserScheduleData where _id = ?", id);

            if (list.Count != 0)
                return true;
            else
                return false;
        }

        static public List<UserScheduleData> ReadUserScheduleData()
        {
            string DBPath = GetPath();

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
