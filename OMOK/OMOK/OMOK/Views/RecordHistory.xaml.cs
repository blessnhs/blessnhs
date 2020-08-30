using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class RecordHistory : ContentPage
    {
        public RecordHistory()
        {
            InitializeComponent();

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };
            main_grid.Children.Add(PrevBtn, 0, 0);

            var list = SQLite.ReadResultLog();

            int pos = 1;
            foreach (var r in list)
            {
                var stackLayout = new StackLayout();

                var text = pos.ToString() + ". " + r.Time.ToString("yyyy-MM-dd HH:mm") + " " + r.MyName + " vs " + r.OpponentName + " " + (r.Result == 1 ? "승" : "패");

                text.Replace('\n', ' ');
                var labelText = new Label { Padding = new Thickness(0,10,0,0),   Text = text, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Start };

                stackLayout.Children.Add(labelText);

                var frame = new Frame { BorderColor = Color.Black, Padding = new Thickness(0,0,0,0) };
                frame.Content = stackLayout;

                main_grid.Children.Add(frame, 0, pos++);

                if (pos > 20)
                    break;
            }
        }
    }
}