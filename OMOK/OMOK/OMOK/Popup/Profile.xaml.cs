using Rg.Plugins.Popup.Extensions;
using Rg.Plugins.Popup.Pages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class LoginPopupPage : PopupPage
    {
        public LoginPopupPage()
        {
            InitializeComponent();
        }
        public bool IsAnimationEnabled { get; private set; } = true;
        protected override void OnAppearing()
        {
            base.OnAppearing();

            if (User.myInfo.PhotoPath != null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

            var level = Helper.LevelConverter(User.myInfo.level);
            var Record = User.myInfo.win + "승" + User.myInfo.lose + "패";
            Record = level + User.myInfo.NickName + "\n" + Record;

            if (User.Locale != "ko")
            {
                Label1.Text = User.myInfo.NickName + "  :  " + level;
                Label2.Text = "Record     :  " + User.myInfo.win + " Win " + User.myInfo.lose + " Defeat";
                Label3.Text = "Tournament : None";
                Label4.Text = "Word Rank  :  " + User.myInfo.rank;
                Label5.Text = "Level Point   :  " + User.myInfo.score;
            }
            else
            {
                Label1.Text = User.myInfo.NickName + "  :  " + level;
                Label2.Text = "전적     :  " + User.myInfo.win + "승" + User.myInfo.lose + "패";
                Label3.Text = "토너먼트 순위 : 없음";
                Label4.Text = "월드 랭킹  :  " + User.myInfo.rank;
                Label5.Text = "승급 포인트   :  " + User.myInfo.score;
            }


            var current = User.myInfo.score * 0.01;
            Progress.Progress = current;

            FrameContainer.HeightRequest = -1;
            if (!IsAnimationEnabled)
            {
                CloseImage.Rotation = 0;
                CloseImage.Scale = 1;
                CloseImage.Opacity = 1;

                OKButton.Scale = 1;
                OKButton.Opacity = 1;
                return;
            }

            CloseImage.Rotation = 380;
            CloseImage.Scale = 0.3;
            CloseImage.Opacity = 0;
            OKButton.Scale = 0.3;
            OKButton.Opacity = 0;
        }

        protected override async void OnAppearingAnimationEnd()
        {
            if (!IsAnimationEnabled)
                return;

            var translateLength = 400u;

            await Task.WhenAll(
                (new Func<Task>(async () =>
                {
                    await Task.Delay(200);

                }))());

            await Task.WhenAll(
                CloseImage.FadeTo(1),
                CloseImage.ScaleTo(1, easing: Easing.SpringOut),
                CloseImage.RotateTo(0),
                OKButton.ScaleTo(1),
                OKButton.FadeTo(1));
        }

        protected override async void OnDisappearingAnimationBegin()
        {
            if (!IsAnimationEnabled)
                return;

            var taskSource = new TaskCompletionSource<bool>();

            var currentHeight = FrameContainer.Height;

            await Task.WhenAll(
                OKButton.FadeTo(0));

            FrameContainer.Animate("HideAnimation", d =>
            {
                FrameContainer.HeightRequest = d;
            },
            start: currentHeight,
            end: 170,
            finished: async (d, b) =>
            {
                await Task.Delay(300);
                taskSource.TrySetResult(true);
            });

            await taskSource.Task;
        }

        private void OnCloseButtonTapped(object sender, EventArgs e)
        {
            CloseAllPopup();
        }

        protected override bool OnBackgroundClicked()
        {
            CloseAllPopup();

            return false;
        }

        private async void CloseAllPopup()
        {
            await Navigation.PopAllPopupAsync();
        }
        async void OnClosedClicked(object sender, System.EventArgs e)
        {
            CloseAllPopup();
        }
        
    }
}