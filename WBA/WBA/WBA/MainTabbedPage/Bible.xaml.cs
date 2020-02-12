using WBA.MainTabbedPage;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Input;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Xamarin.Essentials;

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

            bible[type][name][chapter][verse] = context;
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

        //niv 불러오기
        static public void LoadNIV()
        {
            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;
            var list = assembly.GetManifestResourceNames().Where(r => r.StartsWith("WBA.Resource.NIV") /*&& r.EndsWith(".txt")*/).ToArray();

            foreach( var book in list)
            {
                using (var reader = new System.IO.StreamReader(assembly.GetManifestResourceStream(book)))
                {
                    try
                    {
                        int currentIndex = 0;
                        string currentBook = "";
                        string text;
                        bool FirstLine = false;
                        while ((text = reader.ReadLine()) != null)
                        {
                            if (text == "" || text == null)
                                continue;

                            if (FirstLine == false)
                            {
                                currentBook = text.TrimEnd();
                                FirstLine = true;
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


                            int chapter = Convert.ToInt32(Header[0]);
                            int verse = Convert.ToInt32(Header[1]);
                            string str = verse.ToString() + " ";
                            for (int i = 1; i < words.Length; i++)
                            {
                                str += words[i];
                                str += " ";
                            }

                            Upsert(BibleType.NIV, currentBook, chapter, verse, str);
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

        private int GetBibleInfo(string KRName)
        {
            int index = 0;
            foreach( var bible in BibleInfo.List)
            {
                if (bible.Name == KRName)
                    return index;

                ++index;
            }

            return -1;
        }


        //자동이동을 위해 Label을 dictionary에 저장
        private Dictionary<int, Label> MainTextLabel = new Dictionary<int, Label>();

        public void DrawMainText(StackLayout MainLayout, BibleType type = BibleType.KRV)
        {
            MainTextLabel.Clear();

            var TextLayout = new StackLayout { Orientation = StackOrientation.Vertical, Spacing = 5 };
            int verseSize = BibleInfo.GetVerseSize(SQLLiteDB.CacheData.BibleName, SQLLiteDB.CacheData.Chapter);

            for (int i = 1; i <= verseSize; i++)
            {
                string Text = BibleInfo.GetContextText(type,SQLLiteDB.CacheData.BibleName, SQLLiteDB.CacheData.Chapter, i);

                int index = GetBibleInfo(SQLLiteDB.CacheData.BibleName);

                string TextEnglish = BibleInfo.GetContextText(BibleType.NIV, BibleInfo.List[index]?.EngName, SQLLiteDB.CacheData.Chapter, i);

                var Label = new Label { Text = Text, FontSize = SQLLiteDB.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };
                var LabelEnglish = new Label { Text = TextEnglish, FontSize = SQLLiteDB.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };

                MainTextLabel[i] = Label;

                // Your label tap event
                var forgetPassword_tap = new TapGestureRecognizer();
                forgetPassword_tap.Tapped += async (s, e) =>
                {
                    var labelText = s as Label;

                    string action = await DisplayActionSheet("줄 긋기", "안하기",null,"빨강", "노랑", "파랑");

                    switch(action)
                    {
                        case "빨강":
                            labelText.BackgroundColor = Color.Red;
                            break;
                        case "노랑":
                            labelText.BackgroundColor = Color.Yellow;
                            break;
                        case "파랑":
                            labelText.BackgroundColor = Color.Green;
                            break;
                    }

                };

                Label.GestureRecognizers.Add(forgetPassword_tap);

                TextLayout.Children.Add(Label);

                if(SQLLiteDB.CacheData.EnalbeNIV == true)
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
            Title = SQLLiteDB.CacheData.BibleName + " " + SQLLiteDB.CacheData.Chapter.ToString() + "장  KRV";

            var MainLayout = new StackLayout { Padding = new Thickness(5, 10) };

            DrawTopButton(MainLayout);
            DrawMainText(MainLayout);
            DrawBottomButton(MainLayout);

            SQLLiteDB.Upsert(SQLLiteDB.CacheData);

            ScrollView scrollView = new ScrollView();
            scrollView.Content = MainLayout;

            Content = scrollView;
            GotoFocusLabel(scrollView, SQLLiteDB.CacheData.Verse);        
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
            await ShareText(SQLLiteDB.CacheData.BibleName +" " + SQLLiteDB.CacheData.Chapter + "장 까지 읽었습니다.");
        }

        //다음 성경 가져오기
        async void Handle_Clicked_Next(object sender, System.EventArgs e)
        {
            int CurrentMaxChapter = BibleInfo.GetChapterSize(SQLLiteDB.CacheData.BibleName);

            if(SQLLiteDB.CacheData.Chapter + 1 <= CurrentMaxChapter)
            {
                SQLLiteDB.CacheData.Chapter += 1;

                RefreshData();
            }
            else
            {
                //다음 성경 
               string NextBible = SQLLiteDB.CacheData.BibleName;
               int currentPos = 0;
               foreach( var bible in BibleInfo.List)
               {
                    if (bible.Name == SQLLiteDB.CacheData.BibleName)
                        break;

                    currentPos++;
               }

               if(BibleInfo.List.Count > currentPos + 1)
                {
                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos + 1].Name;
                    SQLLiteDB.CacheData.Chapter = 1;
                    SQLLiteDB.CacheData.Verse = 1;

                    RefreshData();
                }
            }
        }
        //이전 성경 가져오기
        async void Handle_Clicked_Prev(object sender, System.EventArgs e)
        {
            int CurrentMaxChapter = BibleInfo.GetChapterSize(SQLLiteDB.CacheData.BibleName);

            //최소 1장보다 커야한다. 
            if (SQLLiteDB.CacheData.Chapter - 1 < 0)
            {
                SQLLiteDB.CacheData.Chapter -= 1;

                RefreshData();
            }
            else
            {
                //다음 성경 
                string NextBible = SQLLiteDB.CacheData.BibleName;
                int currentPos = 0;
                foreach (var bible in BibleInfo.List)
                {
                    if (bible.Name == SQLLiteDB.CacheData.BibleName)
                        break;

                    currentPos++;
                }

                if (0 <= ( currentPos - 1) && SQLLiteDB.CacheData.Chapter == 1)
                {
                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos - 1].Name;
                    SQLLiteDB.CacheData.Chapter = BibleInfo.GetChapterSize(SQLLiteDB.CacheData.BibleName);
                    SQLLiteDB.CacheData.Verse = 1;
                }
                else
                {
                    int chapter = SQLLiteDB.CacheData.Chapter - 1;
                    if (chapter < 1)
                        chapter = 1;

                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos].Name;
                    SQLLiteDB.CacheData.Chapter = chapter;
                    SQLLiteDB.CacheData.Verse = 1;
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