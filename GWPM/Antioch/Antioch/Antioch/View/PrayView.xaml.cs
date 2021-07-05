using Antioch.View.Chat.Model;
using Antioch.View.Chat.ViewModels;
using MvvmHelpers;
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
    public partial class PrayView : ContentView
    {
        public PrayView()
        {
            InitializeComponent();

            Device.StartTimer(new TimeSpan(0, 0, 10), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    NetProcess.SendPrayList();
                });
                return true; // runs again, or false to stop
            });

            BindingContext = new PrayViewModel();
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
         
        }
        void Entry_TextChanged(object sender, TextChangedEventArgs e)
        {
           
        }

        public void LoadPrayList(PRAY_MESSAGE_RES res)
        {
            List<PrayModelInfo> prayList = new List<PrayModelInfo>();
            foreach (var pray in res.VarList)
            {
                var info2 = new PrayModelInfo();
                info2.Name = pray.VarName;
                info2.Content = pray.VarMessage;
                prayList.Add(info2);
            }

            listView.ItemsSource = prayList;

        }

        private void Entry_TextChanged(object sender, EventArgs e)
        {
            NetProcess.SendMakePray(PrayEntry.Text);

            PrayEntry.Text = "";

            NetProcess.SendPrayList();

        }
    }
}