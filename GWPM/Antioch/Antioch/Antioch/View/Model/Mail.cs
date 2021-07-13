using MvvmHelpers;
using System;
using System.Collections.Generic;
using System.Text;

namespace Antioch.View.Model
{
    public class Mail : ObservableObject
    {
        string name;

        public string Name
        {
            get { return name; }
            set { SetProperty(ref name, value); }
        }

        string timestring;
        public string Timestring
        {
            get { return timestring; }
            set { SetProperty(ref timestring, value); }
        }

        string content;

        public string Content
        {
            get { return content; }
            set { SetProperty(ref content, value); }
        }

        string title;

        public string Title
        {
            get { return title; }
            set { SetProperty(ref title, value); }
        }
    }
}
