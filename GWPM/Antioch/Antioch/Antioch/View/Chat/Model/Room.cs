using MvvmHelpers;
using System;
using System.Collections.Generic;
using System.Text;

namespace Antioch
{
    public class RoomInfo : ObservableObject
    {
        string name;

        public string Name
        {
            get { return name; }
            set { SetProperty(ref name, value); }
        }

        int id;

        public int Id
        {
            get { return id; }
            set { SetProperty(ref id, value); }
        }

        int count;

        public int Count
        {
            get { return count; }
            set { SetProperty(ref count, value); }
        }
    }
}
