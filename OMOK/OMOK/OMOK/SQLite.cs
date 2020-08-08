﻿using SQLite;
using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    static public class SQLite
    {
        [Table("ResultLog")]

        public class ResultLog
        {
            [PrimaryKey, AutoIncrement, Column("_id")]
            public int Id { get; set; }

            public DateTime Time { get; set; }

            public string OpponentName  { get; set; }
            public string MyName { get; set; }
            public byte Result { get; set; }
        }

        static public bool InsertResultLog(DateTime time,string opponentName,byte result)
        {
            ResultLog Data = new ResultLog();
            Data.Time = time;
            Data.MyName = User.myInfo.NickName;
            Data.OpponentName = opponentName;
            Data.Result = result;

            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "BD2.db");

            var db = new SQLiteConnection(DBPath);
            db.CreateTable<ResultLog>();

            db.Insert(Data);

            return true;
        }

        static public List<ResultLog> ReadResultLog()
        {
            string DBPath = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "BD2.db");

            var db = new SQLiteConnection(DBPath);

            db.CreateTable<ResultLog>();

            var table = db.Table<ResultLog>();

            List<ResultLog> list = new List<ResultLog>();

            foreach (var s in table)
            {
                list.Add(s);
            }

            return list;
        }

    }
}
