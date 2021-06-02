using System;
using System.Collections.Generic;

using Xamarin.Forms;

namespace Antioch
{
    public partial class RoomsPage : ContentPage
    {
        public RoomsPage()
        {
            InitializeComponent();
            BindingContext = new RoomsViewModel();
        }
    }
}

