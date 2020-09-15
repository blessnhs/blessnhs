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


            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start,HeightRequest = 45 };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };
            main_grid.Children.Add(PrevBtn, 0, 0);

            var titletext = "RANK";
            var titlelabelText = new Label { Text = titletext, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Center, FontSize = 25 };
            main_grid.Children.Add(titlelabelText, 2, 0);
    

            int pos = 1;
            foreach(var r in list )
            {
                var imgcontry = new Image
                {
                    Source = ImageSource.FromResource(Helper.GetLocaleImagePath(Helper.ToStr(r.VarContry.ToByteArray()))),
                    BackgroundColor = Color.White,
                    HeightRequest = 30
                };
                main_grid.Children.Add(imgcontry, 0, pos);
           
               //프로필
                var img = new Image
                {
                    Source = new Uri(Helper.ToStr(r.VarPicUri.ToByteArray())),
                    BackgroundColor = Color.White,
                    HeightRequest = 30
                };

                main_grid.Children.Add(img, 1, pos);
                /////////////////////////////////////////////////////
                /////기록
                var stackLayout = new StackLayout() {  };

                var text = r.VarRank + "위 " + Helper.ToStr(r.VarName.ToByteArray()) + " " + r.VarWin + "승" + r.VarLose + "패";

                if (User.Locale != "ko")
                    text = r.VarRank + "Rank " + Helper.ToStr(r.VarName.ToByteArray()) + " " + r.VarWin + "Win" + r.VarLose + "Defeat";

                text.Replace('\n', ' ');
                var labelText = new Label { Padding = new Thickness(5, 10, 0, 0), Text = text, TextColor = Xamarin.Forms.Color.FromRgb(0, 0, 0), HorizontalTextAlignment = TextAlignment.Start };

                stackLayout.Children.Add(labelText);

                var frame = new Frame { BorderColor = Color.Black, Padding = new Thickness(0, 0, 0, 0) ,HeightRequest = 30};
                frame.Content = stackLayout;

                main_grid.Children.Add(frame, 2, pos++);
                /////////////////////////////////////////////////////

            }
        }
    }
}