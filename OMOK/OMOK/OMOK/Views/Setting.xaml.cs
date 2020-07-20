using OMOK.Network;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Setting : ContentPage
    {
        public Setting()
        {
            InitializeComponent();

            if (User.myInfo.PhotoPath != null)
                Profile.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));


            RecordLabel.Text = User.myInfo.win + "승" + User.myInfo.lose + "패" + User.myInfo.draw + "무승부";

            RankLabel.Text = "현재 순위 " + User.myInfo.rank + " 위";
        }

        
         async void OnCloseButtonClicked(object sender, EventArgs e)
        {
            Navigation.PopModalAsync();
        }
        async void OnSendButtonClicked(object sender, EventArgs e)
        {
            NetProcess.SendQNS(contents.Text);

            await DisplayAlert("", "전송하였습니다..\n", "OK");

            contents.Text = "";
        }
        async void OnOptionClicked(object sender, EventArgs e)
        {
            GlobalVariable.ip = ip.Text;
            GlobalVariable.port = Convert.ToInt32(port.Text);
        }
        
    }

}