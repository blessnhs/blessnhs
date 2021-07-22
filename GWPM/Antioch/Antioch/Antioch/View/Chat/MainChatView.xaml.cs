using Antioch.View;
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

    public partial class MainChatView : ContentView
    {
        MainChatViewModel vm;
        public MainChatView()
        {
            InitializeComponent();
            BindingContext = vm = new MainChatViewModel();


            vm.Messages.CollectionChanged += (sender, e) =>
            {
                var target = vm.Messages[vm.Messages.Count - 1];
                MessagesListView.ScrollTo(target, ScrollToPosition.End, true);
            };

            MessagesListView.ItemAppearing += (sender, e) =>
            {
                if (vm.Messages.Count > 0)
                {
                    var target = vm.Messages[vm.Messages.Count - 1];
                    if(target == (Antioch.Message)e.Item)
                        MessagesListView.ScrollTo(target, ScrollToPosition.MakeVisible, false);
                }
              
            };
        }

        async void OnSendClicked(object sender, EventArgs args)
        {
            NetProcess.SendRoomMessage(entry_message.Text);
        }


        public void ReceiveMessage(string text, string name,string time)
        {
            if (name != User.CacheData.UserName)
                vm.AddMessage(text, name,time, Message.type.Incoming);
            else
                vm.AddMessage(text, name,time, Message.type.Outgoing);
            
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
       //     var cameraPage = new CameraPage();
        //    Application.Current.MainPage.Navigation.PushModalAsync(cameraPage);
        
        }
    }
}
