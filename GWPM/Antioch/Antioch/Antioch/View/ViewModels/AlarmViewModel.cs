using Antioch.View.Model;
using MvvmHelpers;
using MvvmHelpers.Commands;
using System;
using System.Collections.Generic;
using System.Text;

namespace Antioch.View.ViewModels
{
    class AlarmViewModel
    {
        public ObservableRangeCollection<Alarm> Alarms { get; }
        public Command ConnectCommand { get; }

        public AlarmViewModel()
        {

        }
    }
}
