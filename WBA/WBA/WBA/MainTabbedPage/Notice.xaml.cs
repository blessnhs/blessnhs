using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using System.IO;
using WBA.MainTabbedPage;
using System.Linq;

namespace WBA
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Notice : ContentPage
    {

        public class MyPosToBibleRead
        {
            public string begin_bibleName;
            public int begin_chapter;

            public string end_bibleName;
            public int end_chapter;
        }

        public DateTime UtcToStandardTime()
        {
            DateTime currentTime = DateTime.UtcNow;
            return TimeZoneInfo.ConvertTimeBySystemTimeZoneId(currentTime, "Asia/Seoul");
        }
        
        public DateTime WeekDateTime(DateTime dt, DayOfWeek startOfWeek)
        {
            dt = UtcToStandardTime();

            int diff = (7 + (dt.DayOfWeek - startOfWeek)) % 7;
            return dt.AddDays(-1 * diff).Date;
        }

        private static void GetNextPos(string BibleName, int Chapter, out string outbible, out int chapter)
        {
            outbible = "종료";
            chapter = 0;

            int CurrentMaxChapter = BibleInfo.GetChapterSize(BibleName);

            if (Chapter + 1 <= CurrentMaxChapter)
            {
                Chapter += 1;
                outbible = BibleName;
                chapter = Chapter;
            }
            else
            {
                //다음 성경 
                string NextBible = BibleName;
                int currentPos = 0;
                foreach (var bible in BibleInfo.List)
                {
                    if (bible.Name == BibleName)
                        break;

                    currentPos++;
                }

                if (BibleInfo.List.Count > currentPos + 1)
                {
                    outbible = BibleInfo.List[currentPos + 1].Name;
                    chapter = 1;
                }
            }
        }

        private MyPosToBibleRead CalculateTodayBibleChapter(int addDay = 0)
        {
            MyPosToBibleRead readpos = new MyPosToBibleRead();

            var data = SQLLiteDB.ReadBibleReadPlan();
            if(data == null)
            {
                return null;
            }

            DateTime BeginTime = data.StartTime;

            DateTime MondayTime = WeekDateTime(DateTime.Now, DayOfWeek.Monday);

            MondayTime = MondayTime.AddDays(addDay);

            if (data.StartTime > MondayTime)
            {
                return null;
            }

            int DiffDay = (MondayTime - BeginTime).Days;

            var search = BibleInfo.List.Find(e => e.Name == data.BibleName);
            if (search == null)
                return null;

            int accChapterSize = 0;
            foreach (var bible in BibleInfo.List)
            {
                if (search.Id > bible.Id)
                    continue;                

          
                int chapter = BibleInfo.GetChapterSize(bible.Name);
  
                int currChapter = DiffDay * data.Count;
                accChapterSize += chapter;


                if (currChapter + 1 <= accChapterSize)
                {
                    readpos.begin_bibleName = bible.Name;
                    readpos.begin_chapter = Math.Abs((accChapterSize - currChapter) - chapter) + 1;
             
                    string outbible = "";
                    int outchapter = 1;

                    string inbible = readpos.begin_bibleName;
                    int inchapter = readpos.begin_chapter;

                    for (int i = 0; i < data.Count - 1; i++)
                    {
                        GetNextPos(inbible, inchapter, out outbible, out outchapter);

                        inbible = outbible;
                        inchapter = outchapter;
                    }

                    readpos.end_bibleName = outbible;
                    readpos.end_chapter = outchapter;
                    break;
                }

            }

            return readpos;

        }


        private void SetJuboLabel()
        {
            // Your label tap event
            var forgetPassword_tap = new TapGestureRecognizer();
            forgetPassword_tap.Tapped += async (s, e) =>
            {
                var labelText = s as Label;

                Device.OpenUri(new Uri("http://www.antiochi.net/jubo.pdf"));
            };

            label_jubo.GestureRecognizers.Add(forgetPassword_tap);
        }

        private void SetNoticeLabel()
        {
            string Message = "";
            try
            {
                var NoticeFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "notice.txt");
                using (var reader = new StreamReader(NoticeFile, true))
                {
                    if (reader == null)
                        return;

                    string line;
                    while ((line = reader.ReadLine()) != null)
                    {
                        Message += line;
                        Message += "\n";
                    }
                }
            }
            catch (Exception e)
            {
               
            }

            notify.Text = Message;
        }
      
        private void SetWorshipLabel()
        {
            string Message = "";
            try
            { 
                var WorshipFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "worship.txt");
                using (var reader = new StreamReader(WorshipFile, true))
                {
                    if (reader == null)
                        return;

                    string line;
                    while ((line = reader.ReadLine()) != null)
                    {
                        Message += line;
                        Message += "\n";
                    }
                }
            }
            catch (Exception e)
            {
               
            }

            worship.Text = Message;
        }
        private void Set2020Message()
        {
           //  Your label tap event
            var forgetPassword_tap = new TapGestureRecognizer();
            forgetPassword_tap.Tapped += async (s, e) =>
            {
                var labelText = s as Label;

            //    Navigation.PushModalAsync(new TextView("WBA.Resource.Message2020.txt"));
            };

            label_2020msg.GestureRecognizers.Add(forgetPassword_tap);
        }

        private void SetWeeklyReadTable()
        {
            try
            {
                Label[] weekLabel = new Label[7];

                weekLabel[0] = label_mon;
                weekLabel[1] = label_tue;
                weekLabel[2] = label_wed;
                weekLabel[3] = label_thu;
                weekLabel[4] = label_fri;
                weekLabel[5] = label_sat;
                weekLabel[6] = label_sun;

                string[] week = { "월", "화", "수", "목", "금", "토", "일" };
                for (int i = 0; i < 7; i++)
                {
                    var info = CalculateTodayBibleChapter(i);
                    if (info == null)
                    {
                        weekLabel[i].Text = week[i] + " ";
                        continue;
                    }

                    string Text = week[i] + " " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " 장" + "~ " + info.end_bibleName + " " + info.end_chapter.ToString() + " 장";

                    //한장씩 읽을때는 end_bibleName 없으므로 시작 만 출력해준다. 
                    if (info.end_bibleName == "")
                    {
                        Text = week[i] + " " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " 장";
                    }

                    weekLabel[i].Text = Text;
                    weekLabel[i].HorizontalTextAlignment = TextAlignment.Start;

                    DateTime checkDay = UtcToStandardTime();

                    int dayofPos = (int)checkDay.DayOfWeek - 1;
                    if (dayofPos < 0)
                        dayofPos = 6;

                    if (dayofPos == i)
                        weekLabel[i].TextColor = Color.Red;


                    // Your label tap event
                    var table_tap = new TapGestureRecognizer();
                    table_tap.Tapped += async (sender, e) =>
                    {
                        try
                        {
                            var labelText = sender as Label;

                            var parentPage = this.Parent as TabbedPage;

                            string[] words = labelText.Text.Split(' ');

                            User.CacheData.BibleName = words[1];
                            User.CacheData.Chapter = Convert.ToInt32(words[2]);
                            User.CacheData.Verse = 1;

                            parentPage.CurrentPage = parentPage.Children[1];
                        }
                        catch(Exception )
                        {

                        }
                    };

                    weekLabel[i].GestureRecognizers.Add(table_tap);
                }

            }
            catch (Exception e)
            {

            }

        }
        //밑줄 표시 grid row 시작 위치
        static int UnderliningPos = 4;

        private void AddMarkWord()
        {
            try
            {
                Helper.RemoveRowGrid(main_grid, UnderliningPos);

                var list = SQLLiteDB.ReadUnderlining();
                if (list != null)
                {
                    int startpos = UnderliningPos;
                    foreach (var data in list)
                    {
                        var contexttext = BibleInfo.GetContextText(BibleType.KRV, data.BibleName, data.Chapter, data.Verse);

                        int __verse;
                        string line;
                        Helper.SpliteVerseText(contexttext, out __verse, out line);

                        var text = data.BibleName + " " + data.Chapter + " 장" + " " + data.Verse + "절 \n" + line;

                        var labelText = new Label { Text = text, LineBreakMode = LineBreakMode.WordWrap, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };

                        var stackLayout = new StackLayout();

                        stackLayout.Children.Add(labelText);
                        var frame = new Frame { BorderColor = Color.Black, Padding = new Thickness(3, 3, 3, 3) };
                        frame.Content = stackLayout;

                        main_grid.Children.Add(frame, 0, startpos++);
                        Grid.SetColumnSpan(frame, 2);

                    }

                }
            }
            catch(Exception e)
            {

            }
        }

        public void RefreshData()
        {
            SetJuboLabel();

            SetWeeklyReadTable();

            Set2020Message();

            SetNoticeLabel();

            SetWorshipLabel();

            AddMarkWord();
        }

        protected override void OnAppearing()
        {
            RefreshData();
        }


        public Notice()
        {
            InitializeComponent();

           
            return;
           
        }
    }
}