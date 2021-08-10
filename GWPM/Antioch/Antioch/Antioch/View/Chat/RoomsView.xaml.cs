using Antioch.View.Chat;
using MvvmHelpers;
using System;
using System.Collections.Generic;

using Xamarin.Forms;

namespace Antioch
{
    public partial class RoomsPageView : ContentView
    {
        public RoomsPageView()
        {
            InitializeComponent();
            BindingContext = new RoomsViewModel();
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
                info.Name = room.VarName;
                info.Count = room.VarCurrentCount;

                roomList.Add(info);
            }

            listView.ItemsSource = roomList;

        }

        async void Clicked_Create(object sender, System.EventArgs e)
        {
            //string action = await App.Current.MainPage.DisplayActionSheet("Room Create #General LEGION KOR ", "Cancel", null, "Individual", "Local", "Global");

            string action = await App.Current.MainPage.DisplayPromptAsync("room create", "enter room name");

            if (action == null)
                return;

            if (action.Length > 1024)
            {
                await App.Current.MainPage.DisplayAlert("name checker", "size is long", "ok");
                return;
            }

            NetProcess.SendMakeRoom(User.CacheData.UserName + "_" + action);

            //switch (action)
            //{
            //    case "Individual":
            //    case "Local":
            //    case "Global":
            //        NetProcess.SendMakeRoom(User.CacheData.UserName + "_#" + action);
            //        break;
            //    default:
            //        {
            //            if (action != "Cancel")
            //                NetProcess.SendMakeRoom(User.CacheData.UserName + "_" + "general");
            //        }
            //        break;
            //}
        }

        async void Clicked_Exit(object sender, System.EventArgs e)
        {
            var mainpage = (MainPage)Application.Current.MainPage;

            mainpage.LoadLobby();
        }

    }

  
}

