using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Text;

namespace Antioch.View.Chat.ViewModels
{
    class SettingViewModel : INotifyPropertyChanged
    {
        private string _Message { get; set; }

        public string Message
        {
            get { return _Message; }
            set
            {
                _Message = value;
                OnPropertyChanged(nameof(Message));
            }
        }

        private string _Version { get; set; }

        public string Version
        {
            get { return _Version; }
            set
            {
                _Version = value;
                OnPropertyChanged(nameof(Version));
            }
        }

        private string _BtnMessage { get; set; }
        public string BtnMessage
        {
            get { return _BtnMessage; }
            set
            {
                _BtnMessage = value;
                OnPropertyChanged(nameof(BtnMessage));
            }
        }

        private string _Id { get; set; }

        public string Id
        {
            get { return _Id; }
            set
            {
                _Id = value;
                OnPropertyChanged(nameof(Id));
            }
        }


        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
