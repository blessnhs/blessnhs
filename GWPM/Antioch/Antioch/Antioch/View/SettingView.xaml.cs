using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SettingView : ContentView
    {
        public string message = "";

        private double StepValue = 1.0;
        public SettingView()
        {
            InitializeComponent();

            TextSizeSlider.Value = User.CacheData.FontSize;

            TextSizeSlider.ValueChanged += (sender, args) =>
            {
                var newStep = Math.Round(args.NewValue / StepValue);

                Slider SliderMain = sender as Slider;

                SliderMain.Value = newStep * StepValue;

                User.CacheData.FontSize = (int)args.NewValue;

                TextSizeLabel.FontSize = (int)args.NewValue;

                SQLLiteDB.Upsert(User.CacheData);
            };

            KJVOption.IsToggled = User.CacheData.EnalbeKJV;

            usernameEntry.Focus();

            //이미 로그인 중이면 메세지 표시를 변경
            if (User.LoginSuccess == true)
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    message = "접속중";
                    usernameEntry.Text = User.CacheData.UserName;
                    passwordEntry.Text = "****";
                    loginbutton.IsVisible = false;
                });
               
            }

            BindingContext = this;
        }

        void OnToggledKJV(object sender, ToggledEventArgs e)
        {
            User.CacheData.EnalbeKJV = e.Value;
            SQLLiteDB.Upsert(User.CacheData);
        }

        async void Clicked(object sender, System.EventArgs e)
        {
            User.Username = usernameEntry.Text;
            User.Password = passwordEntry.Text;
            NetProcess.SendLogin(usernameEntry.Text, passwordEntry.Text);
        }
    }
}