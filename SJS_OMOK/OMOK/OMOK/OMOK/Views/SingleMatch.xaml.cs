using OMOK.Control;
using OMOK.CustomAdMobView;
using OMOK.Popup;
using Rg.Plugins.Popup.Extensions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ToastMessage;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SingleMatch : ContentPage
    {
        AI _ai = new AI();
        BoardRenderer _renderer = new BoardRenderer();

        public bool isPlaying = false;
        int size  = 15;

        public void InitBoardGrid()
        {
            Device.StartTimer(new TimeSpan(0, 0, 1), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    _renderer.Init(size/* //오목 15x15*/, Width, ref absoluteLayout, blackLabel, whiteLabel, bottom1picture, bottom2picture);
                });
                return false; // runs again, or false to stop
            });

        }

        //전면광고
        iAd_IterstitialView iIterstitia;

        //보상광고
        iAd_RewardVideoView rewardVideo;

        public SingleMatch()
        {
            InitializeComponent();

            InitBoardGrid();

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();
            rewardVideo = DependencyService.Get<iAd_RewardVideoView>();

            _ai._renderer = _renderer;

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                   try
                    {
                        if (User.myInfo.ai_reset_flag == true)
                        {

                            _ai.PlayGame(User.myInfo.ai_rule, User.myInfo.ai_mode);
                            isPlaying = true;
                            User.myInfo.ai_reset_flag = false;

                            _renderer.UpdateBattleInfo();
                        }

                        if (isPlaying == true)
                        {
                            isPlaying = _ai.PlaygameLoop(this,User.myInfo.ai_mode);

                            if (isPlaying == false) //종료
                            {
                                isPlaying = false;

                            }
                        }
                    }
                    catch(Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                });

                if (isExit == true)
                    return false;

                return true;
            });

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

            Clicked.IsEnabled = false;


            MessagingCenter.Subscribe<SingleMatch>(this, "close", (sender) =>
            {
                Navigation.PopModalAsync();
            });


            //언어 수동 변환
            if (User.Locale != "ko")
            {
                LeaveRoom.Text = "Exit";

                LeftButton.Text = "Left";
                UPButton.Text = "Up";
                Clicked.Text = "Stone";
                DownButton.Text = "Down";
                RightButton.Text = "Right";

                black.Text = "Black";
                white.Text = "White";

                BeginBtn.Text = "Begin";
                RuleBtn.Text = "Rule";
            }

        }

        protected override void OnAppearing()
        {
            base.OnAppearing();
        }

        bool isExit = false;

        protected override void OnDisappearing()
        {
            isExit = true;
        }
      
     
        async void OnSettingClicked(object sender, System.EventArgs e)
        {
            if (retrycount != 0)
            {
                if(retrycount % 3 == 0)
                    rewardVideo.ShowAd();
                else
                    iIterstitia.ShowAd();
                
            }

            _renderer.ClearBoardState();

            retrycount++;
            await Navigation.PushPopupAsync(new AISelect(this));
        }

        int retrycount = 0;

        void OnRetryClicked(object sender, System.EventArgs e)
        {
            _renderer.ClearBoardState();

            if (retrycount != 0)
            {
                if (retrycount % 3 == 0)
                    rewardVideo.ShowAd();
                else
                    iIterstitia.ShowAd();
            }

            retrycount++;

            Clicked.IsEnabled = true;

            User.myInfo.ai_reset_flag = true;
        }
        async void OnLeaveClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushPopupAsync(new Confirm(this));

       //     var page = await Navigation.PopModalAsync();
        }

        void OnClickedLeft(object sender, System.EventArgs e)
        {
            if (0 >_renderer.aimx)
                return;

            _renderer.aimx -= 1;

            _renderer.UpdateAim();
        }

        void OnClickedUp(object sender, System.EventArgs e)
        {
            if (0 > _renderer.aimy)
                return;

            _renderer.aimy -= 1;
            _renderer.UpdateAim();
        }

        void OnPutStone(object sender, System.EventArgs e)
        {
            if (isPlaying == false)
                return;

            _ai.aix = _renderer.aimx;
            _ai.aiy = _renderer.aimy;
       }

        void OnClickedDown(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= _renderer.aimy)
                return;

            _renderer.aimy += 1;
            _renderer.UpdateAim();
        }

        void OnClickedRight(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= _renderer.aimx)
                return;

            _renderer.aimx += 1;
            _renderer.UpdateAim();
        }

    }
}