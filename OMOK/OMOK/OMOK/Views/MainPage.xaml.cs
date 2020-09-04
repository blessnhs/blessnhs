
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK.Network;
using System.Runtime.InteropServices; // DllImport 애트리뷰트

namespace OMOK.Views
{
    public partial class MainPage : TabbedPage
    {
        public MainPage()
        {
            InitializeComponent();


            Children.Add(new Lobby());
            var sizxe = Children[0].Height;

            Children[0].HeightRequest = 0;
            var bs = Children[0].Bounds;



            //소켓 연결
            {

                Device.StartTimer(new TimeSpan(0, 0, 0, 0, 300), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.start();
                        NetProcess.Loop(this);
                    });
                    return true; //if true repeat
                });

                Device.StartTimer(new TimeSpan(0, 0, 15), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.SendReqRoomList();
                    });
                    return true; //if true repeat
                });

            }

        }
    }
}