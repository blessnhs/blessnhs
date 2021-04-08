using NetClient;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using WBA.Network;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Community : ContentPage
    {
        CommunityViewModel viewModel = new CommunityViewModel();

        protected override void OnAppearing()
        {
            base.OnAppearing();
        }

        public void UpdateMessage(CompletePacket packet)
        {
             viewModel.RoomModel.Clear();

            ROOM_LIST_RES res = new ROOM_LIST_RES();
            res = ROOM_LIST_RES.Parser.ParseFrom(packet.Data);

            foreach (var room in res.VarRoomList)
            {
                CommunityRoomInfoModel croom = new CommunityRoomInfoModel();
                croom.Id = room.VarId;
                croom.Name = Helper.ToStr(room.VarName.ToByteArray());
                croom.CurrentCount = room.VarCurrentCount;

                viewModel.RoomModel.Add(croom);
            }

            listView.ItemsSource = viewModel.RoomModel;
        }

        protected override void OnDisappearing()
        {
        }

        public Community()
        {
            InitializeComponent();

            Device.StartTimer(new TimeSpan(0, 0, 3), () =>
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    NetProcess.SendReqRoomList();
                });
                return true; //if true repeat
            });

            
        }

        public void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as CommunityRoomInfoModel;

            if(contact != null)
                NetProcess.SendEnterRoom(contact.Id);
        }

        async void OnRoomMakeClicked(object sender, System.EventArgs e)
        {
            string inputstring = await InputBox(this.Navigation);

        //    if (inputstring != null)
        //        NetProcess.SendMakeRoom(inputstring);
        }

        public static Task<string> InputBox(INavigation navigation)
        {
            // wait in this proc, until user did his input 
            var tcs = new TaskCompletionSource<string>();

            var lblTitle   = new Label { Text = "포럼 생성", HorizontalOptions = LayoutOptions.Center, FontAttributes = FontAttributes.Bold };
            var lblMessage = new Label { Text = "방 이름을 입력하세요:" };
            var txtInput   = new Entry { Text = "포럼1" };

            var btnOk = new Button
            {
                Text = "확인",
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
                Text = "취소",
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

}