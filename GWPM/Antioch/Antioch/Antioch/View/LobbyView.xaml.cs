using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class LobbyView : ContentView
    {
        public RoomsPage roompage = new RoomsPage();
        public MainChatPage chatpage = new MainChatPage();
        public LobbyView()
        {
            InitializeComponent();

            banner_image.Source = ImageSource.FromResource("Antioch.Resource.Image.banner.png");
        }


        void OnTapped(object sender, EventArgs e)
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
                    LoadView(new Lecture()); 
                    break;
                case "Chat":
                    Navigation.PushModalAsync(roompage);
                    break;

                case "Plan":
                    LoadView(new BibleReadPlan()); 
                    break;
                case "Worship":
                    LoadView(new Hymn());
                    break;
                case "Pray":
                    LoadView(new PrayView());
                    break;
                case "Bible":
                    LoadView(new BibleView());
                    break;
                case "HomePage":
                    Device.OpenUri(new Uri("http://www.antiochi.net"));
                    break;
                case "QNA":
                    break;
            }
           

           

        }

        public void LoadView(ContentView view)
        {
            var grid = this.Parent as Grid;
            grid.Children.Clear();
            grid.Children.Add(view);
        }
    }
}