using System;
using System.Collections.Generic;


namespace Antioch
{
    public class UserCacheData
    {
        public UserCacheData()
        {
            Id = 1;
            BibleName = "창세기";
            Chapter = 1;
            Verse = 1;
            FontSize = 20;
            EnalbeKJV = true;
        }
         public int Id { get; set; }

        public int FontSize { get; set; }

        public string BibleName { get; set; } //디폴트는 창세기
        public int Chapter { get; set; }    //디폴트 1장
        public int Verse { get; set; }     //디폴트 1절

        public bool EnalbeKJV { get; set; }

        public string UserName { get; set; }
        public string Passwd { get; set; }
    }

    public class Underlining
    {
        public int Id { get; set; }

        public string BibleName { get; set; }

        public int Chapter { get; set; }

        public int Verse { get; set; }

        public string Color { get; set; }
    }
}
