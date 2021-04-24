using Antioch.TestamentPage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class PageNewTestament : ContentView
    {
		public PageNewTestament ()
		{
            InitializeComponent();

            main_grid.Children.Add(new BibleList(BibleSplite.NewTestament), 0, 0);
            main_grid.Children.Add(new Chapter(1), 1, 0);
            main_grid.Children.Add(new Verse(1), 2, 0);
           
        }
	}
}