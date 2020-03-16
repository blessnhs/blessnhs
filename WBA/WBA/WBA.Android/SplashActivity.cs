using System;
using System.IO;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Support.V7.App;
using Android.Util;
using Plugin.LocalNotifications;
using WBA.MainTabbedPage;

namespace WBA.Droid
{
    [Activity(Theme = "@style/MyTheme.Splash", MainLauncher = true, NoHistory = true)]
    public class SplashActivity : AppCompatActivity
    {
        static readonly string TAG = "X:" + typeof (SplashActivity).Name;

        public override void OnCreate(Bundle savedInstanceState, PersistableBundle persistentState)
        {
            base.OnCreate(savedInstanceState, persistentState);
            Log.Debug(TAG, "SplashActivity.OnCreate");
        }

        // Launches the startup task
        protected override void OnResume()
        {
            base.OnResume();
            Task startupWork = new Task(() => { SimulateStartup(); });
            startupWork.Start();
        }

        // Prevent the back button from canceling the startup process
        public override void OnBackPressed() { }

        private void LoadNoticeData()
        {
            try
            {
                var NoticeFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "notice.txt");
                GoolgeService.DownloadFileFromURLToPath("https://drive.google.com/file/d/1NSS2P2ECkU6QdvEj0cX3TVDveIMwyn01/view?usp=sharing", NoticeFile);

                var worship = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "worship.txt");
                GoolgeService.DownloadFileFromURLToPath("https://drive.google.com/file/d/1EamFoQDhKCminaJRFfKGU1OB50lobXos/view?usp=sharing", worship);
            }
            catch (Exception e)
            {
#if DEBUG
                CrossLocalNotifications.Current.Show("Google Shared Drive Failed", DateTime.Now.ToString(), 0, DateTime.Now);
#endif
            }
        }

        private void LoadCacheUserData()
        {
            try
            {
                SQLLiteDB.LoadCacheData();

                //성경 읽기계획이 없는 경우 디폴트로 분당에서 하는 것으로 설정한다. 
                var plan = SQLLiteDB.ReadBibleReadPlan();
                if (plan == null)
                {
                    DateTime StartTime = new DateTime(2020, 1, 13);
                    SQLLiteDB.InsertBibleReadPlan(StartTime.Date, "마태복음", 1);
                }
            }
            catch (Exception e)
            {
#if DEBUG
                CrossLocalNotifications.Current.Show("SQLLiteDB.LoadCacheData Failded", DateTime.Now.ToString(), 0, DateTime.Now);
#endif
            }
        }

        private void LoadResourceData()
        {
            try
            {
                BibleInfo.LoadKRV();
                BibleInfo.LoadKJV();
                Dic.LoadDic();
                Hymn.LoadList();
                //BibleInfo.LoadNIV();
                //BibleInfo.CheckValidate();
            }
            catch (Exception e)
            {
#if DEBUG
                CrossLocalNotifications.Current.Show("SQLLiteDB.LoadKRV Failded", DateTime.Now.ToString(), 0, DateTime.Now);
#endif
            }
        }

        // Simulates background work that happens behind the splash screen
        async void SimulateStartup ()
        {
            LoadNoticeData();
            LoadCacheUserData();
            LoadResourceData();

            StartActivity(new Intent(Application.Context, typeof (MainActivity)));
        }
    }
}