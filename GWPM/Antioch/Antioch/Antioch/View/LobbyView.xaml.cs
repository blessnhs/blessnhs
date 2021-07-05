﻿using System;
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
        public RoomsPage roompage = new RoomsPage();
        public MainChatPage chatpage = new MainChatPage();
        public PrayView praypage = new PrayView();

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
                        LoadView(new Lecture());
                        break;

                    case "Lecture2":
                        LoadView(new Lecture2());
                        break;

                    case "Chat":
                        Navigation.PushModalAsync(roompage);
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
                        LoadView(praypage);
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
            var grid = this.Parent as Grid;
            grid.Children.Clear();
            grid.Children.Add(view);
        }
    }
}