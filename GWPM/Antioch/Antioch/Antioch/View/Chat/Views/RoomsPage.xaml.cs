using System;
using System.Collections.Generic;

using Xamarin.Forms;

namespace Antioch
{

    public class RoomInfo
    {
        public RoomInfo(string name)
        {
            Name = name;
        }
        public int Id { get; set; }
        public string Name { get; set; }

        public int Count { get; set; }
    };

    public partial class RoomsPage : ContentPage
    {
        public RoomsPage()
        {
            InitializeComponent();
            BindingContext = new RoomsViewModel();

            NetProcess.SendRoomList();
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as RoomInfo;
        }

        public void LoadRoomList()
        {

            List<RoomInfo> LoadInfo = new List<RoomInfo>();

            LoadInfo.Add(new RoomInfo("123"));
            LoadInfo.Add(new RoomInfo("124"));
            LoadInfo.Add(new RoomInfo("125"));
            LoadInfo.Add(new RoomInfo("126"));

            listView.ItemsSource = LoadInfo;
        }
    }

  
}

