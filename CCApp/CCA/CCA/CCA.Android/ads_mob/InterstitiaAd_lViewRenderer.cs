using System;

using CCA.Droid;
using CCA.CustomAdMobView;

using Android.Gms.Ads.Reward;
using Android.Gms.Ads;
using Xamarin.Forms;

using Android.Util;

//https://developers.google.com/admob/unity/test-ads

[assembly: Dependency(typeof(InterstitiaAd_lViewRenderer))]
namespace CCA.Droid
{
    public class InterstitiaAd_lViewRenderer : AdListener, iAd_IterstitialView
    {
        InterstitialAd interstitialAd;

        //안드로이드 테스트 아이디 및 테스트 디바이스.
        string AdUnitId = "ca-app-pub-9541028236702321/2002871218"; // 전면 광고

        bool isShow = false;

        public InterstitiaAd_lViewRenderer()
        {
            interstitialAd = new InterstitialAd(Android.App.Application.Context);

            interstitialAd.AdUnitId = AdUnitId;
            interstitialAd.AdListener = this;

            isShow = false;
            //LoadAd();
        }

        void LoadAd()
        {
            var requestbuilder = new AdRequest.Builder();

            // 테스트 하기 위한 테스트용 기기 번호 넣은것.
            //requestbuilder.AddTestDevice(TestDeviceID);

            interstitialAd.LoadAd(requestbuilder.Build());
        }

        public void ShowAd()
        {
            base.OnAdLoaded();

            isShow = true;

            if (interstitialAd.IsLoaded)
                interstitialAd.Show();

            LoadAd();
        }

        public override void OnAdLoaded()
        {
            if (!isShow)
            {
                return;
            }

            base.OnAdLoaded();

            if (interstitialAd.IsLoaded) // false
                interstitialAd.Show();
        }

        public override void OnAdFailedToLoad(int errorCode)
        {
            base.OnAdFailedToLoad(errorCode); // errorCode = 0
        }
    }
}