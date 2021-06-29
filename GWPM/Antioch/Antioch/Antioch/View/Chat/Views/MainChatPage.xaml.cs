using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;

namespace Antioch
{
    public static class ViewModelLocator
    {
        static MainChatViewModel chatVM;
        public static MainChatViewModel MainChatViewModel
        {
            get
            {
                if (chatVM == null)
                {
                    chatVM = new MainChatViewModel();
                }
                return chatVM;

            }
        }

    }

    public partial class MainChatPage : ContentPage
    {
        MainChatViewModel vm;
        public MainChatPage()
        {
            InitializeComponent();
            Title = "#general";
            BindingContext = vm = new MainChatViewModel();


            vm.Messages.CollectionChanged += (sender, e) =>
            {
                var target = vm.Messages[vm.Messages.Count - 1];
                MessagesListView.ScrollTo(target, ScrollToPosition.End, true);
            };

            entry_message.Completed += (sender, e) =>
            {
                NetProcess.SendRoomMessage(entry_message.Text);
            };
        }

        public void ReceiveMessage(string text, string name)
        {
            if (name != User.CacheData.UserName)
                vm.AddMessage(text, name, Message.type.Incoming);
            else
                vm.AddMessage(text, name, Message.type.Outgoing);
        }

        public void ReceiveMessage(string text, string name, Message.type type)
        {
            vm.AddMessage(text, name, type);

            vm.AddChatUserMessage(name);
        }

        void MyListView_OnItemSelected(object sender, SelectedItemChangedEventArgs e)
        {
            MessagesListView.SelectedItem = null;
        }

        void MyListView_OnItemTapped(object sender, ItemTappedEventArgs e)
        {
            MessagesListView.SelectedItem = null;

        }

        private async void List_Clicked(object sender, EventArgs e)
        {
            if (UserList.IsVisible == true)
                UserList.IsVisible = false;
            else
                UserList.IsVisible = true;
        }
    }
}
