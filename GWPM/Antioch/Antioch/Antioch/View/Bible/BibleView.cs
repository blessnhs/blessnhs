using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Xamarin.Essentials;
using Antioch.Util;

namespace Antioch
{

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class BibleView : ContentView
    {
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
#if GLOBAL
        public void DrawMainText(StackLayout MainLayout, BibleType type = BibleType.KRV)
#else
        public void DrawMainText(StackLayout MainLayout, BibleType type = BibleType.KRV)
#endif
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
                    //try
                    //{
                    //    var labelText = s as Label;

                    //    string Context = labelText.Text;

                    //    string[] words = Context.Split(' ');

                    //    Dictionary<string, string> help = new Dictionary<string, string>();
                    //    if (words != null || words.Length > 0)
                    //    {
                    //        for (int k = 0; k < words.Length; k++)
                    //        {
                    //            words[k] = words[k].ToLower();
                    //            string outstr;
                    //            if (Dic._dictionary.TryGetValue(words[k], out outstr) == true)
                    //            {
                    //                help[words[k]] = outstr;
                    //            }
                    //            else
                    //            {
                    //                if (words[k].Length > 3)
                    //                {
                    //                    //끝에 하나버림
                    //                    string sub1 = words[k].Substring(0, words[k].Length - 1);
                    //                    if (Dic._dictionary.TryGetValue(sub1, out outstr) == true)
                    //                    {
                    //                        help[sub1] = outstr;
                                        
                    //                    }
                    //                    else
                    //                    {
                    //                        //끝에 두개버림
                    //                        sub1 = words[k].Substring(0, words[k].Length - 2);
                    //                        if (Dic._dictionary.TryGetValue(sub1, out outstr) == true)
                    //                        {
                    //                            help[sub1] = outstr;
                    //                        }
                    //                    }
                    //                    }

                    //                }
                    //            }
                    //        }
                    //    }

                    //    string context = "";
                    //    foreach( var str in help)
                    //    {
                    //        context += str.Key;
                    //        context += " : ";
                    //        context += str.Value;
                    //        context += "\n\n";
                    //    }

                    //    await App.Current.MainPage.DisplayAlert("Help", context, "OK");
                    //}
                    //catch (Exception)
                    //{

                    //}

                };

                var labeltab = new TapGestureRecognizer();
                labeltab.Tapped += async (s, e) =>
                {
                    var labelText = s as Label;

                    try
                    {
                        string action = await App.Current.MainPage.DisplayActionSheet("Line", "cancel", null, "red", "yellow", "blue");

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
                            case "red":
                                labelText.BackgroundColor = Color.Red;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "빨강");
                                break;
                            case "yellow":
                                labelText.BackgroundColor = Color.Yellow;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "노랑");
                                break;
                            case "blue":
                                labelText.BackgroundColor = Color.Green;
                                SQLLiteDB.InsertUnderlining(User.CacheData.BibleName, User.CacheData.Chapter, iverse, "파랑");
                                break;
                            case "cancel":
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


            var LabelEndLine0 = new Label { Text = "", FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Center };
            TextLayout.Children.Add(LabelEndLine0);

            var LabelEndLine1 = new Label { Text = "성경전서 개역한글판의 저작권은", FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Center };
            TextLayout.Children.Add(LabelEndLine1);

            var LabelEndLine2 = new Label { Text = "( 재 ) 대한성서공회에 있습니다", FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Center };
            TextLayout.Children.Add(LabelEndLine2);

            var LabelEndLine3 = new Label { Text = "", FontSize = User.CacheData.FontSize, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };
            TextLayout.Children.Add(LabelEndLine3);


            MainLayout.Children.Add(TextLayout);

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
#if GLOBAL
            OldT.Text = "OldT";
            NewT.Text = "NewT";

            Title.Text = User.CacheData.BibleName + " " + User.CacheData.Chapter.ToString() + "Chapter";
#else
            OldT.Text = "구약";
            NewT.Text = "신약";
            Title.Text = User.CacheData.BibleName + " " + User.CacheData.Chapter.ToString() + "장";
#endif
            MainLayout.Children.Clear();
            DrawMainText(MainLayout);
     
            SQLLiteDB.Upsert(User.CacheData);
 
            GotoFocusLabel(scrollView, User.CacheData.Verse);        
        }

        public BibleView()
        {
            InitializeComponent();

            RefreshData();
        }

        //구약 성경 버튼 클릭
        async void Handle_Clicked_OldTestament(object sender, System.EventArgs e)
        {
            var grid = this.Parent as Grid;

            PageOldTestament bible = new PageOldTestament();
            grid.Children.Clear();
            grid.Children.Add(bible);
        }

        //신약 성경 버튼 클릭
        async void Handle_Clicked_NewTestament(object sender, System.EventArgs e)
        {
            var grid = this.Parent as Grid;

            PageNewTestament bible = new PageNewTestament();
            grid.Children.Clear();
            grid.Children.Add(bible);
        }

        //공유 버튼 클릭
        async void Handle_Clicked_Shared(object sender, System.EventArgs e)
        {
#if GLOBAL
            await ShareText(User.CacheData.BibleName +" " + User.CacheData.Chapter + "Chapter Read.");
#else
            await ShareText(User.CacheData.BibleName + " " + User.CacheData.Chapter + "장 까지 읽었습니다.");
#endif
        }

        //다음 성경 가져오기
        async void Handle_Clicked_Next(object sender, System.EventArgs e)
        {
            int CurrentMaxChapter = BibleInfo.GetChapterSize(User.CacheData.BibleName);

            if(User.CacheData.Chapter + 1 <= CurrentMaxChapter)
            {
                User.CacheData.Chapter += 1;

                User.CacheData.Verse = 1;

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
                Title = "Bible Read Share"
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