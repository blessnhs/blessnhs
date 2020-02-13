using WBA.MainTabbedPage;
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using System.IO;

namespace WBA
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Notice : ContentPage
    {

        public class MyPosToBibleRead
        {
            public string bibleName;
            public int chapter;
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

        private MyPosToBibleRead CalculateTodayBibleChapter(int addDay = 0)
        {
            MyPosToBibleRead readpos = new MyPosToBibleRead();
                                
            DateTime BeginTime = new DateTime(2020, 01, 13, 0, 0, 0, DateTimeKind.Local);
            DateTime MondayTime = WeekDateTime(DateTime.Now, DayOfWeek.Monday);

            MondayTime = MondayTime.AddDays(addDay);

            int DiffDay = (MondayTime - BeginTime).Days;

            string begin = "마태복음";

            int accChapterSize = 0;
            foreach(var bible in BibleInfo.ListNewTestament)
            {
                int chapter = BibleInfo.GetChapterSize(bible);
                accChapterSize += chapter;

                if ( DiffDay < accChapterSize)
                {
                    readpos.bibleName = bible;

                    
                    int v = (accChapterSize - chapter);
                    if (v < 0)
                        v = 0;

                    int diff = DiffDay - v; 

                    readpos.chapter = diff + 1;
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

            notify.Text = Message;
        }
      
        private void SetWorshipLabel()
        {
            string Message = "";
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

            worship.Text = Message;
        }
        private void Set2020Message()
        {
            // Your label tap event
            var forgetPassword_tap = new TapGestureRecognizer();
            forgetPassword_tap.Tapped += async (s, e) =>
            {
                var labelText = s as Label;

                Navigation.PushModalAsync(new ImageView("WBA.Resource.Image.2020Message.jpg"));
            };

            label_2020msg.GestureRecognizers.Add(forgetPassword_tap);
        }

        private void SetWeeklyReadTable()
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

                string Text = week[i] + "         " + info.bibleName + " " + info.chapter.ToString() + " 장";

                weekLabel[i].Text = Text;

                DateTime checkDay = UtcToStandardTime();

                int dayofPos = (int)checkDay.DayOfWeek - 1;
                if (dayofPos < 0)
                    dayofPos = 6;

                if (dayofPos == i)
                    weekLabel[i].TextColor = Color.Red;


                // Your label tap event
                var forgetPassword_tap = new TapGestureRecognizer();
                forgetPassword_tap.Tapped += async (sender, e) =>
                {
                    var labelText = sender as Label;

                    var parentPage = this.Parent as TabbedPage;

                    string[] words = labelText.Text.Split(' ');

                    SQLLiteDB.CacheData.BibleName = words[9];
                    SQLLiteDB.CacheData.Chapter = Convert.ToInt32(words[10]);
                    SQLLiteDB.CacheData.Verse = 1;

                    parentPage.CurrentPage = parentPage.Children[1];
                };

                weekLabel[i].GestureRecognizers.Add(forgetPassword_tap);
            }

        }


        public Notice()
        {
            InitializeComponent();

            SetJuboLabel();

            SetWeeklyReadTable();

            Set2020Message();

            SetNoticeLabel();

            SetWorshipLabel();
            return;
           
        }
    }
}