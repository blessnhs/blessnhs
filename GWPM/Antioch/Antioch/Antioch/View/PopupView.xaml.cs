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
	public partial class PopupView : ContentView
	{
		ContentView _prevWindow;
		public PopupView (ContentView prevWindow,string Content)
		{
			InitializeComponent ();

			TextEditor.Text = Content;
			_prevWindow = prevWindow;
		}

		private async void OnCloseButtonClicked(object sender, EventArgs e)
		{
			var mainpage = (MainPage)Application.Current.MainPage;
			mainpage.LoadView(_prevWindow);
		}
	}
}