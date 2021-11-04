using System;

using Android.App;
using Android.Runtime;
using Android.Views;
using Android.OS;
using Android.Content;
using Android.Content.PM;
using Android.Support.V4.Content;
using System.Collections.Generic;
using Android;
using static Android.OS.PowerManager;

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


            Window.AddFlags(WindowManagerFlags.KeepScreenOn |
                      WindowManagerFlags.DismissKeyguard |
                      WindowManagerFlags.ShowWhenLocked |
                      WindowManagerFlags.TurnScreenOn);
         

            RequestPermissionsManually();

            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);
            LoadApplication(new App());

            Rg.Plugins.Popup.Popup.Init(this, savedInstanceState);

            RegisterForegroundService();

            unlockScreen();
        }
        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] Android.Content.PM.Permission[] grantResults)
        {
            Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            base.OnRequestPermissionsResult(requestCode, permissions, grantResults);
        }

        private void unlockScreen()
        { 
            WakeLock screenLock = ((PowerManager)GetSystemService(Context.PowerService)).NewWakeLock(WakeLockFlags.ScreenBright | WakeLockFlags.AcquireCausesWakeup, "tag");
            screenLock.Acquire();
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

        List<string> _permission = new List<string>();
        private void RequestPermissionsManually()
        {
            try
            {
                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.RecordAudio) != Permission.Granted)
                    _permission.Add(Manifest.Permission.RecordAudio);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.Camera) != Permission.Granted)
                    _permission.Add(Manifest.Permission.Camera);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.WriteExternalStorage) != Permission.Granted)
                    _permission.Add(Manifest.Permission.WriteExternalStorage);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.CallPhone) != Permission.Granted)
                    _permission.Add(Manifest.Permission.CallPhone);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.AccessNetworkState) != Permission.Granted)
                    _permission.Add(Manifest.Permission.AccessNetworkState);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.Internet) != Permission.Granted)
                    _permission.Add(Manifest.Permission.Internet);

                if (_permission.Count > 0)
                {
                    string[] array = _permission.ToArray();

                    RequestPermissions(array, array.Length);
                }

            }
            catch (Exception ex)
            {

            }
        }
    }
}