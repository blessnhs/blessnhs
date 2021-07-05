using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class QnAView : ContentView
	{
		public QnAView ()
		{
			InitializeComponent ();
		}


		private async void OnSendButtonClicked(object sender, EventArgs e)
		{
			if(contents.Text == "")
            {
				await App.Current.MainPage.DisplayAlert("", "내용을 입력해 주세요.", "OK");
				return;
            }

			NetProcess.SendQNS(contents.Text);
			contents.Text = "";
			await App.Current.MainPage.DisplayAlert("", "전송되었습니다.", "OK");
		}

		private async void OnCloseButtonClicked(object sender, EventArgs e)
		{
			var mainpage = (MainPage)Application.Current.MainPage;
			mainpage.LoadLobby();
		}
	}
}