using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Community : ContentPage
    {
        public Community()
        {
            InitializeComponent();
        }

        public void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as CommunityRoomInfo;
        }
    }

    public class CommunityRoomInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int CurrentCount { get; set; }
    };
}