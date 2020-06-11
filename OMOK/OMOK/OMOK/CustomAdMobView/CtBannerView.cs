using System;
using System.Collections.Generic;
using System.Text;

using Xamarin.Forms;

namespace OMOK.CustomAdMobView
{
    public class CtBannerView : View
    {
        public enum eBannerSizeLise { Standardbanner, LargeBanner, MediumRectangle, FullBanner, Leaderboard, SmartBannerPortrait }
        public eBannerSizeLise eBannerSize { get; set; }

    }
}
