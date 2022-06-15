using Antioch.View;
using Antioch.View.Chat;
using Rg.Plugins.Popup.Services;
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
           //     var target = vm.Messages[vm.Messages.Count - 1];
           //     MessagesListView.ScrollTo(target, ScrollToPosition.End, true);
            };

            MessagesListView.ItemAppearing += (sender, e) =>
            {
                if (vm.Messages.Count > 0)
                {
                  
                        MessagesListView.ScrollTo(vm.Messages.Last(), ScrollToPosition.MakeVisible, false);
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

        public void ScrollEnd()
        {
            if (vm.Messages.Count > 0)
            {
                MessagesListView.ScrollTo(vm.Messages.Last(), ScrollToPosition.MakeVisible, false);
            }
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

        public HashSet<string> UserList = new HashSet<string>();


        private async void Face_Clicked(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PushAsync(new CameraPage());
        }

        private async void Cam_Clicked(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PushAsync(new QualityCam());
        }
        private async void List_Clicked(object sender, EventArgs e)
        {
            var page = new ChatUserList(UserList);

            await PopupNavigation.Instance.PushAsync(page);

            return;


            //var layout = new StackLayout
            //{
            //    WidthRequest = App.Current.MainPage.Width / 3,
            //    HeightRequest = App.Current.MainPage.Height,
            //    BackgroundColor = Color.White,
            //    HorizontalOptions = LayoutOptions.End,
            //    VerticalOptions = LayoutOptions.CenterAndExpand

            //};

            //{
            //    var DataTemplate = new DataTemplate(() =>
            //    {
            //        var grid = new Grid();
            //        var nameLabel = new Label { FontAttributes = FontAttributes.Bold ,TextColor = Color.Black};
            //        var ageLabel = new Label();
            //        var locationLabel = new Label { HorizontalTextAlignment = TextAlignment.End };

            //      // nameLabel.SetBinding(Label.TextProperty, "Name");
            //       // ageLabel.SetBinding(Label.TextProperty, "Age");
            //       // locationLabel.SetBinding(Label.TextProperty, "Location");

            //        grid.Children.Add(nameLabel);
            //       // grid.Children.Add(ageLabel, 1, 0);
            //      //  grid.Children.Add(locationLabel, 2, 0);

            //        return new ViewCell { View = grid };
            //    });

            //    var listview = new ListView();
            //    listview.ItemsSource = UserList;
            //  //  listview.ItemTemplate = DataTemplate;
            //    listview.Margin = new Thickness(20, 50, 20, 20);

            //    layout.Children.Add(listview);

          //  }


            ////close button
            //{
            //    var closebutton = new Button();
            //    closebutton.BackgroundColor = Color.White;
            //    closebutton.TextColor = Color.Black;
            //    closebutton.Text = "Close";
            //    closebutton.Clicked += async (s, args) => await App.Current.MainPage.Navigation.PopModalAsync();
            //    layout.Children.Add(closebutton);
            //}




            
            //await App.Current.MainPage.Navigation.PushModalAsync(
            //    new ContentPage
            //    {
            //        BackgroundColor = Color.Transparent,
            //        Content = layout,
            //    });
            

            //     var cameraPage = new CameraPage();
            //    Application.Current.MainPage.Navigation.PushModalAsync(cameraPage);

        }
    }
}
