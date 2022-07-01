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
        public AIGameResult(bool Result)
        {
            InitializeComponent();

            if (Result == true)
                Label1.Text = "Veictory";
            else
                Label1.Text = "Defeat";

            if (User.myInfo.PhotoPath != null)
                mypicture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

        }

    async void OnClosedClicked(object sender, System.EventArgs e)
        {
            await Navigation.PopAllPopupAsync();
        }
    }
}