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
    public class BibleTableInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string EngName { get; set; }

        public int MaxChapterSize { get; set; }
        public int MaxVerseSize { get; set; }
    };

    static public class BibleInfo
    {
        static public List<string> List = new List<string>();
        static public List<string> ListOldTestament = new List<string>();
        static public List<string> ListNewTestament = new List<string>();

        static public Dictionary<string, Dictionary<int, Dictionary<int, string>>> bible = new Dictionary<string, Dictionary<int, Dictionary<int, string>>>();

        static public void Upsert(string name, int chapter, int verse, string context)
        {
            if (bible.ContainsKey(name) == false)
            {
                bible[name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[name].ContainsKey(chapter) == false)
            {
                bible[name][chapter] = new Dictionary<int, string>();
            }

            bible[name][chapter][verse] = context;
        }

        static public string GetContextText(string name, int chapter, int verse)
        {
            if (bible.ContainsKey(name) == false)
            {
                bible[name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[name].ContainsKey(chapter) == false)
            {
                bible[name][chapter] = new Dictionary<int, string>();
            }

            return bible[name][chapter][verse];
        }
        
        static public int GetChapterSize(string name)
        {
            if (bible.ContainsKey(name) == false)
            {
                bible[name] = new Dictionary<int, Dictionary<int, string>>();
            }

            return bible[name].Count;
        }

        static public int GetVerseSize(string name,int chapter)
        {
            if (bible.ContainsKey(name) == false)
            {
                bible[name] = new Dictionary<int, Dictionary<int, string>>();
            }

            if (bible[name].ContainsKey(chapter) == false)
            {
                bible[name][chapter] = new Dictionary<int, string>();
            }

            return bible[name][chapter].Count;
        }

        static private void LoadBibleList(List<string> bibleList)
        {
            Stream stream;
            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;

            stream = assembly.GetManifestResourceStream("WBA.Resource.Oldtestament.txt");

            using (var reader = new System.IO.StreamReader(stream/*, Encoding.GetEncoding("euc-kr")*/))
            {
                string text;
                while ((text = reader.ReadLine()) != null)
                {
                    string[] words = text.Split(',');

                    bibleList.Add(words[0]);
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

                    bibleList.Add(words[0]);
                    ListNewTestament.Add(words[0]);
                }
            }
        }

        static public void LoadKRV()
        {
            LoadBibleList(List);
   
            Stream stream;
            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;

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

                        Upsert(List[currentIndex - 1], chapter, verse, str);
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

        public void DrawMainText(StackLayout MainLayout)
        {
            var TextLayout = new StackLayout { Orientation = StackOrientation.Vertical, Spacing = 5 };
            int verseSize = BibleInfo.GetVerseSize(SQLLiteDB.CacheData.BibleName, SQLLiteDB.CacheData.Chapter);

            for (int i = 1; i <= verseSize; i++)
            {
                string Text = BibleInfo.GetContextText(SQLLiteDB.CacheData.BibleName, SQLLiteDB.CacheData.Chapter, i);

                var Label = new Label { Text = Text, FontSize = SQLLiteDB.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };

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
            }
            MainLayout.Children.Add(TextLayout);

        }

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

        public void RefreshData()
        {
            Title = SQLLiteDB.CacheData.BibleName + " " + SQLLiteDB.CacheData.Chapter.ToString() + "장  KRV";

            var MainLayout = new StackLayout { Padding = new Thickness(5, 10) };

            DrawTopButton(MainLayout);
            DrawMainText(MainLayout);
            DrawBottomButton(MainLayout);

            UserCacheData data = new UserCacheData();
            data.FontSize = SQLLiteDB.FontSize;
            data.Id = 1;
            data.BibleName = SQLLiteDB.CacheData.BibleName;
            data.Chapter = SQLLiteDB.CacheData.Chapter;
            data.Verse = SQLLiteDB.CacheData.Verse;
            data.UserName = SQLLiteDB.CacheData.UserName;
            data.Passwd = SQLLiteDB.CacheData.Passwd;

            SQLLiteDB.Upsert(data);

            ScrollView scrollView = new ScrollView();
            scrollView.Content = MainLayout;
              
            Content = scrollView;
        }

        public Bible()
        {
            RefreshData();

        }

        async void Handle_Clicked_OldTestament(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new PageOldTestament());
        }

        async void Handle_Clicked_NewTestament(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new PageNewTestament());
        }

        async void Handle_Clicked_Shared(object sender, System.EventArgs e)
        {
            await ShareText(SQLLiteDB.CacheData.BibleName +" " + SQLLiteDB.CacheData.Chapter + "장 까지 읽었습니다.");
        }

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
                    if (bible == SQLLiteDB.CacheData.BibleName)
                        break;

                    currentPos++;
               }

               if(BibleInfo.List.Count > currentPos + 1)
                {
                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos + 1];
                    SQLLiteDB.CacheData.Chapter = 1;
                    SQLLiteDB.CacheData.Verse = 1;

                    RefreshData();
                }
            }
        }

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
                    if (bible == SQLLiteDB.CacheData.BibleName)
                        break;

                    currentPos++;
                }

                if (0 <= ( currentPos - 1) && SQLLiteDB.CacheData.Chapter == 1)
                {
                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos - 1];
                    SQLLiteDB.CacheData.Chapter = BibleInfo.GetChapterSize(SQLLiteDB.CacheData.BibleName);
                    SQLLiteDB.CacheData.Verse = 1;
                }
                else
                {
                    int chapter = SQLLiteDB.CacheData.Chapter - 1;
                    if (chapter < 1)
                        chapter = 1;

                    SQLLiteDB.CacheData.BibleName = BibleInfo.List[currentPos];
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