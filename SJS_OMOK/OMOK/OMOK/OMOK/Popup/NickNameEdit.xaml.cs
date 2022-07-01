using OMOK.Network;
using Rg.Plugins.Popup.Pages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Popup
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class NickNameEdit : PopupPage
    {
        public NickNameEdit()
        {
            InitializeComponent();
            if (User.Locale != "ko")
                NickNameLabel.Text = "Please enter your nickname (7 Characters)";
            else
                NickNameLabel.Text = "닉네임을 입력하세요(7자 이내)";

            MessagingCenter.Subscribe<NickNameEdit>(this, "Fail", (sender) =>
            {
                if (User.Locale != "ko")
                    NickNameLabel.Text = "Exist NickName Please Retry your nickname (7 Characters)";
                else
                    NickNameLabel.Text = "존재하는 닉네임 입니다";

                NickNameLabel.TextColor = Color.Red;
                UsernameEntry.Text = "";
            });
        }

        async void OnClick(object sender, System.EventArgs e)
        {
            if(UsernameEntry.Text == null)
            {
                if (User.Locale != "ko")
                    NickNameLabel.Text = "Please Retry your nickname (7 Characters)";
                else
                    NickNameLabel.Text = "닉네임을 다시 입력하세요.";

                UsernameEntry.Text = "";
                return;
            }

            if(UsernameEntry.Text.Length > 20)
            {
                if (User.Locale != "ko")
                    NickNameLabel.Text = " your nickname Too Long Name";
                else
                    NickNameLabel.Text = "닉네임 길이가 너무 깁니다.다시 입력하세요";
                UsernameEntry.Text = "";
                return;
            }

            if(UsernameEntry.Text.Length == 0)
            {
                if (User.Locale != "ko")
                    NickNameLabel.Text = "Please Retry your nickname (7 Characters)";
                else
                    NickNameLabel.Text = "닉네임을 다시 입력하세요.";

                UsernameEntry.Text = "";
                return;
            }

            if(UsernameEntry.Text.IndexOf(' ') != -1)
            {
                if (User.Locale != "ko")
                    NickNameLabel.Text = "Please Retry your nickname (7 Characters)";
                else
                    NickNameLabel.Text = "닉네임을 다시 입력하세요.";

                UsernameEntry.Text = "";
                return;
            }


            NetProcess.SendCheckNickName(UsernameEntry.Text);
        }
    }
}