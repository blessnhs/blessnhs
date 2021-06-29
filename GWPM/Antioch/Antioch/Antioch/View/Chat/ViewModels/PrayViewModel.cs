using Antioch.View.Chat.Model;
using MvvmHelpers;
using MvvmHelpers.Commands;
using System;
using System.Collections.Generic;
using System.Text;

namespace Antioch.View.Chat.ViewModels
{
    class PrayViewModel
    {
        public ObservableRangeCollection<PrayModelInfo> Prays { get; }
        public Command ConnectCommand { get; }

        public PrayViewModel()
        {

        }



    }
}
