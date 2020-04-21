using Google.Protobuf.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Rank : ContentPage
    {
        public Rank(RepeatedField<global::Rank> list)
        {
            InitializeComponent();


            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };
            main_grid.Children.Add(PrevBtn, 0, 0);

            var titletext = "전체 랭크";
            var titlelabelText = new Label { Text = titletext, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Center };
            main_grid.Children.Add(titlelabelText, 1, 0);
    

            int pos = 1;
            foreach(var r in list )
            {

                var img = new Image
                {
                    Source = new Uri(Helper.ToStr(r.VarPicUri.ToByteArray())),
                    BackgroundColor = Color.Black,
                };

                main_grid.Children.Add(img,0, pos);

                var stackLayout = new StackLayout();

                var text = r.VarRank + "위 " + Helper.ToStr(r.VarName.ToByteArray()) + " " + r.VarWin + "승" + r.VarLose + "패";
                var labelText = new Label { Text = text, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0) };

                stackLayout.Children.Add(labelText);

                var frame = new Frame { BorderColor = Color.Black, Padding = new Thickness(3, 3, 3, 3) };
                frame.Content = stackLayout;

                main_grid.Children.Add(frame, 1, pos++);
            }
        }
    }
}