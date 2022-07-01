using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK.ViewModels;

using NetClient;
using OMOK.Network;
using OMOK.CustomAdMobView;
using Google.Protobuf.Collections;
using Rg.Plugins.Popup.Extensions;
using OMOK.Popup;
using ToastMessage;
using System.IO;
using System.Threading;

namespace OMOK.Views
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Lobby : ContentPage
    {
        CommunityViewModel viewModel = new CommunityViewModel();
        iAd_IterstitialView iIterstitia;
    
        public ImageSource SomeImage
        {
            get
            {
                var source = new Uri(User.myInfo.PhotoPath);
                return source;
            }
        }

        public Lobby()
        {
            InitializeComponent();
 
            iIterstitia = DependencyService.Get<iAd_IterstitialView>();
      //      iIterstitia.ShowAd();

            mypicture.Source = null;
    
            Device.StartTimer(TimeSpan.FromMilliseconds(50), () =>
            {
                if (NoticeLabel.TranslationX > -(NoticeLabel.Width + (NoticeLabel.Width * 0.2)))
                {
                    NoticeLabel.TranslationX -= 5f;
                }
                else
                {
                    NoticeLabel.TranslationX = NoticeLabel.Width;
                }

                return true;
            });


            //network thread
            Task.Run(() =>
            {
                while (true)
                {
                    NetProcess.start();
                    NetProcess.client.PacketRecvSync();
                }
            });

            //network thread
            Task.Run(() =>
            {
                DateTime checktime = DateTime.Now;

                while (true)
                {
                    NetProcess.Loop(this);
                }
            });

            UpdateLocalMenu();

            Navigation.PushModalAsync(new Loading()/*_MachPage*/);
        }

        public void UpdateLocalMenu()
        {
            //언어 수동 변환
            if (User.Locale != "ko")
            {
                NameButton.Text = "Name";
                HistroyButton.Text = "History";
                RankButton.Text = "Rank";
                SettingButton.Text = "Setting";
                MatchButton.Text = "Real Time Matching";
            }
            else
            {
                NameButton.Text = "프로필";
                HistroyButton.Text = "기록";
                RankButton.Text = "랭크";
                SettingButton.Text = "설정";
                MatchButton.Text = "실시간 매칭";
            }
        }

        protected override void OnAppearing()
        {
            if (User.myInfo.PhotoPath != null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
        }

        public void UpdatePlayerInfo()
        {
            var level = Helper.LevelConverter(User.myInfo.level);
            var Record = User.myInfo.win + "승" + User.myInfo.lose + "패";

            if (User.Locale != "ko")
                Record = User.myInfo.win + "Win" + User.myInfo.lose + "Defeat";

            NameButton.Text = level + "\n" + User.myInfo.NickName;
        }

        public void CreateRankPage(RepeatedField<global::Rank> list)
        {
            Navigation.PushModalAsync(new Rank(list));
        }


        public void UpdateMessage(CompletePacket packet)
        {
            viewModel.RoomModel.Clear();

            ROOM_LIST_RES res = new ROOM_LIST_RES();
            res = ROOM_LIST_RES.Parser.ParseFrom(packet.Data);

            foreach (var room in res.VarRoomList)
            {
                CommunityRoomInfoModel croom = new CommunityRoomInfoModel();
                croom.Id = room.VarId;
                croom.Name = Helper.ToStr(room.VarName.ToByteArray()); 
                croom.CurrentCount = room.VarCurrentCount;

                viewModel.RoomModel.Add(croom);
            }

            listView.ItemsSource = viewModel.RoomModel;

            if (User.myInfo.PhotoPath != null && mypicture.Source == null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

        }

        public void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as CommunityRoomInfoModel;

            //if (contact != null)
            //    NetProcess.SendEnterRoom(contact.Id);
        }

        public void LoginInformation(string msg)
        {
            DisplayAlert("", msg, "OK");
        }

        public void SetNotice(string message)
        {
            NoticeLabel.Text = message;
            NoticeLabel.TranslationX = 0;
        }

        public void ClosePopup()
        {
            Navigation.PopModalAsync();
        }

        public void PushRoomPopup(ContentPage page)
        {
            Navigation.PushModalAsync(page);

        }

        public void PopRoomPopup()
        {
            Navigation.PopModalAsync();
        }

        public void CloseAllPopup()
        {
            Navigation.PopAllPopupAsync();
        }

        NickNameEdit currentNickNamePopup = new NickNameEdit();

        public void SendNickNamePopupMessage()
        {
            MessagingCenter.Send<NickNameEdit>(currentNickNamePopup, "Fail");
        }

        public void NickNamePopup()
        {
             Navigation.PushPopupAsync(currentNickNamePopup);
        }

        public static Task<string> InputBox(INavigation navigation)
        {
            // wait in this proc, until user did his input 
            var tcs = new TaskCompletionSource<string>();

            var lblTitle = new Label { Text = "로그인", HorizontalOptions = LayoutOptions.Center, FontAttributes = FontAttributes.Bold };
            var lblMessage = new Label { Text = "아이디를 입력하세요" };
            var txtInput = new Entry { Text = "nhs1" };

            var btnOk = new Button
            {
                Text = "확인",
                WidthRequest = 100,
                BackgroundColor = Color.FromRgb(0.8, 0.8, 0.8),
            };
            btnOk.Clicked += async (s, e) =>
            {
                // close page
                var result = txtInput.Text;
                await navigation.PopModalAsync();
                // pass result
                tcs.SetResult(result);
            };

            var btnCancel = new Button
            {
                Text = "취소",
                WidthRequest = 100,
                BackgroundColor = Color.FromRgb(0.8, 0.8, 0.8)
            };
            btnCancel.Clicked += async (s, e) =>
            {
                // close page
                await navigation.PopModalAsync();
                // pass empty result
                tcs.SetResult(null);
            };

            var slButtons = new StackLayout
            {
                Orientation = StackOrientation.Horizontal,
                Children = { btnOk, btnCancel },
            };

            var layout = new StackLayout
            {
                Padding = new Thickness(0, 40, 0, 0),
                VerticalOptions = LayoutOptions.StartAndExpand,
                HorizontalOptions = LayoutOptions.CenterAndExpand,
                Orientation = StackOrientation.Vertical,
                Children = { lblTitle, lblMessage, txtInput, slButtons },
            };

            // create and show page
            var page = new ContentPage();
            page.Content = layout;
            navigation.PushModalAsync(page);
            // open keyboard
            txtInput.Focus();

            // code is waiting her, until result is passed with tcs.SetResult() in btn-Clicked
            // then proc returns the result
            return tcs.Task;
        }

        async void OnProfileClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushPopupAsync(new LoginPopupPage());
        }

        async void OnMatchClicked(object sender, System.EventArgs e)
        {
           NetProcess.SendMatch();

            await Navigation.PushModalAsync(new MatchInfoPage()/*_MachPage*/);

        }

       public void MatchAuto()
        {
            NetProcess.SendMatch();

            Navigation.PushModalAsync(new MatchInfoPage()/*_MachPage*/);
        }

        
        async void OnHistoryClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new RecordHistory()/*_MachPage*/);

        }

        async void OnRankClicked(object sender, System.EventArgs e)
        {
            NetProcess.SendRank();
        }
        async void OnSettingClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushModalAsync(new Setting());
        }

        async void OnSingleMatchClicked(object sender, System.EventArgs e)
        {
            User.myInfo.ai_reset_flag = false;

            await Navigation.PushModalAsync(new SingleMatch());
        }

        async void OnShopClicked(object sender, System.EventArgs e)
        {
            var cameraPage = new CameraPage();
            //Navigation.PushAsync((new CameraPage()/*_MachPage*/));
            Navigation.PushPopupAsync(cameraPage);

            //await DisplayAlert("Info", "Comming Soon", "OK");
            DependencyService.Get<Toast>().Show("Comming Soon");
        }

        async void OnLoginClicked(object sender, System.EventArgs e)
        {
            string inputstring = await InputBox(this.Navigation);

            User.myInfo.NickName = inputstring;

            NetProcess.SendLogin(inputstring, inputstring);
        }

       

    }
}