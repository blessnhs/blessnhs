using System;
using System.Windows.Input;
using MvvmHelpers;
using Xamarin.Forms;

namespace Antioch
{
    public class RoomsViewModel : BaseViewModel
    {
        public ObservableRangeCollection<RoomInfo> Rooms { get; }
        public Command ConnectCommand { get; }

        public RoomsViewModel()
        {
         
        }



    }
}

