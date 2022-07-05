using System.ComponentModel;

using OMOK;
using OMOK.Droid;
using OMOK.CustomAdMobView;

using Android.Content;
using Android.Gms.Ads;
using Android.Widget;

using Xamarin.Forms;
using Xamarin.Forms.Platform.Android;

//https://developers.google.com/admob/unity/test-ads

[assembly: ExportRenderer(typeof(CtBannerView), typeof(AdMobBannerViewRenderer))]
namespace OMOK.Droid
{
    public class AdMobBannerViewRenderer : ViewRenderer<CtBannerView, AdView>
    {
        public AdMobBannerViewRenderer(Context context) : base(context) { }

        // 안드로이드 용 테스트 아이디
        public string AdUnitIdSet = "ca-app-pub-9541028236702321/1413912651";

        protected override void OnElementChanged(ElementChangedEventArgs<CtBannerView> e)
        {
            base.OnElementChanged(e);

            if (e.NewElement != null && Control == null)
            {
                SetNativeControl(CreateAdView());
            }
        }

        protected override void OnElementPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            base.OnElementPropertyChanged(sender, e);

            if (e.PropertyName == nameof(AdView.AdUnitId))
                Control.AdUnitId = AdUnitIdSet;
        }

        private AdView CreateAdView()
        {
            /*
            var adView = new AdView(Context)
            {
                //AdSize = AdSize.SmartBanner,
                //AdUnitId = AdUnitIdSet
            };
            */
            var adView = new AdView(Context);

            adView.AdUnitId = AdUnitIdSet;

            adView.LayoutParameters = new LinearLayout.LayoutParams(LayoutParams.MatchParent, LayoutParams.MatchParent);

            switch ((Element as CtBannerView).eBannerSize)
            {
                case CtBannerView.eBannerSizeLise.Standardbanner:
                    adView.AdSize = AdSize.Banner;
                    break;
                case CtBannerView.eBannerSizeLise.LargeBanner:
                    adView.AdSize = AdSize.LargeBanner;
                    break;
                case CtBannerView.eBannerSizeLise.MediumRectangle:
                    adView.AdSize = AdSize.MediumRectangle;
                    break;
                case CtBannerView.eBannerSizeLise.FullBanner:
                    adView.AdSize = AdSize.FullBanner;
                    break;
                case CtBannerView.eBannerSizeLise.Leaderboard:
                    adView.AdSize = AdSize.Leaderboard;
                    break;
                case CtBannerView.eBannerSizeLise.SmartBannerPortrait:
                    adView.AdSize = AdSize.SmartBanner;
                    break;
                default:
                    adView.AdSize = AdSize.Banner;
                    break;
            }

            adView.LoadAd(new AdRequest.Builder().Build());

            // 테스트 하기 위한 테스트용 기기 번호 넣은것.
            // AdRequest request = new AdRequest.Builder().AddTestDevice(TestDeviceID).Build();
            //  adView.LoadAd(request);

            return adView;
        }
    }
}