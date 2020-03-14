using WBA.MainTabbedPage;
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
            Children.Add(new PlanDetail());
            Children.Add(new Community());
            Children.Add(new Setting());

        }
    }
}
