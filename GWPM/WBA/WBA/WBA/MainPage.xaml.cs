using System;
using System.Threading;
using WBA.Network;

using NetClient;
using WBA.MainTabbedPage;
using Xamarin.Forms;
using System.Linq;

namespace WBA
{
    public partial class MainPage : TabbedPage
    {
        protected override void OnDisappearing()
        {
            base.OnDisappearing();
        }
         
        public MainPage()
        {
            var setting = new Setting();
            var community = new Community();

            Children.Add(new Notice());
            Children.Add(new Bible());
            Children.Add(new Planxaml());
            Children.Add(new PlanDetail());
            Children.Add(community);
            Children.Add(setting);
        }


    }
}
