using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;

namespace CCA.Page
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class NotifyPopup : PopupPage
    {
        PopupPage _prevWindow;
        public NotifyPopup(PopupPage prevWindow, string Content)
        {
            InitializeComponent();

            TextEditor.Text = Content;
            _prevWindow = prevWindow;
        }

        private async void OnCloseButtonClicked(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }
    }
}