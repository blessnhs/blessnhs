using Antioch.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.TestamentPage
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class Chapter : ContentView
    {
        void btn_Clicked(object sender, EventArgs e)
        {
            Button evtButton = sender as Button;
            
            User.CacheData.Chapter = Convert.ToInt32(evtButton.Text);

            var grid = this.Parent as Grid;

            int MaxVerse = BibleInfo.GetVerseSize(User.CacheData.BibleName, User.CacheData.Chapter);

            grid.Children.Add(new Verse(MaxVerse), 2, 0);

            //parentPage.Children[2] = new Verse(MaxVerse);

            //parentPage.CurrentPage = parentPage.Children[2];
        }

        public Chapter (int count)
		{
            InitializeComponent();

             BackgroundColor = Color.FromHex("#404040");

            StackLayout stackLayout = new StackLayout();

            var plainButton = new Style(typeof(Button))
            {
                Setters = {
                      new Setter { Property = Button.BackgroundColorProperty, Value = Color.FromHex ("#eee") },
                      new Setter { Property = Button.TextColorProperty, Value = Color.Black },
                      new Setter { Property = Button.BorderRadiusProperty, Value = 0 },
                      new Setter { Property = Button.FontSizeProperty, Value = 15 }
                }
            };
                     
            var controlGrid = new Grid { RowSpacing = 1, ColumnSpacing = 1 };

            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(2, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(2, GridUnitType.Star) });
            controlGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(2, GridUnitType.Star) });
            
            if (count == 0) count = 1;
            int RowCount = (count / 3);
            for (int i = 0; i < RowCount; i++)
            {
                controlGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
            }

            //var label = new Label
            //{
            //    Text = "0",
            //    HorizontalTextAlignment = TextAlignment.End,
            //    VerticalTextAlignment = TextAlignment.End,
            //    TextColor = Color.White,
            //    FontSize = 15
            //};
            //controlGrid.Children.Add(label, 0, 0);


            //Grid.SetColumnSpan(label, 3);

            int x = 0;
            int y = 0;

            for (int i = 0; i < count; i++)
            {
                x = i % 3;
                y = i / 3;

                Button button = new Button { Text = (i + 1).ToString(), Style = plainButton, FontSize=10 };

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