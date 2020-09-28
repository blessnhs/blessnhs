using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using WBA.Network;
using NetClient;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Setting : ContentPage
    {
        private double StepValue = 1.0;

        public Setting()
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

            MessagingCenter.Subscribe<Setting, CompletePacket>(this, "setting", (s, e) =>
            {
                LOGIN_RES res = new LOGIN_RES();
                res = LOGIN_RES.Parser.ParseFrom(e.Data);

                messageLabel.Text = res.VarCode.ToString();
            });
                                 
            usernameEntry.Focus();

            //이미 로그인 중이면 메세지 표시를 변경
            if(NetProcess.IsSuccessAuth == true)
            {
                messageLabel.Text = "접속중";
                usernameEntry.Text = NetProcess.UserId;
                loginbutton.IsVisible = false;
            }
        }



        async void OnLoginButtonClicked(object sender, EventArgs e)
        {
            User.Username = usernameEntry.Text;
            User.Password = passwordEntry.Text;

            if (User.Username == null || User.Password == null)
            {
                messageLabel.Text = "아이디와 비번을 다시 입력하세요";
                return;
            }

            NetProcess.SendLogin(User.Username, User.Password);

            if (User.Username == "강병욱" || User.Username == "nhs")
            {
                MainPage mainpage = Parent as MainPage;

                bool find = false;
                foreach (var child in mainpage.Children)
                {
                    if (child.Title == "찬송")
                        find = true;
                }
                if (find == false)
                    mainpage.Children.Add(new Hymn());
            }
        }

        void OnToggledKJV(object sender, ToggledEventArgs e)
        {
            User.CacheData.EnalbeKJV = e.Value;
            SQLLiteDB.Upsert(User.CacheData);
        }

        bool AreCredentialsCorrect()
        {
            // return user.Username == Constants.Username && user.Password == Constants.Password;
            return true;
        }
    }
}