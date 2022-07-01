using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    static public class LanguageTable
    {
        static public Dictionary<string, Dictionary<string, string>> LanguageMap = new Dictionary<string, Dictionary<string, string>>();

        static public void Add(string lang,string type,string msg)
        {
            LanguageMap[lang][type] = msg;
        }

        static public string Get(string lang, string type, string msg)
        {
            return LanguageMap[lang][type];
        }

        static public void Init()
        {
            Add("en", "lobby_match", "R-Matching");
            Add("en", "lobby_record", "Record");
            Add("en", "lobby_rank", "Rank");
            Add("en", "lobby_rank", "Rank");
            Add("en", "lobby_setting", "Setting");
        }

    }
}
