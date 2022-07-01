using OMOK.Views;
using Rg.Plugins.Popup.Extensions;
using Rg.Plugins.Popup.Pages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Confirm : PopupPage
    {
        object _parent;
        public Confirm(object parent)
        {
            InitializeComponent();

            _parent = parent;


            if (User.Locale != "ko")
            {
                Label1.Text = "Are you sure ?";
                OKButton.Text = "OK";
                CancelButton.Text = "Cancel";
            }

        }

        void OnYesClicked(object sender, System.EventArgs e)
        {
            if(_parent.ToString() == "OMOK.Views.SingleMatch")
                MessagingCenter.Send<SingleMatch>((SingleMatch)_parent, "close");
            else if (_parent.ToString() == "OMOK.Room")
                MessagingCenter.Send<Room>((Room)_parent, "close");


            Navigation.PopAllPopupAsync();
        }

        void OnNoClicked(object sender, System.EventArgs e)
        {
             Navigation.PopAllPopupAsync();
        }
    }
}