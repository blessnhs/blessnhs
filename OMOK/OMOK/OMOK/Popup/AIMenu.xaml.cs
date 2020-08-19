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
    public partial class AISelect : PopupPage
    {
     
        public int game_rule = 1;
        public int mode = 1;

        public AISelect()
        {
            InitializeComponent();
        }

        private void Gomoku_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            game_rule = 1;
        }

        private void Normal_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            game_rule = 2;
        }

        private void Renju_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            game_rule = 3;
        }

        private void mode1_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            mode = 1;
        }

        private void mode2_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            mode = 2;
        }

        private async void CloseButtonClicked(object sender, EventArgs e)
        {
            await Navigation.PopAllPopupAsync();

            User.myInfo.ai_mode = mode;
            User.myInfo.ai_rule = game_rule;
            User.myInfo.ai_set_flag = true;

        }

    }
}