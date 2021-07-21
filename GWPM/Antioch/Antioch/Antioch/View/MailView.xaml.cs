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
	public partial class MailView : ContentView
	{
		public MailView ()
		{
			InitializeComponent ();

			BindingContext = new MailViewModel();
		}

		void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
		{

		}

		public void LoadList(MAIL_LIST_RES res)
		{
			List<Mail> List = new List<Mail>();
			foreach (var mail in res.VarList)
			{
				var info2 = new Mail();
				info2.Name = mail.VarName;
				info2.Content = mail.VarContent;
				info2.Timestring = mail.VarDate;
				info2.Title = mail.VarTitle;
				List.Add(info2);
			}

			listView.ItemsSource = List;

		}
	}
}