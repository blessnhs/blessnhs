using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using MvvmHelpers;
using Plugin.Geolocator;
using Xamarin.Forms;
using System.Globalization;

namespace Antioch
{
    public class MainChatViewModel : BaseViewModel
    {

        public ObservableRangeCollection<Message> Messages { get; }

        public ObservableRangeCollection<ChatRoomUser> RoomUser { get; }
        

        string outgoingText = string.Empty;

        public string OutGoingText
        {
            get { return outgoingText; }
            set { SetProperty(ref outgoingText, value); }
        }

        public ICommand SendCommand { get; set; }

        public ICommand ExitCommand { get; set; }
        public ICommand LocationCommand { get; set; }

        public void AddMessage(string text,string name, Message.type type)
        {
            var message = new Message
            {
                Text = text,
                MessageType = type,
                AttachementUrl = "",
                MessageDateTime = DateTime.Now,
                ProfileUrl = name// "https://lh4.googleusercontent.com/-MEdrkpWi6Yg/AAAAAAAAAAI/AAAAAAAAAAA/AMZuuckIeT9M_SOv920jGkFwWiOCWkZRJA/s96-c/photo.jpg"
            };

            Messages.Add(message);

            OutGoingText = string.Empty;
        }

        public void AddChatUserMessage(string name)
        {
            ChatRoomUser usr = new ChatRoomUser();
            usr.User = name;

            RoomUser.Add(usr);
        }


        public MainChatViewModel()
        {
            // Initialize with default values
            Messages = new ObservableRangeCollection<Message>();
            RoomUser = new ObservableRangeCollection<ChatRoomUser>();


            SendCommand = new Command(() =>
            {
                
            });

            ExitCommand = new Command(() =>
            {
                NetProcess.SendLeaveRoom(0);
            });

            LocationCommand = new Command(async () =>
            {
                try
                {
                    var local = await CrossGeolocator.Current.GetPositionAsync();
                    var map = $"https://maps.googleapis.com/maps/api/staticmap?center={local.Latitude.ToString(CultureInfo.InvariantCulture)},{local.Longitude.ToString(CultureInfo.InvariantCulture)}&zoom=17&size=400x400&maptype=street&markers=color:red%7Clabel:%7C{local.Latitude.ToString(CultureInfo.InvariantCulture)},{local.Longitude.ToString(CultureInfo.InvariantCulture)}&key=";

                    var message = new Message
                    {
                        Text = "I am here",
                        AttachementUrl = map,
                        MessageType = Message.type.Outgoing,
                        MessageDateTime = DateTime.Now
                    };

                    Messages.Add(message);

                }
                catch (Exception ex)
                {

                }
            });
        }

    }
    
}
