using Antioch.Util;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    public class MyPosToBibleRead
    {
        public string begin_bibleName;
        public int begin_chapter;

        public string end_bibleName;
        public int end_chapter;
    }

  

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class NoticeView : ContentView
    {
        public NoticeView()
        {
            InitializeComponent();

            RefreshData();
        }

        public DateTime UtcToStandardTime()
        {
            DateTime currentTime = DateTime.UtcNow;

            try
            {
                var ret = TimeZoneInfo.ConvertTimeBySystemTimeZoneId(currentTime, "Asia/Seoul");
                return ret;
            }
            catch(Exception e)
            {
                return currentTime;

            }
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

            var data =  SQLLiteDB.ReadBibleReadPlan();
            if (data == null)
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
                currChapter += data.BeginChapter;


                accChapterSize += chapter;


                if (currChapter <= accChapterSize)
                {
                    readpos.begin_bibleName = bible.Name;
                    readpos.begin_chapter = Math.Abs((accChapterSize - currChapter) - chapter);

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
            var tap = new TapGestureRecognizer();
            tap.Tapped += async (s, e) =>
            {
                var labelText = s as Label;
                await Share.RequestAsync(new ShareTextRequest
                {
                    Text = ShareTimeTable,
                    Title = "Bible Read Share"
                });
            };

            share_labal.GestureRecognizers.Add(tap);
        }

        string ShareTimeTable;

        private void SetWeeklyReadTable()
        {
            try
            {
                ShareTimeTable = "";

                Label[] weekLabel = new Label[7];

                weekLabel[0] = label_mon;
                weekLabel[1] = label_tue;
                weekLabel[2] = label_wed;
                weekLabel[3] = label_thu;
                weekLabel[4] = label_fri;
                weekLabel[5] = label_sat;
                weekLabel[6] = label_sun;

#if GLOBAL
                string[] week = { "Mon", "Thu", "Wed", "Thur", "Fri", "Sat", "Sun" };
#else
                string[] week = { "월", "화", "수", "목", "금", "토", "일" };
#endif
                for (int i = 0; i < 7; i++)
                {
                    var info = CalculateTodayBibleChapter(i);
                    if (info == null)
                    {
                        weekLabel[i].Text = week[i] + "| ";
                        continue;
                    }
#if GLOBAL
                    string Text = week[i] + " " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " chapter" + "~ " + info.end_bibleName + " " + info.end_chapter.ToString() + " chapter";
#else
                    string Text = week[i] + " " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " 장" + "~ " + info.end_bibleName + " " + info.end_chapter.ToString() + " 장";

#endif
                    //한장씩 읽을때는 end_bibleName 없으므로 시작 만 출력해준다. 
                    if (info.end_bibleName == "")
                    {
#if GLOBAL
                        Text = week[i] + " | " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " chapter";
#else
                        Text = week[i] + " " + info.begin_bibleName + " " + info.begin_chapter.ToString() + " 장";
#endif
                    }

                    weekLabel[i].Text = Text;
                    weekLabel[i].HorizontalTextAlignment = TextAlignment.Start;

                    DateTime checkDay = UtcToStandardTime();

                    int dayofPos = (int)checkDay.DayOfWeek - 1;
                    if (dayofPos < 0)
                        dayofPos = 6;

                    if (dayofPos == i)
                        weekLabel[i].TextColor = Color.Red;

                    ShareTimeTable += Text += "\n";


                    // Your label tap event
                    var table_tap = new TapGestureRecognizer();
                    table_tap.Tapped += async (sender, e) =>
                    {
                        try
                        {
                            //var labelText = sender as Label;

                            //var parentPage = this.Parent as TabbedPage;

                            //string[] words = labelText.Text.Split(' ');

                            //User.CacheData.BibleName = words[1];
                            //User.CacheData.Chapter = Convert.ToInt32(words[2]);
                            //User.CacheData.Verse = 1;

                            //parentPage.CurrentPage = parentPage.Children[1];
                        }
                        catch (Exception)
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
#if GLOBAL
                          var contexttext = BibleInfo.GetContextText(BibleType.KJV, data.BibleName, data.Chapter, data.Verse);
#else
                        var contexttext = BibleInfo.GetContextText(BibleType.KRV, data.BibleName, data.Chapter, data.Verse);
#endif


                        int __verse;
                        string line;
                        Helper.SpliteVerseText(contexttext, out __verse, out line);

#if GLOBAL
                        var text = data.BibleName + " " + data.Chapter + " Chapter" + " " + data.Verse + "Verse \n" + line;
#else
                        var text = data.BibleName + " " + data.Chapter + " 장" + " " + data.Verse + "절 \n" + line;
#endif
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
            catch (Exception e)
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
    }
}