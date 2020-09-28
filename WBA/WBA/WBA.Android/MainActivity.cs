using System;

using Android.App;
using Android.Runtime;
using Android.Views;
using Android.OS;
using Android.Content;

using Microsoft.AppCenter;
using Microsoft.AppCenter.Analytics;
using Microsoft.AppCenter.Crashes;
using Microsoft.AppCenter.Distribute;

namespace WBA.Droid
{
    [Activity(Label = "WBA")]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        protected override void OnCreate(Bundle savedInstanceState)
        {
            AppCenter.Start("d9ecfc4b-f729-47c3-9a13-3612f85034d4",
                   typeof(Analytics), typeof(Crashes));

            AppCenter.Start("{Your Xamarin Android App Secret}", typeof(Distribute));
            Distribute.SetEnabledForDebuggableBuild(true);

            TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.Window.RequestFeature(WindowFeatures.ActionBar);
            // Name of the MainActivity theme you had there before.
            // Or you can use global::Android.Resource.Style.ThemeHoloLight
            base.SetTheme(Resource.Style.MainTheme);


            base.OnCreate(savedInstanceState);
            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);

            LoadApplication(new App());
                   
            CreateService();
        }
        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] Android.Content.PM.Permission[] grantResults)
        {
            Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            base.OnRequestPermissionsResult(requestCode, permissions, grantResults);
        }


        private void CreateService()
        {
            Intent startService = new Intent(this, typeof(BackEndService));
            startService.SetAction(Constants.ACTION_START_SERVICE);

            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {
                StartForegroundService(startService);
            }
            else
            {
                StartService(startService);
            }

        }

    }

}

