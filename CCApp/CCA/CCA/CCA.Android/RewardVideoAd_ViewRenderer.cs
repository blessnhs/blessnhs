using System;

using OMOK.Droid;
using OMOK.CustomAdMobView;

using Android.Gms.Ads.Reward;
using Android.Gms.Ads;
using Xamarin.Forms;

using Android.Util;

[assembly: Dependency(typeof(RewardVideoAd_ViewRenderer))]
namespace OMOK.Droid
{
    public class RewardVideoAd_ViewRenderer : AdListener, iAd_RewardVideoView
    {
        InterstitialAd RewardVideoAd;

        //안드로이드 테스트 아이디 및 테스트 디바이스.
        //https://developers.google.com/admob/unity/test-ads
        string AdUnitId = "ca-app-pub-9541028236702321/4678326084"; // 동영상 광고


        bool isShow = false;

        public RewardVideoAd_ViewRenderer()
        {
            RewardVideoAd = new InterstitialAd(Android.App.Application.Context);

            RewardVideoAd.AdUnitId = AdUnitId;
            RewardVideoAd.AdListener = this;

            isShow = false;
            //LoadAd();

            RewardVideoAd.RewardedVideoAdOpened += RewardVideoAd_RewardedVideoAdOpened;
            RewardVideoAd.Rewarded += RewardVideoAd_Rewarded;
            RewardVideoAd.RewardedVideoAdClosed += RewardVideoAd_RewardedVideoAdClosed;
            RewardVideoAd.RewardedVideoAdLeftApplication += RewardVideoAd_RewardedVideoAdLeftApplication;
            RewardVideoAd.RewardedVideoAdLoaded += RewardVideoAd_RewardedVideoAdLoaded;

        }

        private void RewardVideoAd_RewardedVideoAdOpened(object sender, EventArgs e)
        {
            Log.Debug("TEST", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        }

        private void RewardVideoAd_Rewarded(object sender, RewardedEventArgs e)
        {
            //e.Reward.Amount;
            //e.Reward.GetType()
            Log.Debug("TEST", "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
        }

        private void RewardVideoAd_RewardedVideoAdClosed(object sender, EventArgs e)
        {
            Log.Debug("TEST", "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
        }

        private void RewardVideoAd_RewardedVideoAdLeftApplication(object sender, EventArgs e)
        {
            Log.Debug("TEST", "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
        }

        private void RewardVideoAd_RewardedVideoAdLoaded(object sender, EventArgs e)
        {
            Log.Debug("TEST", "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
        }

        void LoadAd()
        {
            var requestbuilder = new AdRequest.Builder();

            // 테스트 하기 위한 테스트용 기기 번호 넣은것.
            //requestbuilder.AddTestDevice(TestDeviceID);

            RewardVideoAd.LoadAd(requestbuilder.Build());
        }

        public void ShowAd()
        {
            base.OnAdLoaded();

            isShow = true;

            if (RewardVideoAd.IsLoaded)
                RewardVideoAd.Show();

            LoadAd();
        }

        public override void OnAdLoaded()
        {
            if (!isShow)
            {
                return;
            }

            base.OnAdLoaded();

            if (RewardVideoAd.IsLoaded) // false
                RewardVideoAd.Show();
        }

        public override void OnAdFailedToLoad(int errorCode)
        {
            base.OnAdFailedToLoad(errorCode); // errorCode = 0
        }

        /*
        public override void OnAdClosed()
        {
            Log.Debug("TEST", "QQQQQQQQQQQQQQQQQQQQQQ");
        }

        public override void OnAdOpened()
        {
            Log.Debug("TEST", "SSSSSSSSSSSSSSSSSSSSSSS");
        }
        */
    }
}