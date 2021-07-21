#define GLOBAL


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

            SQLLiteDB.LoadCacheData();

            MainPage = new MainPage();
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
