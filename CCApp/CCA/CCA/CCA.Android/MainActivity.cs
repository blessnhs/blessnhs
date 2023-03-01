using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.OS;
using static Android.OS.PowerManager;
using System.Collections.Generic;
using AndroidX.Core.Content;
using Android;
using Xamarin.Forms;
using System;
using Android.Net.Wifi;
using Android.Runtime;
using Android.Views;
using Firebase.Auth;
using Android.Gms.Common.Apis;
using Firebase;
using Android.Gms.Auth.Api.SignIn;
using Android.Gms.Auth.Api;
using Android.Content;
using Android.Gms.Common.Api.Internal;
using Rg.Plugins.Popup.Services;
using Android.Gms.Ads;
using System.Threading.Tasks;
using Android.Gms.Common;
using Android.Gms.Auth;
using Plugin.InAppBilling;
using DependencyHelper.Droid;
using Xamarin.Essentials;
using rtaNetworking.Streaming;

namespace CCA.Droid
{
   
    [Activity(Label = "CCA", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize ,ScreenOrientation = ScreenOrientation.UserPortrait)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        static WakeLock lockctl;
        static WifiManager.WifiLock lockwifi;

        public static Context context;
        public static MainActivity activity;

        public static int BatteryLevel;

        Intent startServiceIntent;
        Intent stopServiceIntent;
        bool isStarted = false;


        public static ImageStreamingServer server = new ImageStreamingServer();


        protected override void OnCreate(Bundle savedInstanceState)
        {
            
            base.OnCreate(savedInstanceState);

            if (savedInstanceState != null)
            {
                isStarted = savedInstanceState.GetBoolean(Constants.SERVICE_STARTED_KEY, false);
            }


            context = this;
            activity = this;

            server.Start(1801);

            Window.AddFlags(WindowManagerFlags.KeepScreenOn |
             WindowManagerFlags.DismissKeyguard |
             WindowManagerFlags.ShowWhenLocked |
             WindowManagerFlags.TurnScreenOn
             );

            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.OMr1)
            {
                SetShowWhenLocked(true);
                SetTurnScreenOn(true);
            }
            else 
            {
            }
 
            //CrossInAppBilling.Current.InTestingMode = true;


            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);

            Rg.Plugins.Popup.Popup.Init(this);

            MobileAds.Initialize(ApplicationContext, /*"YOUR ANDROID APP ID HERE"*/"ca-app-pub-9541028236702321~7685624496");

            UnlockCPU_WIFI();

            RequestPermissionsManually();

            BatteryCheckReg();

//            CreateService();

            DeviceDisplay.KeepScreenOn = true;
        }

        private void BatteryCheckReg()
        {
            //battery refresh check
            Device.StartTimer(TimeSpan.FromSeconds(10), () =>
            {
                var filter = new IntentFilter(Intent.ActionBatteryChanged);
                var battery = RegisterReceiver(null, filter);
                int level = battery.GetIntExtra(BatteryManager.ExtraLevel, -1);
                int scale = battery.GetIntExtra(BatteryManager.ExtraScale, -1);

                BatteryLevel = (int)System.Math.Floor(level * 100D / scale);

                return true; // return true to repeat counting, false to stop timer
            });
        }

        public FirebaseAuth FirebaseAuth_ { get; private set; }
        public GoogleApiClient GoogleApiClient { get; private set; }

        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] Android.Content.PM.Permission[] grantResults)
        {
            try
            {
                Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

                base.OnRequestPermissionsResult(requestCode, permissions, grantResults);

                FirebaseApp.InitializeApp(context);

                FirebaseAuth_ = FirebaseAuth.Instance;

                if (FirebaseAuth_ == null)
                    FirebaseAuth_ = new FirebaseAuth(FirebaseApp.Instance);

                GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DefaultSignIn)
                    .RequestIdToken("926850429943-envuu4ga9i133mbaq5hd77g1b9bdcrj5.apps.googleusercontent.com")
                    .RequestEmail()
                    .RequestId()
                    .Build();
                GoogleApiClient = new GoogleApiClient.Builder(this)
                    .AddApi(Auth.GOOGLE_SIGN_IN_API, signInOptions)
                    .Build();


                GoogleApiClient.Connect();

                GoogleSignIn();
                LoadApplication(new App());
            }
            catch(Exception ex)
            {
                Method_Android.NotificationException(ex);
            }
        }

        const int SignInRequestCode = 9001;

        public void GoogleSignIn()
        {
            try
            {
                Intent signInIntent = Auth.GoogleSignInApi.GetSignInIntent(GoogleApiClient);
                StartActivityForResult(signInIntent, SignInRequestCode);
            }
            catch (Exception ex)
            {
                Method_Android.NotificationException(ex);
            }
        }

        public void GoolgeLogout()
        {
            NetProcess.client.ClearSocket();

            User.Clear();

            FirebaseAuth_.SignOut();
            Auth.GoogleSignInApi.SignOut(GoogleApiClient);
            Intent signInIntent = Auth.GoogleSignInApi.GetSignInIntent(GoogleApiClient);
            StartActivityForResult(signInIntent, SignInRequestCode);
        }

        protected override void OnActivityResult(int requestCode, Result resultCode, Intent data)
        {
            base.OnActivityResult(requestCode, resultCode, data);

            switch (requestCode)
            {
                case SignInRequestCode:
                    ProcessSignInResult(data);
                    break;
            }
        }

        public override void OnActivityReenter(int resultCode, Intent data)
        {
            base.OnActivityReenter(resultCode, data);
        }
        
        public async override void OnBackPressed()
        {
            if (Rg.Plugins.Popup.Popup.SendBackPressed(base.OnBackPressed))
            {
                await PopupNavigation.Instance.PopAsync();
            }
            else
            {
                // Do something if there are not any pages in the `PopupStack`
            }
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();

            FirebaseAuth_?.SignOut();

            lockctl?.Release();
            lockctl = null;
            lockwifi?.Release();
            lockwifi = null;

            //           NetProcess.SendStopStream();

            EndService();

            server?.Stop();
        }

        async void ProcessSignInResult(Intent data)
        {
            try
            {
                GoogleSignInResult signInResult = Auth.GoogleSignInApi.GetSignInResultFromIntent(data);

                if (signInResult == null) return;

                int statusCode = signInResult.Status.StatusCode;

                if (signInResult.IsSuccess)
                {
                    AuthCredential credential = GoogleAuthProvider.GetCredential(signInResult.SignInAccount.IdToken, null);

                    User.ProfileUrl = signInResult.SignInAccount.PhotoUrl.ToString();

                    User.Clear();

                    User.Uid = signInResult.SignInAccount.Id;
                    User.Token = signInResult.SignInAccount.IdToken;
                    User.Name = signInResult.SignInAccount.DisplayName;
                    User.EMail = signInResult.SignInAccount.Email;

                    NetProcess.IsActivate = true;

                    IAuthResult authResult = await FirebaseAuth_.SignInWithCredentialAsync(credential);
                    FirebaseUser user = authResult.User;
                }
                else
                {
                }
            }
            catch (Exception ex)
            {
                Method_Android.NotificationException(ex);
            }
        }

        private void UnlockCPU_WIFI()
        {
            //cpu가 저절전 모드로 들어가지 않게한다.
            lockctl = ((PowerManager)GetSystemService(Android.Content.Context.PowerService)).NewWakeLock(
               WakeLockFlags.Partial, "tag"); //cpu 항상 on 저절전 모드 해제 화면은 꺼짐 키보드 꺼짐
            lockctl.Acquire();

            //와이파이가 저절전 모드로 들어가지 않게한다.
            WifiManager wifi = ((WifiManager)GetSystemService(Android.Content.Context.WifiService));
            lockwifi = wifi.CreateWifiLock(Android.Net.WifiMode.Full, "wifilock");
            lockwifi.SetReferenceCounted(true);
            lockwifi.Acquire();
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

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.RequestIgnoreBatteryOptimizations) != Permission.Granted)
                    _permission.Add(Manifest.Permission.RequestIgnoreBatteryOptimizations);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.BatteryStats) != Permission.Granted)
                    _permission.Add(Manifest.Permission.BatteryStats);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.ForegroundService) != Permission.Granted)
                    _permission.Add(Manifest.Permission.ForegroundService);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.WakeLock) != Permission.Granted)
                    _permission.Add(Manifest.Permission.WakeLock);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.AccessWifiState) != Permission.Granted)
                    _permission.Add(Manifest.Permission.AccessWifiState);

                if (ContextCompat.CheckSelfPermission(this, Manifest.Permission.Flashlight) != Permission.Granted)
                    _permission.Add(Manifest.Permission.Flashlight);

                if (_permission.Count > 0)
                {
                    string[] array = _permission.ToArray();

                    RequestPermissions(array, array.Length);
                }

            }
            catch (Exception ex)
            {
                FirebaseApp.InitializeApp(context);

                FirebaseAuth_ = FirebaseAuth.Instance;

                if (FirebaseAuth_ == null)
                    FirebaseAuth_ = new FirebaseAuth(FirebaseApp.Instance);

                GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DefaultSignIn)
                    .RequestIdToken("926850429943-envuu4ga9i133mbaq5hd77g1b9bdcrj5.apps.googleusercontent.com")
                    .RequestEmail()
                    .RequestId()
                    .Build();
                GoogleApiClient = new GoogleApiClient.Builder(this)
                    .AddApi(Auth.GOOGLE_SIGN_IN_API, signInOptions)
                    .Build();


                GoogleApiClient.Connect();

                GoogleSignIn();
                LoadApplication(new App());
            }
        }

        private void CreateService()
        {

            OnNewIntent(this.Intent);

            startServiceIntent = new Intent(this, typeof(ServiceCamera));
            startServiceIntent.SetAction(Constants.ACTION_START_SERVICE);

            stopServiceIntent = new Intent(this, typeof(ServiceCamera));
            stopServiceIntent.SetAction(Constants.ACTION_STOP_SERVICE);
     
            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {
                StartForegroundService(startServiceIntent);
            }
            else
            {
                StartService(startServiceIntent);
            }

        }

        private void EndService()
        {
            StopService(stopServiceIntent);

        }

    }
}