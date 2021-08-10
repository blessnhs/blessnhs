using Antioch.View.Chat.ViewModels;
using DependencyHelper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SettingView : ContentView
    {
        SettingViewModel vm = new SettingViewModel();
        private double StepValue = 1.0;
        public SettingView()
        {
            InitializeComponent();

            vm.BtnMessage = "Login";
            vm.Version = "Newest version";

            {
                var currentVersion = VersionTracking.CurrentVersion;

                double myversion = 0;
                double.TryParse(currentVersion,out myversion);
                const Double Eps = 0.000000000000001;

                if (Math.Abs(User.Version - myversion) > Eps)
                {
                    vm.Version = "Updatable";
                }
            }

            TextSizeSlider.Value = User.CacheData.FontSize;

            TextSizeSlider.ValueChanged += (sender, args) =>
            {
                var newStep = Math.Round(args.NewValue / StepValue);

                Slider SliderMain = sender as Slider;

                SliderMain.Value = newStep * StepValue;

                User.CacheData.FontSize = (int)args.NewValue;

                TextSizeLabel.FontSize = (int)args.NewValue;

                SQLLiteDB.Upsert(User.CacheData);

                Device.BeginInvokeOnMainThread(() =>
                {
                    User.OnceVersionNotify = true;
                    DependencyService.Get<Toast>().Notification("New Version Updated");
                });
            };

            KJVOption.IsToggled = User.CacheData.EnalbeKJV;

            usernameEntry.Focus();

            //이미 로그인 중이면 메세지 표시를 변경
            if (User.LoginSuccess == true)
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    messageLabel.Text = "Conncted";
                    usernameEntry.Text = User.CacheData.UserName;
                    passwordEntry.Text = "****";

                    vm.BtnMessage = "Conncted";
                });
               
            }

            BindingContext = vm;
        }

        public void UpdateLoginState(string message)
        {
            vm.Message = message;
            vm.BtnMessage = message;
        }

        public void UpdateNameEntry(string message)
        {
            usernameEntry.Text = message;
        }

        async void OnCheckVersion(object sender, System.EventArgs e)
        {
            var currentVersion = VersionTracking.CurrentVersion;

            double myversion;
            double.TryParse(currentVersion,out myversion);
            const Double Eps = 0.000000000000001;

            if (Math.Abs(User.Version - myversion) > Eps)
            {
                Xamarin.Essentials.Browser.OpenAsync("https://play.google.com/store/apps/details?id=antioch.kor.pkg");
                return;
            }
        }
        
        void OnToggledKJV(object sender, ToggledEventArgs e)
        {

#if GLOBAL
            User.CacheData.EnalbeKJV = true;
#else
    User.CacheData.EnalbeKJV = e.Value;
#endif


            //    SQLLiteDB.Upsert(User.CacheData);
        }


        async void login_btn(object sender, System.EventArgs e)
        {
            User.CacheData.UserName = usernameEntry.Text;
            User.CacheData.Passwd = passwordEntry.Text;
            NetProcess.SendLogin(usernameEntry.Text, passwordEntry.Text);
        }
    }
}