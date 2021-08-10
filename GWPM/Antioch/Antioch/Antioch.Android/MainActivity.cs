﻿using System;

using Android.App;
using Android.Runtime;
using Android.Views;
using Android.OS;
using Android.Content;
using Android.Content.PM;

namespace Antioch.Droid
{
    [Activity(Label = "Antioch", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = false, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize )]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        protected override void OnCreate(Bundle savedInstanceState)
        {
            TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.OnCreate(savedInstanceState);

            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);
            LoadApplication(new App());

            Rg.Plugins.Popup.Popup.Init(this, savedInstanceState);

            RegisterForegroundService();
        }
        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] Android.Content.PM.Permission[] grantResults)
        {
            Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            base.OnRequestPermissionsResult(requestCode, permissions, grantResults);
        }

        public override void OnBackPressed()
        {
            if (Rg.Plugins.Popup.Popup.SendBackPressed(base.OnBackPressed))
            {
            }
            else
            {
            }
        }
    

        void RegisterForegroundService()
        {
            NotificationManager Manager = (NotificationManager)GetSystemService(NotificationService);

            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {

                var chan1 = new NotificationChannel("DE",
                    "DE", NotificationImportance.Default);
                chan1.LockscreenVisibility = NotificationVisibility.Private;
                Manager.CreateNotificationChannel(chan1);

                var notification = new Notification.Builder(this, "DE")
                .SetContentTitle("Antioch Bible Reader")
                .SetContentText("Antioch")
                .SetSmallIcon(Resource.Drawable.xamagonBlue)
                .Build();


                //StartForeground(Constants.SERVICE_RUNNING_NOTIFICATION_ID, notification);
            }
            else
            {
                var notification = new Notification.Builder(this)
                           .SetContentTitle("Antioch Bible Reader")
                           .SetContentText("Antioch")
                           .SetSmallIcon(Resource.Drawable.xamagonBlue)
                           .Build();


                //StartForeground(Constants.SERVICE_RUNNING_NOTIFICATION_ID, notification);
            }
        }

        //백그라운드 서비스 등록
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