using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace CCA.Page
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class QnAPage : PopupPage
	{
		public QnAPage ()
		{
			InitializeComponent ();

			var mainDisplayInfo = DeviceDisplay.MainDisplayInfo;
			var width = mainDisplayInfo.Width;
		

		}


		private async void OnSendButtonClicked(object sender, EventArgs e)
		{
			if(Contents.Text == "")
            {
				await App.Current.MainPage.DisplayAlert("", "내용을 입력해 주세요.", "OK");
				return;
            }

			NetProcess.SendQNS(Contents.Text);
			Contents.Text = "";
			await App.Current.MainPage.DisplayAlert("", "전송되었습니다.", "OK");
		}

		private async void OnCloseButtonClicked(object sender, EventArgs e)
		{
			await PopupNavigation.Instance.PopAsync();
		}
	}
}