using WBA.TestamentPage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class PageNewTestament : TabbedPage
    {
		public PageNewTestament ()
		{
            Children.Add(new BibleList(BibleSplite.NewTestament));
            Children.Add(new Chapter(1));
            Children.Add(new Verse(1));
        }
	}
}