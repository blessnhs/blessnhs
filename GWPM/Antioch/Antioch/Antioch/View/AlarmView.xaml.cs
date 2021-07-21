using Antioch.View.Model;
using Antioch.View.ViewModels;
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
	public partial class AlarmView : ContentView
	{
		public AlarmView ()
		{
			InitializeComponent ();

			BindingContext = new AlarmViewModel();
		}

		void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
		{
			var mainpage = (MainPage)Application.Current.MainPage;

			var item = e.SelectedItem as Alarm;
			
			if(item != null)
				mainpage.LoadView(new PopupView(this, item.Content));
		}

		public void LoadList(NOTICE_RES res)
		{
			List<Alarm> prayList = new List<Alarm>();
			foreach (var pray in res.VarList)
			{
				var info2 = new Alarm();
				info2.Content = pray.VarContent;
				info2.Timestring = pray.VarDate;
				prayList.Add(info2);
			}

			listView.ItemsSource = prayList;

		}
	}
}