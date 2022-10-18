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

namespace CCA.Droid
{
    [Activity(Label = "CCA", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize )]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        public static Context context;
        public static MainActivity activity;

        public static int BatteryLevel;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            
            base.OnCreate(savedInstanceState);

            context = this;
            activity = this;
            Window.AddFlags(WindowManagerFlags.KeepScreenOn |
                WindowManagerFlags.DismissKeyguard |
                WindowManagerFlags.ShowWhenLocked |
                WindowManagerFlags.TurnScreenOn);

            
            PowerManager pm = (PowerManager)GetSystemService(Context.PowerService);
            PowerManager.WakeLock wl = pm.NewWakeLock(WakeLockFlags.Full | WakeLockFlags.AcquireCausesWakeup, "");
            wl.Acquire();
            

            //string packageName = "com.blessnhs.cca";
            //PowerManager pm = (PowerManager)GetSystemService(Context.PowerService);
         
            //if(pm.IsIgnoringBatteryOptimizations(packageName))
            //{
            //    Intent i = new Intent();
            //    i.SetAction(Android.Provider.Settings.ActionRequestIgnoreBatteryOptimizations);
            //    i.SetData(Android.Net.Uri.Parse("package:" + packageName));

            //    StartActivity(i);
            //}

            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);

            Rg.Plugins.Popup.Popup.Init(this);

            MobileAds.Initialize(ApplicationContext, /*"YOUR ANDROID APP ID HERE"*/"ca-app-pub-9541028236702321~7685624496");

            UnlockScreen();

            RequestPermissionsManually();


            BatteryCheckReg();
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

        const int SignInRequestCode = 9001;

        public void GoogleSignIn()
        {
            Intent signInIntent = Auth.GoogleSignInApi.GetSignInIntent(GoogleApiClient);
            StartActivityForResult(signInIntent, SignInRequestCode);
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

            FirebaseAuth_.SignOut();

 //           NetProcess.SendStopStream();
        }

        async void ProcessSignInResult(Intent data)
        {
            GoogleSignInResult signInResult = Auth.GoogleSignInApi.GetSignInResultFromIntent(data);
            if (signInResult.IsSuccess)
            {
                AuthCredential credential = GoogleAuthProvider.GetCredential(signInResult.SignInAccount.IdToken, null);
                try
                {
                    User.ProfileUrl = signInResult.SignInAccount.PhotoUrl.ToString();

                    User.Uid = signInResult.SignInAccount.Id;
                    User.Token = signInResult.SignInAccount.IdToken;
                    User.Name = signInResult.SignInAccount.DisplayName;
                    User.EMail = signInResult.SignInAccount.Email;

                    IAuthResult authResult = await FirebaseAuth_.SignInWithCredentialAsync(credential);
                    FirebaseUser user = authResult.User;

                }
                catch (Exception ex)
                {
                    //                    new Handler(MainLooper).Post(() => new AlertDialog.Builder(this).SetMessage("파이어베이스 등록 실패\n\n" + ex).Show());
                }

            }
            else
            {


            }
        }

        private void UnlockScreen()
        {
            WakeLock screenLock = ((PowerManager)GetSystemService(Android.Content.Context.PowerService)).NewWakeLock(WakeLockFlags.ScreenBright | WakeLockFlags.AcquireCausesWakeup, "tag");
            screenLock.Acquire();
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
                    _permission.Add(Manifest.Permission.RequestIgnoreBatteryOptimizations);

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
    }
}