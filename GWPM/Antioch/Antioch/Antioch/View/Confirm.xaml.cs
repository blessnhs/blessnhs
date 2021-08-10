using Rg.Plugins.Popup.Extensions;
using Rg.Plugins.Popup.Pages;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Confirm : PopupPage
    {
    private    Confirm(string message)
        {
            InitializeComponent();


#if GLOBAL
            {
                Label1.Text = message;
                OKButton.Text = "OK";
                CancelButton.Text = "Cancel";
            }
#else
            {
                Label1.Text = message;
                OKButton.Text = "확인";
                CancelButton.Text = "취소";
            }
#endif
        }

        public Confirm()
        {
        }

        void OnYesClicked(object sender, System.EventArgs e)
        {
            Navigation.PopAllPopupAsync();
        }

        void OnNoClicked(object sender, System.EventArgs e)
        {
             Navigation.PopAllPopupAsync();
        }
    }
}