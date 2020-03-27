
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK_T.Network;

namespace OMOK_T.Views
{
    public partial class MainPage : TabbedPage
    {
        public MainPage()
        {
            InitializeComponent();

            Children.Add(new Lobby());
            Children.Add(new Room());

            //소켓 연결
            {
                NetProcess.start();

                Device.StartTimer(new TimeSpan(0, 0, 0, 0, 300), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.Loop(this);
                    });
                    return true; //if true repeat
                });

                Device.StartTimer(new TimeSpan(0, 0, 5), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.SendReqRoomList();
                    });
                    return true; //if true repeat
                });

            }
            //Navigation.PushModalAsync(new NavigationPage(new Room()));

        }
    }
}