using System;
using MvvmHelpers;
using Humanizer;

namespace Antioch
{
    public class Message : ObservableObject
    {
        public enum type
        {
            Incoming,
            Outgoing,
            Info,
        };

        string text;

        public string Text
        {
            get { return text; }
            set { SetProperty(ref text, value); }
        }

        DateTime messageDateTime;

        public DateTime MessageDateTime
        {
            get { return messageDateTime; }
            set { SetProperty(ref messageDateTime, value); }
        }

        public string MessageTimeDisplay => MessageDateTime.Humanize();

        type messageType;

        public type MessageType
        {
            get { return messageType; }
            set { SetProperty(ref messageType, value); }
        }

        public bool HasAttachement => !string.IsNullOrEmpty(attachementUrl);

        string attachementUrl;

        public string AttachementUrl
        {
            get { return attachementUrl; }
            set { SetProperty(ref attachementUrl, value); }
        }

        string profileUrl;

        public string ProfileUrl
        {
            get { return profileUrl; }
            set { SetProperty(ref profileUrl, value); }
        }
    }


    public class ChatRoomUser : ObservableObject
    {
        string user;

        public string User
        {
            get { return user; }
            set { SetProperty(ref user, value); }
        }
    }
}

