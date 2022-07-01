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


            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start,HeightRequest = 45 ,WidthRequest=80};
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };
            main_grid.Children.Add(PrevBtn, 0, 0);

            int pos = 1;
            foreach(var r in list )
            {
                //순위
                var labelText = new Label { FontSize = 30,Text = r.VarRank.ToString(), TextColor = Color.Black, HorizontalTextAlignment = TextAlignment.Center,VerticalTextAlignment = TextAlignment.Center };
                main_grid.Children.Add(labelText, 0, pos);

                //국기
                var imgcontry = new Image
                {
                    Source = ImageSource.FromResource(Helper.GetLocaleImagePath(Helper.ToStr(r.VarContry.ToByteArray()))),
                    BackgroundColor = Color.White,
                    HeightRequest = 30
                };
                main_grid.Children.Add(imgcontry, 1, pos);
           
               //프로필

                if(Helper.ToStr(r.VarPicUri.ToByteArray()) != "")
                {

                    var img = new Image
                    {
                        Source = new Uri(Helper.ToStr(r.VarPicUri.ToByteArray())),
                        BackgroundColor = Color.White,
                        HeightRequest = 30
                    };
                    main_grid.Children.Add(img, 2, pos);


                }
                /////////////////////////////////////////////////////

                //이름
                var labelName = new Label { Text = Helper.ToStr(r.VarName.ToByteArray()), TextColor = Color.Black, HorizontalTextAlignment = TextAlignment.Center, VerticalTextAlignment = TextAlignment.Center };
                main_grid.Children.Add(labelName, 3, pos);

                /////////////////////////////////////////////////////
                /////기록
                var stackLayout = new StackLayout() {  };

                var content = r.VarWin + "승" + r.VarLose + "패";

                if (User.Locale != "ko")
                    content = r.VarWin + " Win " + r.VarLose +  " Defeat ";

                var contentText = new Label { FontSize = 15, Padding = new Thickness(5, 10, 0, 0), Text = content, TextColor = Color.Black, HorizontalTextAlignment = TextAlignment.Start, VerticalTextAlignment = TextAlignment.Center };

                stackLayout.Children.Add(contentText);

                var frame = new Frame { BorderColor = Color.Black, Padding = new Thickness(0, 0, 0, 0) ,HeightRequest = 30};
                frame.Content = stackLayout;

                main_grid.Children.Add(frame, 4, pos++);
                /////////////////////////////////////////////////////

            }
        }
    }
}