using System;
using System.Threading;
using WBA.Network;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA
{
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();

            MainPage = new MainPage();

            //소켓 연결
            {
                NetProcess.start();

                Device.StartTimer(new TimeSpan(0, 0, 0, 0, 300), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.Loop((MainPage)MainPage);
                    });
                    return true; //if true repeat
                });
                
            }
        }

        protected override void OnStart()
        {
            // Handle when your app starts
        }

        protected override void OnSleep()
        {
            // Handle when your app sleeps
        }

        protected override void OnResume()
        {
            // Handle when your app resumes
        }
    }
}
