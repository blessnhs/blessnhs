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
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as HymnTableInfo;

            NavigationPage.SetHasNavigationBar(this, true);
            //      NavigationPage.SetHasBackButton(this, true);

          //  Navigation.PushModalAsync(new HymnViewer(contact.Id));
        }
        void Entry_TextChanged(object sender, TextChangedEventArgs e)
        {
           
        }
    }
}