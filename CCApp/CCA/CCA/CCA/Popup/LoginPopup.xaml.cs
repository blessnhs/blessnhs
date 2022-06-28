using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace CCA.Popup
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class LoginPopup : PopupPage
    {
        public LoginPopup()
        {
            InitializeComponent();
        }


        private async void OnCloseButtonTapped(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }
        private async void OnLogin(object sender, EventArgs e)
        {
           // await PopupNavigation.Instance.PopAsync();

        
        }
    }
}