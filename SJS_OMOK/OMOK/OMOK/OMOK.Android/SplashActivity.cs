using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.Gms.Auth.Api;
using Android.Gms.Auth.Api.SignIn;
using Android.Gms.Common.Apis;
using Android.OS;
using Android.Runtime;
using Android.Support.V7.App;
using Android.Views;
using Android.Widget;
using Firebase;
using Firebase.Auth;

namespace OMOK.Droid
{
    [Activity(Theme = "@style/MyTheme.Splash", MainLauncher = true, NoHistory = true)]
    public class SplashActivity : AppCompatActivity
    {
        public FirebaseAuth FirebaseAuth_ { get; private set; }
        public GoogleApiClient GoogleApiClient { get; private set; }

        public override void OnCreate(Bundle savedInstanceState, PersistableBundle persistentState)
        {

            base.OnCreate(savedInstanceState, persistentState);
        }


        bool once = false;
        // Launches the startup task
        protected override void OnResume()
        {
            base.OnResume();
            if (once == true)
                return;
            Task startupWork = new Task(() => { SimulateStartup(); });
            startupWork.Start();

            once = true;
        }


        // Prevent the back button from canceling the startup process
        public override void OnBackPressed() { }


        // Simulates background work that happens behind the splash screen
        void SimulateStartup()
        {
            StartActivity(new Intent(Application.Context, typeof(MainActivity)));
            //FirebaseApp.InitializeApp(Application.Context);

            //FirebaseAuth_ = FirebaseAuth.Instance;

            //if (FirebaseAuth_ == null)
            //    FirebaseAuth_ = new FirebaseAuth(FirebaseApp.Instance);

            //GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DefaultSignIn)
            //    .RequestIdToken("926850429943-envuu4ga9i133mbaq5hd77g1b9bdcrj5.apps.googleusercontent.com")
            //    .RequestEmail()
            //    .Build();
            //GoogleApiClient = new GoogleApiClient.Builder(this)
            //    .EnableAutoManage(this, null)
            //    .AddApi(Auth.GOOGLE_SIGN_IN_API, signInOptions)
            //    .Build();


            //GoogleSignIn();
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
            base.OnDestroy();
            //   FirebaseAuth_.SignOut();
        }

        async void ProcessSignInResult(Intent data)
        {
            GoogleSignInResult signInResult = Auth.GoogleSignInApi.GetSignInResultFromIntent(data);
            if (signInResult.IsSuccess)
            {
                AuthCredential credential = GoogleAuthProvider.GetCredential(signInResult.SignInAccount.IdToken, null);
                try
                {
                    IAuthResult authResult = await FirebaseAuth_.SignInWithCredentialAsync(credential);
                    FirebaseUser user = authResult.User;

                    User.myInfo.PhotoPath = signInResult.SignInAccount.PhotoUrl.ToString();

                    User.Uid = signInResult.SignInAccount.Id;
                    User.Token = signInResult.SignInAccount.IdToken;
                    User.myInfo.NickName = signInResult.SignInAccount.DisplayName;

                    StartActivity(new Intent(Application.Context, typeof(MainActivity)));


                }
                catch (Exception ex)
                {
                //    new Handler(MainLooper).Post(() => new Android.App.AlertDialog.Builder(this).SetMessage("var_TeamRecord 등록 실패\n\n" + ex).Show());
                }

            }
            else
            {
                StartActivity(new Intent(Application.Context, typeof(MainActivity)));

            }
        }
    }
}
