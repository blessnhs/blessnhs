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

            ////이미 로그인 중이면 메세지 표시를 변경
            //if (NetProcess.IsSuccessAuth == true)
            //{
            //    messageLabel.Text = "접속중";
            //    usernameEntry.Text = NetProcess.UserId;
            //    loginbutton.IsVisible = false;
            //}
        }

        void OnToggledKJV(object sender, ToggledEventArgs e)
        {
            User.CacheData.EnalbeKJV = e.Value;
            SQLLiteDB.Upsert(User.CacheData);
        }

        async void Clicked(object sender, System.EventArgs e)
        {
            NetProcess.SendLogin(usernameEntry.Text, passwordEntry.Text);
        }
    }
}