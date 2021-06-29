using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;


namespace Antioch.Util
{
    static public class Dic
    {
        static public Dictionary<string, string> _dictionary = new Dictionary<string, string>();
        static public void LoadDic()
        {
            if (_dictionary.ContainsKey("A") == true)
                return;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(Dic)).Assembly;
            var list = assembly.GetManifestResourceNames().Where(r => r.StartsWith("Antioch.Resource.Dic") /*&& r.EndsWith(".txt")*/).ToArray();

            foreach (var book in list)
            {
                using (var reader = new System.IO.StreamReader(assembly.GetManifestResourceStream(book)))
                {
                    try
                    {
                        string text;
                        while ((text = reader.ReadLine()) != null)
                        {
                            if (text == "" || text == null)
                                continue;

                            string[] words = text.Split(':');
                            if (words == null || words.Length < 2)
                            {
                                words = text.Split('[');
                                if (words == null || words.Length < 2)
                                    continue;
                            }

                            string word = words[0].TrimEnd();

                            _dictionary[word] = words[1];
                        }
                    }
                    catch (Exception e)
                    {

                    }
                }
            }
        }

    }
}
