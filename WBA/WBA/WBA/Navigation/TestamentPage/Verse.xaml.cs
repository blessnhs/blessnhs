using WBA.MainTabbedPage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.TestamentPage
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class Verse : ContentPage
	{
        void btn_Clicked(object sender, EventArgs e)
        {
            Button evtButton = sender as Button;

            User.CacheData.Verse = Convert.ToInt32(evtButton.Text);

            Navigation.PopModalAsync();

        }

        public Verse (int count)
		{
			InitializeComponent ();

            Title = "장";
            BackgroundColor = Color.FromHex("#404040");

            StackLayout stackLayout = new StackLayout();

            var plainButton = new Style(typeof(Button))
            {
                Setters = {
                      new Setter { Property = Button.BackgroundColorProperty, Value = Color.FromHex ("#eee") },
                      new Setter { Property = Button.TextColorProperty, Value = Color.Black },
                      new Setter { Property = Button.BorderRadiusProperty, Value = 0 },
                      new Setter { Property = Button.FontSizeProperty, Value = 30 }
                }
            };

            var controlGrid = new Grid { RowSpacing = 1, ColumnSpacing = 1 };

            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });

            if (count == 0) count = 1;
            int RowCount = (count / 4);
            for (int i = 0; i < RowCount; i++)
            {
                controlGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
            }

            var label = new Label
            {
                Text = "0",
                HorizontalTextAlignment = TextAlignment.End,
                VerticalTextAlignment = TextAlignment.End,
                TextColor = Color.White,
                FontSize = 30
            };
            controlGrid.Children.Add(label, 0, 0);


            Grid.SetColumnSpan(label, 4);

            int x = 0;
            int y = 0;

            for (int i = 0; i < count; i++)
            {
                x = i % 4;
                y = i / 4;

                Button button = new Button { Text = (i + 1).ToString(), Style = plainButton };

                button.Clicked += btn_Clicked;

                controlGrid.Children.Add(button, x, y);
            }

            stackLayout.Children.Add(controlGrid);

            ScrollView scrollView = new ScrollView();
            scrollView.Content = stackLayout;
            Content = scrollView;
        }
	}
}