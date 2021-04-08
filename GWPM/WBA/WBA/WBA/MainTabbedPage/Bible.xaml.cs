using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Xamarin.Essentials;
using WBA.MainTabbedPage;

namespace WBA
{
    //성경 타입
    public enum BibleType
    {
        KRV  = 0,
        NIV  = 1,
        KJV  = 2
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
        static public Dictionary<BibleType,Dictionary<string, Dictionary<int, Dictionary<int, string>>>> bible = 
            new Dictionary<BibleType, Dictionary<string, Dictionary<int, Dictionary<int, string>>>>();

        static public void Upsert(BibleType type,string name, int chapter, int verse, string context)
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
        static public string GetContextText(BibleType type,string name, int chapter, int verse)
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
        static public int GetVerseSize(string name,int chapter, BibleType type = BibleType.KRV)
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

            stream = assembly.GetManifestResourceStream("WBA.Resource.Oldtestament.txt");
            int idInc = 1;
            using (var reader = new System.IO.StreamReader(stream/*, Encoding.GetEncoding("euc-kr")*/))
            {
                string text;
                while ((text = reader.ReadLine()) != null)
                {
                    string[] words = text.Split(',');

                    var info = new BibleTableInfo();
                    info.Id = idInc++;
                    info.Name = words[0];
                    info.EngName = words[1];
                    info.MaxChapterSize = Convert.ToInt32(words[2]);
                    bibleList.Add(info);
                    ListOldTestament.Add(words[0]);
                }
            }

            stream = assembly.GetManifestResourceStream("WBA.Resource.Newtestament.txt");

            using (var reader = new System.IO.StreamReader(stream/*, Encoding.GetEncoding("euc-kr")*/))
            {
                string text;
                while ((text = reader.ReadLine()) != null)
                {
                    string[] words = text.Split(',');

                    var info = new BibleTableInfo();
                    info.Id = idInc++;
                    info.Name = words[0];
                    info.EngName = words[1];
                    info.MaxChapterSize = Convert.ToInt32(words[2]);
                    bibleList.Add(info);
                    ListNewTestament.Add(words[0]);
                }
            }
        }

        static public void CheckValidate()
        {
            foreach(var data in List)
            {
                if (GetChapterSize(data.Name, BibleType.KRV) != GetChapterSize(data.Name, BibleType.KJV))
                    continue;

                for( int i=1;i<= GetChapterSize(data.Name, BibleType.KRV);i++)
                {
                    if (GetVerseSize(data.Name,i, BibleType.KRV) != GetVerseSize(data.Name, i, BibleType.KJV))
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
        
            Stream stream = assembly.GetManifestResourceStream("WBA.Resource.KJV.txt");
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

                            int chapter =  dchapter = Convert.ToInt32(result[1]);

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
            var list = assembly.GetManifestResourceNames().Where(r => r.StartsWith("WBA.Resource.NIV") /*&& r.EndsWith(".txt")*/).ToArray();

            int DPrevVerse = 1, DPrevChapter = 1;
            foreach (var book in list)
            {
                using (var reader = new System.IO.StreamReader(assembly.GetManifestResourceStream(book)))
                {
                    try
                    {
                        string[] path = book.Split('.');
                        string bookName = path[path.Length-2];
                        string text;
                        int PrevVerse = 1,PrevChapter = 1;
                        while ((text = reader.ReadLine()) != null)
                        {
                            if (text == "" || text == null)
                                continue;

                            int pos = text.IndexOf(':');
                            if(pos == -1)
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
                            if(checkNewLine == null || checkNewLine.Length < 2)
                            {
                                Upsert(BibleType.NIV, book, PrevChapter, PrevVerse, text);
                                continue;
                            }

                            long number;
                            if(long.TryParse(checkNewLine[0], out number) == false)
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
                return ;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;
      
            LoadBibleList(List);
   
            Stream stream;
    
            stream = assembly.GetManifestResourceStream("WBA.Resource.KRV.txt");

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

                        Upsert(BibleType.KRV,List[currentIndex - 1].Name, chapter, verse, str);
                    }

                }
            }
        }
    }


    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Bible : ContentPage
    {
        protected override void OnAppearing()
        {
            RefreshData();
        }

        public void DrawTopButton(StackLayout MainLayout)
        {
            var ButtonLayout = new StackLayout { Orientation = StackOrientation.Horizontal };

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += Handle_Clicked_Prev;

            Button buttonOld = new Button { Text = "구약성경", HorizontalOptions = LayoutOptions.Start };
            buttonOld.Clicked += Handle_Clicked_OldTestament;

            Button buttonNew = new Button { Text = "신약성경", HorizontalOptions = LayoutOptions.End };
            buttonNew.Clicked += Handle_Clicked_NewTestament;


            Button NextBtn = new Button { Text = "▷", HorizontalOptions = LayoutOptions.End };
            NextBtn.Clicked += Handle_Clicked_Next;


            ButtonLayout.Children.Add(PrevBtn);

            ButtonLayout.Children.Add(buttonOld);
            ButtonLayout.Children.Add(buttonNew);

            ButtonLayout.Children.Add(NextBtn);

            {
                var tapGestureRecognizer = new TapGestureRecognizer();
                tapGestureRecognizer.Tapped += (s, e) => {


                };

                MainLayout.GestureRecognizers.Add(tapGestureRecognizer);

            }

            MainLayout.Children.Add(ButtonLayout);
        }

        private void CheckUnderLine(List<Underlining> list,Label labelText,int verse)
        {
            if(list != null)
            {
                foreach (var data in list)
                {
                    if(data.BibleName == User.CacheData.BibleName && data.Chapter == User.CacheData.Chapter && data.Verse == verse)
                    {

                        switch (data.Color)
                        {
                            case "빨강":
                                labelText.BackgroundColor = Color.Red;
                                break;
                            case "노랑":
                                labelText.BackgroundColor = Color.Yellow;
                                break;
                            case "녹색":
                                labelText.BackgroundColor = Color.Green;
                                break;
                        }
                    }
                }
            }
        }

        //자동이동을 위해 Label을 dictionary에 저장
        private Dictionary<int, Label> MainTextLabel = new Dictionary<int, Label>();

        public void DrawMainText(StackLayout MainLayout, BibleType type = BibleType.KRV)
        {
            //db에 밑줄 저장 데이터가 있는지 로딩한다. 
            var list = SQLLiteDB.ReadUnderlining();

            MainTextLabel.Clear();

            var TextLayout = new StackLayout { Orientation = StackOrientation.Vertical, Spacing = 5 };
            int verseSize = BibleInfo.GetVerseSize(User.CacheData.BibleName, User.CacheData.Chapter);
            int verseSizeEng = BibleInfo.GetVerseSize(User.CacheData.BibleName, User.CacheData.Chapter,BibleType.KJV);

            int size = verseSize > verseSizeEng ? verseSize : verseSizeEng;

            for (int i = 1; i <= size; i++)
            {
                string Text = BibleInfo.GetContextText(type, User.CacheData.BibleName, User.CacheData.Chapter, i);

                string TextEnglish = BibleInfo.GetContextText(BibleType.KJV, User.CacheData.BibleName, User.CacheData.Chapter, i);

                var Label = new Label { Text = Text, FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };
                var LabelEnglish = new Label { Text = TextEnglish, FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };

                //SQL DB에 저장되어 있는 데이터가 존재하면 해당 색으로 배경을 변경한다.
                CheckUnderLine(list, Label,i);

                //자동이동을 위해 따로 저장한다.
                MainTextLabel[i] = Label;

                // Your label tap event
                var englishlabeltab = new TapGestureRecognizer();
                englishlabeltab.Tapped += async (s, e) =>
                {
                    try
                    {
                        if (User.Username != "강병욱")
                            return;

                        var labelText = s as Label;

                        string Context = labelText.Text;

                        string[] words = Context.Split(' ');

                        Dictionary<string, string> help = new Dictionary<string, string>();
                        if (words != null || words.Length > 0)
                        {
                            for (int k = 0; k < words.Length; k++)
                            {
                                words[k] = words[k].ToLower();
                                string outstr;
                                if (Dic._dictionary.TryGetValue(words[k], out outstr) == true)
                                {
                                    help[words[k]] = outstr;
                                }
                                else
                                {
                                    if (words[k].Length > 3)
                                    {
                                        //끝에 하나버림
                                        string sub1 = words[k].Substring(0, words[k].Length - 1);
                                        if (Dic._dictionary.TryGetValue(sub1, out outstr) == true)
                                        {
                                            help[sub1] = outstr;
                                        
                                        }
                                        else
                                        {
                                            //끝에 두개버림
                                            sub1 = words[k].Substring(0, words[k].Length - 2);
                                            if (Dic._dictionary.TryGetValue(sub1, out outstr) == true)
                                            {
                                                help[sub1] = outstr;
                                            }
                                        }

                                    }
                                }
                            }
                        }

                        string context = "";
                        foreach( var str in help)
                        {
                            context += str.Key;
                            context += " : ";
                            context += str.Value;
                            context += "\n\n";
                        }

                        await DisplayAlert("Help", context, "OK");
                    }
                    catch (Exception)
                    {

                    }

                };

                var labeltab = new TapGestureRecognizer();
                labeltab.Tapped += async (s, e) =>
                {
                    var labelText = s as Label;

                    try
                    {
                        string action = await DisplayActionSheet("줄긋기", "안하기", null, "빨강", "노랑", "파랑");

                        //선택한 라벨의 절수를 가져온다.
                        int iverse = 1;
                        string verse = labelText.Text;
                        if (verse != null)
                        {
                            string[] header = verse.Split(' ');
                            if (header.Length > 0)
                            {
                                iverse = Convert.ToInt32(header[0]);
                            }
                        }

                        switch (action)
                        {
                            case "빨강":
                                labelText.BackgroundColor = Color.Red;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "빨강");
                                break;
                            case "노랑":
                                labelText.BackgroundColor = Color.Yellow;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "노랑");
                                break;
                            case "파랑":
                                labelText.BackgroundColor = Color.Green;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "파랑");
                                break;
                            case "안하기":
                                labelText.BackgroundColor = Color.White;
                                SQLLiteDB.DeleteUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "");
                                break;
                        }
                    }
                    catch(Exception)
                    {

                    }
                };

                Label.GestureRecognizers.Add(labeltab);
                LabelEnglish.GestureRecognizers.Add(englishlabeltab);

                TextLayout.Children.Add(Label);

                if(User.CacheData.EnalbeKJV == true)
                    TextLayout.Children.Add(LabelEnglish);
                
            }
            MainLayout.Children.Add(TextLayout);

        }

        //하단 ui그리기
        public void DrawBottomButton(StackLayout MainLayout)
        {
            var ButtonLayout = new StackLayout { Orientation = StackOrientation.Horizontal };

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += Handle_Clicked_Prev;

            Button NextBtn = new Button { Text = "▷", HorizontalOptions = LayoutOptions.End };
            NextBtn.Clicked += Handle_Clicked_Next;

            Button SharedBtn = new Button { Text = "공유", HorizontalOptions = LayoutOptions.End };
            SharedBtn.Clicked += Handle_Clicked_Shared;


            ButtonLayout.Children.Add(PrevBtn);
            ButtonLayout.Children.Add(NextBtn);

            ButtonLayout.Children.Add(SharedBtn);

            MainLayout.Children.Add(ButtonLayout);
        }

        //선택한 절로 이동하기
        private void GotoFocusLabel(ScrollView scrollView,int Verse)
        {
            Label Focus = null;
            if(MainTextLabel.TryGetValue(Verse, out Focus) == true)
            {
                scrollView.ScrollToAsync(Focus, ScrollToPosition.Start, true);
            }
        }

        //ui 갱신
        public void RefreshData()
        {
            Title = User.CacheData.BibleName + " " + User.CacheData.Chapter.ToString() + "장  KRV";

            var MainLayout = new StackLayout { Padding = new Thickness(5, 10) };

            DrawTopButton(MainLayout);
            DrawMainText(MainLayout);
            DrawBottomButton(MainLayout);

            SQLLiteDB.Upsert(User.CacheData);

            ScrollView scrollView = new ScrollView();
            scrollView.Content = MainLayout;

            Content = scrollView;
            GotoFocusLabel(scrollView, User.CacheData.Verse);        
        }

        public Bible()
        {
            RefreshData();

        }

        //구약 성경 버튼 클릭
        async void Handle_Clicked_OldTestament(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new PageOldTestament());
        }

        //신약 성경 버튼 클릭
        async void Handle_Clicked_NewTestament(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new PageNewTestament());
        }

        //공유 버튼 클릭
        async void Handle_Clicked_Shared(object sender, System.EventArgs e)
        {
            await ShareText(User.CacheData.BibleName +" " + User.CacheData.Chapter + "장 까지 읽었습니다.");
        }

        //다음 성경 가져오기
        async void Handle_Clicked_Next(object sender, System.EventArgs e)
        {
            int CurrentMaxChapter = BibleInfo.GetChapterSize(User.CacheData.BibleName);

            if(User.CacheData.Chapter + 1 <= CurrentMaxChapter)
            {
                User.CacheData.Chapter += 1;

                RefreshData();
            }
            else
            {
                //다음 성경 
               string NextBible = User.CacheData.BibleName;
               int currentPos = 0;
               foreach( var bible in BibleInfo.List)
               {
                    if (bible.Name == User.CacheData.BibleName)
                        break;

                    currentPos++;
               }

               if(BibleInfo.List.Count > currentPos + 1)
                {
                    User.CacheData.BibleName = BibleInfo.List[currentPos + 1].Name;
                    User.CacheData.Chapter = 1;
                    User.CacheData.Verse = 1;

                    RefreshData();
                }
            }
        }
        //이전 성경 가져오기
        async void Handle_Clicked_Prev(object sender, System.EventArgs e)
        {
            int CurrentMaxChapter = BibleInfo.GetChapterSize(User.CacheData.BibleName);

            //최소 1장보다 커야한다. 
            if (User.CacheData.Chapter - 1 < 0)
            {
                User.CacheData.Chapter -= 1;

                RefreshData();
            }
            else
            {
                //다음 성경 
                string NextBible = User.CacheData.BibleName;
                int currentPos = 0;
                foreach (var bible in BibleInfo.List)
                {
                    if (bible.Name == User.CacheData.BibleName)
                        break;

                    currentPos++;
                }

                if (0 <= ( currentPos - 1) && User.CacheData.Chapter == 1)
                {
                    User.CacheData.BibleName = BibleInfo.List[currentPos - 1].Name;
                    User.CacheData.Chapter = BibleInfo.GetChapterSize(User.CacheData.BibleName);
                    User.CacheData.Verse = 1;
                }
                else
                {
                    int chapter = User.CacheData.Chapter - 1;
                    if (chapter < 1)
                        chapter = 1;

                    User.CacheData.BibleName = BibleInfo.List[currentPos].Name;
                    User.CacheData.Chapter = chapter;
                    User.CacheData.Verse = 1;
                }

                RefreshData();
            }
        }

        public async Task ShareText(string text)
        {
            await Share.RequestAsync(new ShareTextRequest
            {
                Text = text,
                Title = "성경읽기 공유"
            });
        }

        public async Task ShareUri(string uri)
        {
            await Share.RequestAsync(new ShareTextRequest
            {
                Uri = uri,
                Title = "Share Web Link"
            });
        }
    }


}