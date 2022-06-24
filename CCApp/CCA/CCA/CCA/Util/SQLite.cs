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

namespace CCA
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

        public string UserName { get; set; }
        public string Passwd { get; set; }
    }


    static public class SQLLiteDB
    {
        static public void InitDB()
        {
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

        static public bool Upsert(string UserName, string Passwd, int id = 1)
        {
            UserCacheData data = new UserCacheData();
            data.Id = id;
            data.UserName = UserName;
            data.Passwd = Passwd;

            string DBPath = GetPath();

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<UserCacheData>();


            var list = db.Query<UserCacheData>("select * from User where _id = ?", data.Id);

            if (list.Count == 0)
            {
                // only insert the data if it doesn't already exist
                db.Insert(data);

            }
            else
                db.Update(data);

            return true;
        }
     
        static public void LoadCacheData()
        {
            var UserCacheData = SQLLiteDB.ReadUserCache();
            if (UserCacheData != null)
            {
                User.CacheData.FontSize = UserCacheData.FontSize;
                User.CacheData.UserName = UserCacheData.UserName;
                User.CacheData.Passwd = UserCacheData.Passwd;
            }
            else
            {
#if GLOBAL
                User.CacheData.BibleName = "Genesis";
#else
#endif
            }
        }

        static public string GetPath()
        {
            // Platform (Android)
            var platform = DeviceInfo.Platform;

            if (platform == DevicePlatform.UWP)
            {
                string fileName = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "CCA.db");
                return fileName;
            }
            else if (platform == DevicePlatform.Android)
            {
                return System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "CCA.db");
            }
            else
                return System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "CCA.db");

        }
      
    }

}
