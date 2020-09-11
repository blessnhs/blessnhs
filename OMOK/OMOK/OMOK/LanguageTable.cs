using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public class LanguageTable
    {
        public Dictionary<string, Dictionary<string, string>> LanguageMap = new Dictionary<string, Dictionary<string, string>>();
     
        public void Add(string lang,string type,string msg)
        {
            LanguageMap[lang][type] = msg;
        }

        public string Get(string lang, string type, string msg)
        {
            return LanguageMap[lang][type];
        }

    }
}
