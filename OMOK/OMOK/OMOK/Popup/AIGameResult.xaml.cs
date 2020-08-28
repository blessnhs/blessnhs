using Rg.Plugins.Popup.Extensions;
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
    public partial class AIGameResult : PopupPage
    {
        public AIGameResult(int Result)
        {
            InitializeComponent();

            if (Result == 0)
                Label1.Text = "승리 하셨습니다.";
            else
                Label1.Text = "패배 하였습니다.";

            if (User.myInfo.PhotoPath != null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

        }

        async void OnClosedClicked(object sender, System.EventArgs e)
        {
            await Navigation.PopAllPopupAsync();
        }
    }
}