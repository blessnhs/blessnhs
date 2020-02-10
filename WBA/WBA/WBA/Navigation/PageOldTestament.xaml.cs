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
	public partial class PageOldTestament : TabbedPage
    {
		public PageOldTestament ()
		{
            Children.Add(new BibleList(BibleSplite.OldTestament));
            Children.Add(new Chapter(1));
            Children.Add(new Verse(1));
        }
	}
}