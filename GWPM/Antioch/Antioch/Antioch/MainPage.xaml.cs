﻿using Antioch.Util;
using Antioch.View;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xamarin.Forms;



/*mvvm sample

It is important you learn about MVVM pattern and how to perform the data binding. You can see this link: https://docs.microsoft.com/en-us/xamarin/xamarin-forms/xaml/xaml-basics/data-bindings-to-mvvm.

Basically, you can do this:

Create a ViewModel for your HomePage.

public class HomePageViewModel : INotifyPropertyChanged
{
    private string name;
    public string Name
    {
        get
        {
            return name;
        }
        set
        {
            name = value;
            OnPropertyChanged(nameof(Name));
        }
    }
    public HomePageViewModel()
    {
        // some initialization code here ...
        Name = "John";
    }

    public event PropertyChangedEventHandler PropertyChanged;
    protected virtual void OnPropertyChanged(string propertyName)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }

}
Attach now your ViewModel to the HomePage View

public HomePageView()
{
    InitializeComponent();
    BindingContext = new HomePageViewModel();
}
Then in your XAML, you can have your binding like this:

<Label Text="{Binding Name}" />
Then whenever the Name changes in the ViewModel, it will be reflected in the XAML view.

Share
 */

namespace Antioch
{
    public partial class MainPage : ContentPage
    {
        public LobbyView lobby = new LobbyView();
        public SettingView setting = new SettingView();
        public MainPage()
        {
            InitializeComponent();

            LoadResourceData();

            symbol_image.Source = ImageSource.FromResource("Antioch.Resource.Image.symbol.png");
            symbol_image.GestureRecognizers.Add(new TapGestureRecognizer
            {
                TappedCallback = (v, o) => {
                    LoadView(lobby);
                },
                NumberOfTapsRequired = 1
            });


            ico_notify.Source = ImageSource.FromResource("Antioch.Resource.Image.IcoNotify.png");
            ico_setting.Source = ImageSource.FromResource("Antioch.Resource.Image.IcoSetting.png");


            ContentViews.Children.Add(lobby);

            NetworkProcess();
        }

        private void NetworkProcess()
        {
            //network
            {

                //network thread
                Task.Run(() =>
                {
                    while (true)
                    {
                        NetProcess.start();
                        NetProcess.client.PacketRecvSync();
                        Thread.Sleep(1);
                    }
                });

                //network thread
                Task.Run(() =>
                {
                    DateTime checktime = DateTime.Now;

                    while (true)
                    {
                        NetProcess.Loop();
                        Thread.Sleep(1);
                    }
                });

            }
        }

        protected override bool OnBackButtonPressed()
        {
            LoadView(lobby);
            return true;
        }

        public bool LoadLobby()
        {
            LoadView(lobby);
            return true;
        }

        void ico_notify_clicked(object sender, EventArgs e)
        {
        }

        void ico_login_clicked(object sender, EventArgs e)
        {
        }

        void ico_setting_clicked(object sender, EventArgs e)
        {
            ContentViews.Children.Clear();
            ContentViews.Children.Add(setting);
        }

        void OnTapped(object sender, EventArgs e)
        { 
        }

         public void LoadView(ContentView _view)
        {
            ContentViews.Children.Clear();
            ContentViews.Children.Add(_view);
        }
   

        private void LoadNoticeData()
        {
            try
            {
                var NoticeFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "notice.txt");
                GoolgeService.DownloadFileFromURLToPath("https://drive.google.com/file/d/1NSS2P2ECkU6QdvEj0cX3TVDveIMwyn01/view?usp=sharing", NoticeFile);

                var worship = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "worship.txt");
                GoolgeService.DownloadFileFromURLToPath("https://drive.google.com/file/d/1EamFoQDhKCminaJRFfKGU1OB50lobXos/view?usp=sharing", worship);
            }
            catch (Exception e)
            {
            }
        }

        private void LoadCacheUserData()
        {
            try
            {
            }
            catch (Exception e)
            {
            }
        }

        private void LoadResourceData()
        {
            try
            {
                BibleInfo.LoadKRV();
                BibleInfo.LoadKJV();
                Dic.LoadDic();
                Hymn.LoadList();
                //BibleInfo.LoadNIV();
                //BibleInfo.CheckValidate();
            }
            catch (Exception e)
            {
            }
        }
    }
}
