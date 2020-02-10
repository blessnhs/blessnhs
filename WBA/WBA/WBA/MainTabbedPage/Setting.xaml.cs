using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Internals;
using Xamarin.Forms.Xaml;
using SQLite;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Setting : ContentPage
    {
        private double StepValue = 1.0;

        public Setting()
        {
            InitializeComponent();

            TextSizeSlider.Value = SQLLiteDB.FontSize;

            TextSizeSlider.ValueChanged += (sender, args) =>
            {
                var newStep = Math.Round(args.NewValue / StepValue);

                Slider SliderMain = sender as Slider;

                SliderMain.Value = newStep * StepValue;

                SQLLiteDB.FontSize = (int)args.NewValue;

                TextSizeLabel.FontSize = (int)args.NewValue;

                UserCacheData data = new UserCacheData();
                data.FontSize = SQLLiteDB.FontSize;
                data.Id = 1;
                data.BibleName = SQLLiteDB.CacheData.BibleName;
                data.Chapter = SQLLiteDB.CacheData.Chapter;
                data.Verse = SQLLiteDB.CacheData.Verse;
                data.UserName = SQLLiteDB.CacheData.UserName;
                data.Passwd = SQLLiteDB.CacheData.Passwd;

                SQLLiteDB.Upsert(data);
            };
        }
     
        async void OnLoginButtonClicked(object sender, EventArgs e)
        {
            var user = new User
            {
                Username = usernameEntry.Text,
                Password = passwordEntry.Text
            };

            var isValid = AreCredentialsCorrect(user);
            if (isValid)
            {
                messageLabel.Text = "Login Success";
            }
            else
            {
                messageLabel.Text = "Login failed";
                passwordEntry.Text = string.Empty;
            }
        }

        bool AreCredentialsCorrect(User user)
        {
            // return user.Username == Constants.Username && user.Password == Constants.Password;
            return true;
        }
    }
}