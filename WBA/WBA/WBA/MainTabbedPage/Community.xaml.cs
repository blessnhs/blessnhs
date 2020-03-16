using NetClient;
using System.Threading.Tasks;
using WBA.Navigation;
using WBA.Network;
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

            MessagingCenter.Subscribe<Community, CompletePacket>(this, "community", (s, e) =>
            {
               
            });
        }

        public void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as CommunityRoomInfo;
        }

        async void OnRoomMakeClicked(object sender, System.EventArgs e)
        {
            string inputstring = await InputBox(this.Navigation);

            if (inputstring != null)
                NetProcess.SendMakeRoom(inputstring);
        }

        public static Task<string> InputBox(INavigation navigation)
        {
            // wait in this proc, until user did his input 
            var tcs = new TaskCompletionSource<string>();

            var lblTitle = new Label { Text = "Title", HorizontalOptions = LayoutOptions.Center, FontAttributes = FontAttributes.Bold };
            var lblMessage = new Label { Text = "Enter new text:" };
            var txtInput = new Entry { Text = "" };

            var btnOk = new Button
            {
                Text = "Ok",
                WidthRequest = 100,
                BackgroundColor = Color.FromRgb(0.8, 0.8, 0.8),
            };
            btnOk.Clicked += async (s, e) =>
            {
                // close page
                var result = txtInput.Text;
                await navigation.PopModalAsync();
                // pass result
                tcs.SetResult(result);
            };

            var btnCancel = new Button
            {
                Text = "Cancel",
                WidthRequest = 100,
                BackgroundColor = Color.FromRgb(0.8, 0.8, 0.8)
            };
            btnCancel.Clicked += async (s, e) =>
            {
                // close page
                await navigation.PopModalAsync();
                // pass empty result
                tcs.SetResult(null);
            };

            var slButtons = new StackLayout
            {
                Orientation = StackOrientation.Horizontal,
                Children = { btnOk, btnCancel },
            };

            var layout = new StackLayout
            {
                Padding = new Thickness(0, 40, 0, 0),
                VerticalOptions = LayoutOptions.StartAndExpand,
                HorizontalOptions = LayoutOptions.CenterAndExpand,
                Orientation = StackOrientation.Vertical,
                Children = { lblTitle, lblMessage, txtInput, slButtons },
            };

            // create and show page
            var page = new ContentPage();
            page.Content = layout;
            navigation.PushModalAsync(page);
            // open keyboard
            txtInput.Focus();

            // code is waiting her, until result is passed with tcs.SetResult() in btn-Clicked
            // then proc returns the result
            return tcs.Task;
        }

    }



    public class CommunityRoomInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int CurrentCount { get; set; }
    };
}