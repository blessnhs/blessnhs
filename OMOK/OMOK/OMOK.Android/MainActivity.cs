using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using Firebase.Auth;
using Android.Gms.Common.Apis;
using Firebase;
using Android.Gms.Auth.Api.SignIn;
using Android.Gms.Auth.Api;
using Android.Content;
using Android.Gms.Ads;
using OMOK.Network;
using System.Text;
using System.Collections.Generic;
using Android.Support.V4.Content;
using Android;

namespace OMOK.Droid
{
    [Activity(Label = "OMOK",ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation, ScreenOrientation = ScreenOrientation.UserPortrait)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        public FirebaseAuth FirebaseAuth_ { get; private set; }
        public GoogleApiClient GoogleApiClient { get; private set; }

        protected override void OnCreate(Bundle savedInstanceState)
        {
            TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.Window.RequestFeature(WindowFeatures.ActionBar);
            base.SetTheme(Resource.Style.MainTheme);

            base.OnCreate(savedInstanceState);

            RequestPermissionsManually();

            MyApplication.activity = this;

            OxyPlot.Xamarin.Forms.Platform.Android.PlotViewRenderer.Init();

            Rg.Plugins.Popup.Popup.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.SetFlags("RadioButton_Experimental");
            //테스트 광고가 아닐 경우 APP ID를 넣어줘야 한다.
            MobileAds.Initialize(ApplicationContext, /*"YOUR ANDROID APP ID HERE"*/"ca-app-pub-9541028236702321~9544639066");

            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);


        }

        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] Android.Content.PM.Permission[] grantResults)
        {
            Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            base.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            FirebaseApp.InitializeApp(Application.Context);

            FirebaseAuth_ = FirebaseAuth.Instance;

            if (FirebaseAuth_ == null)
                FirebaseAuth_ = new FirebaseAuth(FirebaseApp.Instance);

            GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DefaultSignIn)
                .RequestIdToken("926850429943-envuu4ga9i133mbaq5hd77g1b9bdcrj5.apps.googleusercontent.com")
                .RequestEmail()
                .Build();
            GoogleApiClient = new GoogleApiClient.Builder(this)
                .EnableAutoManage(this, null)
                .AddApi(Auth.GOOGLE_SIGN_IN_API, signInOptions)
                .Build();

            GoogleSignIn();
            LoadApplication(new App());

        }

        const int SignInRequestCode = 9001;

        public void GoogleSignIn()
        {
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

        protected override void OnDestroy()
        {
            FirebaseAuth_.SignOut();
        }

        async void ProcessSignInResult(Intent data)
        {
             GoogleSignInResult signInResult = Auth.GoogleSignInApi.GetSignInResultFromIntent(data);
            if (signInResult.IsSuccess)
            {
                AuthCredential credential = GoogleAuthProvider.GetCredential(signInResult.SignInAccount.IdToken, null);
                try
                {
                    User.myInfo.PhotoPath = signInResult.SignInAccount.PhotoUrl.ToString();

                    User.Uid = signInResult.SignInAccount.Id;
                    User.Token = signInResult.SignInAccount.IdToken;
                    User.myInfo.NickName = signInResult.SignInAccount.DisplayName;

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