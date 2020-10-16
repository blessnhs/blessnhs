using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace SJS_BADUK
{
    public partial class PVPRoom : ContentPage
    {
        BoardRenderer _renderer = new BoardRenderer();
        BadukControl _control = new BadukControl();

        public PVPRoom(double _size)
        {
            InitializeComponent();

            _control.Init(_renderer);

            Device.StartTimer(new TimeSpan(0, 0, 1), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    _renderer.Init(_size, Width,ref absoluteLayout, _control);
                });
                return false; // runs again, or false to stop
            });

        }

        protected override void OnSizeAllocated(double width, double height)
        {
            base.OnSizeAllocated(width, height);
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();
        }
    }
}