using WBA.MainTabbedPage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace WBA
{
    public partial class MainPage : TabbedPage
    {
        public MainPage()
        {
            Children.Add(new Notice());
            Children.Add(new Bible());
            Children.Add(new Planxaml());
            Children.Add(new Setting());
            //Children.Add(new Admin());
        }
    }
}
