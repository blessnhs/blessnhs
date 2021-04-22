using Antioch.View;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace Antioch
{
    public partial class MainPage : ContentPage
    {
        Lobby lobby = new Lobby();

        public MainPage()
        {
            InitializeComponent();


            symbol_image.Source = ImageSource.FromResource("Antioch.Image.symbol.png");
           
            ContentViews.Children.Add(lobby);
        }
    }
}
