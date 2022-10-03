using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Rg.Plugins.Popup.Pages;

namespace CCA.Page
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class CameraViewer : PopupPage
    {

        public string MachinId;
        public Int64 PlayerId;
        public CameraViewer(string _MachinId,Int64 playerId)
        {
            InitializeComponent();

            MachinId = _MachinId;
            PlayerId = playerId;
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();
        }

        protected override void OnDisappearing()
        {
            base.OnDisappearing();

            NetProcess.SendStopStream();
        }

    }
}