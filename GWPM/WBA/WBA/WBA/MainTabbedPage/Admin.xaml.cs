using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Admin : ContentPage
    {
        public Admin()
        {
            InitializeComponent();
        }

        async void OnNoticeButtonClicked(object sender, EventArgs e)
        {
            await DisplayAlert("", "관리자만 등록가능합니다..", "OK");
        }

        async void OnWorshipButtonClicked(object sender, EventArgs e)
        {
            await DisplayAlert("", "관리자만 등록가능합니다..", "OK");
        }
    }
}