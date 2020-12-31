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

            //언어 수동 변환
            if (User.Locale != "ko")
            {
                QNSLabel.Text = "Question & Bug Report";
                sendbutton.Text = "Send";
                closebutton.Text = "Close";

                RecordLabel.Text = User.myInfo.win + " Win " + User.myInfo.lose + " Lose " + User.myInfo.draw + " Draw ";

                RankLabel.Text = "Rank " + User.myInfo.rank ;
            }
            
        }

        
         async void OnCloseButtonClicked(object sender, EventArgs e)
        {
            Navigation.PopModalAsync();
        }
        async void OnSendButtonClicked(object sender, EventArgs e)
        {
            if(contents.Text.Length == 0)
            {
                await DisplayAlert("", "Transfer complete..\n", "OK");
                return;

            }

            if (contents.Text == "blessnhs")
            {
                User.IsEnableScreenChat = true;
            }
            if (contents.Text == "blessnhs2")
                User.Auto = true;

                NetProcess.SendQNS(contents.Text);

            await DisplayAlert("", "Transfer complete..\n", "OK");

            contents.Text = "";
        }
        async void OnOptionClicked(object sender, EventArgs e)
        {
            //GlobalVariable.ip = ip.Text;
            //GlobalVariable.port = Convert.ToInt32(port.Text);
        }
        
    }

}