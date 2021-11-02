using Rg.Plugins.Popup.Pages;
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
	public partial class CameraPage : PopupPage
	{
		public CameraPage ()
		{
			InitializeComponent ();
		}

		public void DrawLayout()
        {
            var controlGrid = new Grid { RowSpacing = 1, ColumnSpacing = 1 ,BackgroundColor=Color.Red};

            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(2, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(2, GridUnitType.Star) });

            for (int i = 0; i < 3; i++)
            {
                controlGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
            }

            Content = new Label()
            {
                Text = "hllo"
            }  ;
        }



	}
}