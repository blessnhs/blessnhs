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
    public partial class GameResultPage : PopupPage
    {
        GAME_RESULT_NTY nty;

        public GameResultPage(GAME_RESULT_NTY _nty)
        {
            InitializeComponent();
            nty = _nty;

            if (User.Locale != "ko")
            {

                if (nty.VarIndex == User.Id)
                    Label1.Text = "Vectory";
                else
                    Label1.Text = "Defeat";


                if (nty.VarIndex1 == User.Id)
                {
                    if (User.myInfo.level < nty.VarLevel1)
                    {
                        Label1.Text = "Congratulations. promotion ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel1);
                    }
                    else if (User.myInfo.level > nty.VarLevel1)
                    {
                        Label1.Text = "Relegation... ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel1);
                    }


                    Label3.Text = " Point " + User.myInfo.score + " => " + nty.VarLevelPoint1;


                    User.myInfo.level = nty.VarLevel1;
                    User.myInfo.score = nty.VarLevelPoint1;
                }
                else if (nty.VarIndex2 == User.Id)
                {
                    if (User.myInfo.level < nty.VarLevel2)
                    {
                        Label1.Text = "Congratulations. promotion ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel2);
                    }
                    else if (User.myInfo.level > nty.VarLevel2)
                    {
                        Label1.Text = "Relegation... ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel2);
                    }

                    Label3.Text = " Point " + User.myInfo.score + " => " + nty.VarLevelPoint2;

                    User.myInfo.level = nty.VarLevel2;
                    User.myInfo.score = nty.VarLevelPoint2;
                }

                Label5.Text = "Level Point   :  " + User.myInfo.score;
            }
            else
            {

                if (nty.VarIndex == User.Id)
                    Label1.Text = "승리 하셨습니다.";
                else
                    Label1.Text = "패배 하셨습니다.";


                if (nty.VarIndex1 == User.Id)
                {
                    if (User.myInfo.level < nty.VarLevel1)
                    {
                        Label1.Text = "축하합니다. 승급하셨습니다. ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel1);
                    }
                    else if (User.myInfo.level > nty.VarLevel1)
                    {
                        Label1.Text = "강등되셨습니다. ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel1);
                    }


                    Label3.Text = " 포인트 " + User.myInfo.score + " => " + nty.VarLevelPoint1;


                    User.myInfo.level = nty.VarLevel1;
                    User.myInfo.score = nty.VarLevelPoint1;
                }
                else if (nty.VarIndex2 == User.Id)
                {
                    if (User.myInfo.level < nty.VarLevel2)
                    {
                        Label1.Text = "축하합니다. 승급하셨습니다. ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel2);
                    }
                    else if (User.myInfo.level > nty.VarLevel2)
                    {
                        Label1.Text = "강등되셨습니다. ";
                        Label2.Text = Helper.LevelConverter(User.myInfo.level) + " => " + Helper.LevelConverter(nty.VarLevel2);
                    }

                    Label3.Text = " 포인트 " + User.myInfo.score + " => " + nty.VarLevelPoint2;

                    User.myInfo.level = nty.VarLevel2;
                    User.myInfo.score = nty.VarLevelPoint2;
                }

                Label5.Text = "승급 포인트   :  " + User.myInfo.score;

            }

            if (User.Auto == true)
                Navigation.PopAllPopupAsync();


        }
        public bool IsAnimationEnabled { get; private set; } = true;
        protected override void OnAppearing()
        {
            base.OnAppearing();

            if (User.myInfo.PhotoPath != null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

            
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