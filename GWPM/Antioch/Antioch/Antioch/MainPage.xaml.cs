using Antioch.Util;
using Antioch.View;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace Antioch
{
    public partial class MainPage : ContentPage
    {
        LobbyView lobby = new LobbyView();
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


            ContentViews.Children.Add(lobby);
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
