using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View.Chat
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class ChatUserList : PopupPage
    {
		public ChatUserList (HashSet<string>List)
		{
			InitializeComponent ();


            listView.ItemsSource = List;
        }

        private async void OnClose(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }


	}
}