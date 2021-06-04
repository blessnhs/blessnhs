using System;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch
{
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();

            MainPage = new MainPage();

            //network
            {

                //network thread
                Task.Run(() =>
                {
                    while (true)
                    {
                        NetProcess.start();
                        NetProcess.client.PacketRecvSync();
                    }
                });

                //network thread
                Task.Run(() =>
                {
                    DateTime checktime = DateTime.Now;

                    while (true)
                    {
                        NetProcess.Loop();
                    }
                });

            }
        }


        protected override void OnStart()
        {
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
