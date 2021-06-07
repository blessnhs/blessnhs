using MvvmHelpers;
using System;
using System.Collections.Generic;

using Xamarin.Forms;

namespace Antioch
{
    public partial class RoomsPage : ContentPage
    {
        public RoomsPage()
        {
            InitializeComponent();
            BindingContext = new RoomsViewModel();

            Device.StartTimer(new TimeSpan(0, 0, 2), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    NetProcess.SendRoomList();
                });
                return true; // runs again, or false to stop
            });

          
        }

        async void OnRoomEnterClicked(object sender, EventArgs args)
        {
            Button button = sender as Button;

            int id = Convert.ToInt32(button.CommandParameter.ToString());

            NetProcess.SendEnterRoom(id);
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as RoomInfo;
        }

        public void LoadRoomList(ROOM_LIST_RES res)
        {
            List<RoomInfo> roomList = new List<RoomInfo>();
            foreach(var room in res.VarRoomList)
            {
                var info = new RoomInfo();
                info.Id = room.VarId;
                info.Name = Helper.ToStr(room.VarName.ToByteArray());
                info.Count = room.VarCurrentCount;

                roomList.Add(info);
            }

            listView.ItemsSource = roomList;

        }

        async void Clicked_Create(object sender, System.EventArgs e)
        {
            string action = await DisplayActionSheet("Create Room", "Cancel", null, "개인", "지역", "전체");

            switch (action)
            {
                case "전체":
                case "지역":
                case "개인":
                    NetProcess.SendMakeRoom(User.CacheData.UserName + "+" + action);
                    break;
                default:
                    NetProcess.SendMakeRoom(User.CacheData.UserName + "+" + "일반");
                    break;
            }
        }

    }

  
}

