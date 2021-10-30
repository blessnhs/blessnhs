using DependencyHelper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class LobbyView : ContentView
    {
        public RoomsPageView roompage = new RoomsPageView();
        public Dictionary<int,MainChatView> chatpage = new Dictionary<int, MainChatView>();
        public PrayView praypage = new PrayView();


        public Lecture lec1 = new Lecture();
        public Lecture2 lec2 = new Lecture2();

        public MainChatView GetCurrentChatView()
        {
            if (chatpage.Count() == 0)
                return null;

            return chatpage[User.CurrentChatViewNumber];
        }

        public void LoadRoomPageView()
        {
            LoadView(roompage);
        }

        public LobbyView()
        {
            InitializeComponent();
            if (Device.RuntimePlatform == Device.UWP)
            {
                banner_image.Source = ImageSource.FromResource("Antioch.Resource.Image.banner.png", Assembly.GetExecutingAssembly());
            }
            else
            {
                banner_image.Source = ImageSource.FromResource("Antioch.Resource.Image.banner.png", Assembly.GetExecutingAssembly());
            }

#if GLOBAL
 
#else
            Noti.Text = "공지";
            Bull.Text = "주보";
            Serm.Text = "말씀";
            Lec1.Text = "강의1";
            LChat.Text = "채팅";
            LBible.Text = "성경";
            Lec2.Text = "강의2";
            LPray.Text = "기도";

            LPlan.Text = "계획";
            Evan.Text = "전도";

            Home.Text = "홈";
            LQnA.Text = "질문";

#endif

        }


        void OnTapped(object sender, EventArgs e)
        {
            try
            {
                var sndObject = sender as StackLayout;
                var grid = this.Parent as Grid;

                switch (sndObject.StyleId)
                {
                    case "Notice":
                        LoadView(new NoticeView());
                        break;
                    case "Bulletin":
                        Device.OpenUri(new Uri("http://www.antiochi.net/jubo.pdf"));
                        break;
                    case "Sermon":
                        Device.OpenUri(new Uri("http://www.antiochi.net"));
                        break;
                    case "Community":
                        break;

                    case "Lecture":

                        lec1.Content = null;
                        lec1 = null;
                        lec1  = new Lecture();

                        LoadView(lec1);
                        break;

                    case "Lecture2":

                        lec2.Content = null;
                        lec2 = null;
                        lec2 = new Lecture2();

                        LoadView(lec2);
                        break;

                    case "Chat":
                        {
                            if (User.LoginSuccess == false)
                            {
#if GLOBAL
                                if (Device.RuntimePlatform == Device.Android)
                                    DependencyService.Get<Toast>().Show("need to log in");
#else
                                DependencyService.Get<Toast>().Show("로그인해야 합니다");
#endif
                                var mainpage = (MainPage)Application.Current.MainPage;

                                mainpage.LoadSetting();
                            }
                            else
                            {
                                NetProcess.SendRoomList();
                                LoadView(roompage);
                            }
                            //if(GetCurrentChatView() == null)
                            //    LoadView(roompage);
                            //else
                            //    LoadView(GetCurrentChatView());
                        }
                        break;

                    case "Plan":
                        LoadView(new BibleReadPlan());
                        break;
                    case "Worship":
                      //  LoadView(new Hymn());
                        break;
                    case "QnA":

                        LoadView(new QnAView());
                        break;
                    case "Pray":
                        {
                            NetProcess.SendPrayList();
                            LoadView(praypage);
                        }
                        break;
                    case "Bible":
                        LoadView(new BibleView());
                        break;
                    case "HomePage":
                        Device.OpenUri(new Uri("http://www.antiochi.net"));
                        break;

                    case "Evangelize":
                        Share.RequestAsync(new ShareTextRequest
                        {
                            Text = "https://youtu.be/Dm89UpFcHVQ",
                            Title = "#전도 컨텐츠"
                        });
                        break;
                }
            }
            catch(Exception)
            {

            }
        }


        public void LoadView(ContentView view)
        {
            if (this.Parent == null)
            {
                var mainpage = (MainPage)Application.Current.MainPage;
                mainpage.LoadView(view);
            }
            else
            {
                var grid = this.Parent as Grid;
                grid.Children.Clear();
                grid.Children.Add(view);
            }
        }
    }
}