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

            BindingContext = new PrayViewModel();
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            var mainpage = (MainPage)Application.Current.MainPage;

            var item = e.SelectedItem as PrayModelInfo;

            if (item != null)
                mainpage.LoadView(new PopupView(this, item.Content));
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
                info2.Timestring = pray.VarTime;
                prayList.Add(info2);
            }

            listView.ItemsSource = prayList;

        }

        private void Entry_TextChanged(object sender, EventArgs e)
        {

            if (PrayEntry.Text == null || PrayEntry.Text == "")
                return;

            NetProcess.SendMakePray(PrayEntry.Text);

            PrayEntry.Text = "";

            NetProcess.SendPrayList();

        }
    }
}