﻿
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK.Network;

namespace OMOK.Views
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

                Device.StartTimer(new TimeSpan(0, 0, 0, 0, 300), () =>
                {
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        NetProcess.start();
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