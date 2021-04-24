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
    public partial class LobbyView : ContentView
    {


        public LobbyView()
        {
            InitializeComponent();
        }

        void OnTapped(object sender, EventArgs e)
        {

            var grid = this.Parent as Grid;

            BibleView bible = new BibleView();
            grid.Children.Clear();
            grid.Children.Add(bible);

        }

        public void LoadView(ContentView view)
        {
            var grid = this.Parent as Grid;
            grid.Children.Clear();
            grid.Children.Add(view);
        }
    }
}