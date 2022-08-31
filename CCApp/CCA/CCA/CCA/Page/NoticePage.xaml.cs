using Rg.Plugins.Popup.Pages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using MvvmHelpers;
using MvvmHelpers.Commands;
using Rg.Plugins.Popup.Services;

namespace CCA.Page
{
    class Alarm : ObservableObject
    {
        string name;

        public string Name
        {
            get { return name; }
            set { SetProperty(ref name, value); }
        }

        string timestring;
        public string Timestring
        {
            get { return timestring; }
            set { SetProperty(ref timestring, value); }
        }

        string content;

        public string Content
        {
            get { return content; }
            set { SetProperty(ref content, value); }
        }

        string title;

        public string Title
        {
            get { return title; }
            set { SetProperty(ref title, value); }
        }
    }
    class AlarmViewModel
    {
        public ObservableRangeCollection<Alarm> Alarms { get; }

        public AlarmViewModel()
        {
           
        }
    }

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class NoticePage : PopupPage
    {
        public NoticePage()
        {
            InitializeComponent();

            BindingContext = new AlarmViewModel();
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            var mainpage = (MainPage)Application.Current.MainPage;

            var item = e.SelectedItem as Alarm;

            if (item != null)
                PopupNavigation.Instance.PushAsync(new NotifyPopup(this, item.Content));
        }

        public void LoadList(NOTICE_RES res)
        {
            List<Alarm> prayList = new List<Alarm>();
            foreach (var pray in res.VarList)
            {
                var info2 = new Alarm();
                info2.Content = pray.VarContent;
                info2.Timestring = pray.VarDate;
                prayList.Add(info2);
            }

            listView.ItemsSource = prayList;

        }

        private async void OnCloseButtonClicked(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }
    }
}