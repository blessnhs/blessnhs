using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

namespace Antioch.Util
{
    public enum BibleType
    {
        KRV = 0,
        NIV = 1,
        KJV = 2
    }

    //성경의 타이틀 정보
    public class BibleTableInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string EngName { get; set; }

        public int MaxChapterSize { get; set; }
        public int MaxVerseSize { get; set; }
    };


    //krv, kjv,niv의 성경 text파일을 읽고 저장하고 있는 클래스
    static public class BibleInfo
    {
        //성경 목록
        static public List<BibleTableInfo> List = new List<BibleTableInfo>();
        //구약성경 목록
        static public List<string> ListOldTestament = new List<string>();
        //신약성경 목록
        static public List<string> ListNewTestament = new List<string>();

        //성경 데이터 저장 dictionary
        static public Dictionary<BibleType, Dictionary<string, Dictionary<int, Dictionary<int, string>>>> bible =
            new Dictionary<BibleType, Dictionary<string, Dictionary<int, Dictionary<int, string>>>>();

        static public void Upsert(BibleType type, string name, int chapter, int verse, string context)
        {
            if (bible.ContainsKey(type) == false)
            {
                bible[type] = new Dictionary<string, Dictionary<int, Dictionary<int, string>>>();
            }

            if (bible[type].ContainsKey(name) == false)
            {
                bible[type][name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[type][name].ContainsKey(chapter) == false)
            {
                bible[type][name][chapter] = new Dictionary<int, string>();
            }

            if (bible[type][name][chapter].ContainsKey(verse) == false)
            {
                bible[type][name][chapter][verse] = "";
            }

            bible[type][name][chapter][verse] += context;
        }

        //성경 본문 가져오기 이름,장,절
        static public string GetContextText(BibleType type, string name, int chapter, int verse)
        {
            if (bible.ContainsKey(type) == false)
            {
                bible[type] = new Dictionary<string, Dictionary<int, Dictionary<int, string>>>();
            }

            if (bible[type].ContainsKey(name) == false)
            {
                bible[type][name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[type][name].ContainsKey(chapter) == false)
            {
                bible[type][name][chapter] = new Dictionary<int, string>();
            }

            if (bible[type][name][chapter].ContainsKey(verse) == false)
            {
                bible[type][name][chapter][verse] = "";
            }

            return bible[type][name][chapter][verse];
        }

        static public int GetChapterSize(string name, BibleType type = BibleType.KRV)
        {
            if (bible.ContainsKey(type) == false)
            {
                bible[type] = new Dictionary<string, Dictionary<int, Dictionary<int, string>>>();
            }

            if (bible[type].ContainsKey(name) == false)
            {
                bible[type][name] = new Dictionary<int, Dictionary<int, string>>();
            }

            return bible[type][name].Count;
        }

        //절의 갯수 가져오기
        static public int GetVerseSize(string name, int chapter, BibleType type = BibleType.KRV)
        {
            if (bible.ContainsKey(type) == false)
            {
                bible[type] = new Dictionary<string, Dictionary<int, Dictionary<int, string>>>();
            }

            if (bible[type].ContainsKey(name) == false)
            {
                bible[type][name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[type][name].ContainsKey(chapter) == false)
            {
                bible[type][name][chapter] = new Dictionary<int, string>();
            }

            return bible[type][name][chapter].Count;
        }

        //성경 목록 전체,신약,구약 불러오기
        static private void LoadBibleList(List<BibleTableInfo> bibleList)
        {
            Stream stream;
            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;

            stream = assembly.GetManifestResourceStream("Antioch.Resource.Oldtestament.txt");
            int idInc = 1;
            using (var reader = new System.IO.StreamReader(stream/*, Encoding.GetEncoding("euc-kr")*/))
            {
                string text;
                while ((text = reader.ReadLine()) != null)
                {
                    string[] words = text.Split(',');

                    var info = new BibleTableInfo();
                    info.Id = idInc++;
#if GLOBAL
                        info.Name = words[1];
#else
                    info.Name = words[0];
#endif
                    info.EngName = words[1];
                    info.MaxChapterSize = Convert.ToInt32(words[2]);
                    bibleList.Add(info);

#if GLOBAL 
                    ListOldTestament.Add(words[1]);
#else
                    ListOldTestament.Add(words[0]);
#endif
                }
            }

            stream = assembly.GetManifestResourceStream("Antioch.Resource.Newtestament.txt");

            using (var reader = new System.IO.StreamReader(stream/*, Encoding.GetEncoding("euc-kr")*/))
            {
                string text;
                while ((text = reader.ReadLine()) != null)
                {
                    string[] words = text.Split(',');

                    var info = new BibleTableInfo();
                    info.Id = idInc++;

#if GLOBAL
                     info.Name = words[1];
#else
                    info.Name = words[0];
#endif
                    info.EngName = words[1];
                    info.MaxChapterSize = Convert.ToInt32(words[2]);
                    bibleList.Add(info);

#if GLOBAL
                    ListNewTestament.Add(words[1]);
#else
                    ListNewTestament.Add(words[0]);
#endif
                }
            }
        }

        static public void CheckValidate()
        {
            foreach (var data in List)
            {
                if (GetChapterSize(data.Name, BibleType.KRV) != GetChapterSize(data.Name, BibleType.KJV))
                    continue;

                for (int i = 1; i <= GetChapterSize(data.Name, BibleType.KRV); i++)
                {
                    if (GetVerseSize(data.Name, i, BibleType.KRV) != GetVerseSize(data.Name, i, BibleType.KJV))
                        continue;

                }


            }
        }

        static public void LoadKJV()
        {
            //이미 불러옴
            if (bible.ContainsKey(BibleType.KJV) == true)
                return;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;

            Stream stream = assembly.GetManifestResourceStream("Antioch.Resource.KJV.txt");
            //1Chr.14:5 And Ibhar, and Elishua, and Elpalet,
            using (var reader = new System.IO.StreamReader(stream))
            {
                int currentIndex = 0;
                string currentBook = "";
                string text;
                int dchapter = 0, dverse = 0;
                while ((text = reader.ReadLine()) != null)
                {
                    try
                    {
                        if (text == "" || text == null)
                            continue;

                        string[] words = text.Split(' ');

                        string[] Header = words[0].Split(':');

                        int verse = dverse = Convert.ToInt32(Header[1]);

                        {

                            string[] result = Header[0].Split('.');

                            if (currentBook == "" || currentBook != result[0])
                            {
                                currentBook = result[0];
                                currentIndex++;
                            }

                            int chapter = dchapter = Convert.ToInt32(result[1]);

                            string str = verse.ToString() + " ";
                            for (int i = 1; i < words.Length; i++)
                            {
                                str += words[i];
                                str += " ";
                            }

                            Upsert(BibleType.KJV, List[currentIndex - 1].Name, chapter, verse, str);
                        }
                    }
                    catch (Exception e)
                    {

                    }

                }
            }
        }


        static public void LoadNIV()
        {
            //이미 불러옴
            if (bible.ContainsKey(BibleType.NIV) == true)
                return;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;
            var list = assembly.GetManifestResourceNames().Where(r => r.StartsWith("Antioch.Resource.NIV") /*&& r.EndsWith(".txt")*/).ToArray();

            int DPrevVerse = 1, DPrevChapter = 1;
            foreach (var book in list)
            {
                using (var reader = new System.IO.StreamReader(assembly.GetManifestResourceStream(book)))
                {
                    try
                    {
                        string[] path = book.Split('.');
                        string bookName = path[path.Length - 2];
                        string text;
                        int PrevVerse = 1, PrevChapter = 1;
                        while ((text = reader.ReadLine()) != null)
                        {
                            if (text == "" || text == null)
                                continue;

                            int pos = text.IndexOf(':');
                            if (pos == -1)
                            {
                                Upsert(BibleType.NIV, book, PrevChapter, PrevVerse, text);
                                continue;
                            }

                            pos = text.IndexOf('+');
                            if (pos != -1)
                            {
                                Upsert(BibleType.NIV, book, PrevChapter, PrevVerse, text);
                                continue;
                            }

                            string[] checkNewLine = text.Split(':');
                            if (checkNewLine == null || checkNewLine.Length < 2)
                            {
                                Upsert(BibleType.NIV, book, PrevChapter, PrevVerse, text);
                                continue;
                            }

                            long number;
                            if (long.TryParse(checkNewLine[0], out number) == false)
                            {
                                Upsert(BibleType.NIV, book, PrevChapter, PrevVerse, text);
                                continue;
                            }


                            string[] words = text.Split(' ');
                            if (words == null)
                                continue;

                            string[] Header = words[0].Split(':');

                            if (Header == null || Header.Length == 0 || Header.Length == 1)
                                continue;

                            if (Header == null || Header[0] == null || Header[1] == null)
                                continue;


                            int chapter = PrevChapter = DPrevChapter = Convert.ToInt32(Header[0]);
                            int verse = PrevVerse = DPrevVerse = Convert.ToInt32(Header[1]);

                            string str = verse.ToString() + " ";
                            for (int i = 1; i < words.Length; i++)
                            {
                                str += words[i];
                                str += " ";
                            }

                            Upsert(BibleType.NIV, bookName, chapter, verse, str);
                        }
                    }
                    catch (Exception e)
                    {

                    }
                }
            }
        }

        //개역한글 불러오기
        static public void LoadKRV()
        {
            //이미 불러옴
            if (bible.ContainsKey(BibleType.KRV) == true)
                return;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;

            LoadBibleList(List);

            Stream stream;

            stream = assembly.GetManifestResourceStream("Antioch.Resource.KRV.txt");

            using (var reader = new System.IO.StreamReader(stream))
            {
                int currentIndex = 0;
                string currentBook = "";
                string text;

                while ((text = reader.ReadLine()) != null)
                {
                    if (text == "")
                        continue;

                    string[] words = text.Split(' ');

                    string[] Header = words[0].Split(':');

                    int verse = Convert.ToInt32(Header[1]);

                    {
                        string pattern = "[0-9]+";
                        string[] result = Regex.Split(Header[0], pattern);

                        if (currentBook == "" || currentBook != result[0])
                        {
                            currentBook = result[0];
                            currentIndex++;
                        }
                    }

                    {
                        string pattern2 = @"\D+";
                        string[] result2 = Regex.Split(Header[0], pattern2);

                        int chapter = Convert.ToInt32(result2[1]);

                        string str = verse.ToString() + " ";
                        for (int i = 1; i < words.Length; i++)
                        {
                            str += words[i];
                            str += " ";
                        }

                        Upsert(BibleType.KRV, List[currentIndex - 1].Name, chapter, verse, str);
                    }

                }
            }
        }
    }

    class Bible
    {
    }
}
