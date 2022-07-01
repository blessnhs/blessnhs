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
    public partial class AISelect : PopupPage
    {
        SingleMatch _parent;

        public AISelect(SingleMatch parent)
        {
            InitializeComponent();

            if (User.Locale != "ko")
            {
                ai1.Text = "Black AI vs White User";
                ai2.Text = "Black User vs White AI";
             }

            _parent = parent;

            //switch(User.myInfo.ai_rule)
            //{
            //    case 1:
            //        gomoku.IsChecked = true;
            //        break;
            //    case 2:
            //        normal.IsChecked = true;
            //        break;
            //    case 3:
            //        renju.IsChecked = true;
            //        break;
            //}

            //렌주룰로 고정
            User.myInfo.ai_rule = 3;

            switch (User.myInfo.ai_mode)
            {
                case 1:
                    ai1.IsChecked = true;
                    break;
                case 2:
                    ai2.IsChecked = true;
                    break;
              
            }
        }

        private void Gomoku_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            User.myInfo.ai_rule = 1;
        }

        private void Normal_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            User.myInfo.ai_rule = 2;
        }

        private void Renju_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            User.myInfo.ai_rule = 3;
        }

        private void mode1_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            User.myInfo.ai_mode = 1;
        }

        private void mode2_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            User.myInfo.ai_mode = 2;
        }

        private async void CloseButtonClicked(object sender, EventArgs e)
        {
           User.myInfo.ai_reset_flag = true;

            await Navigation.PopAllPopupAsync();

            MessagingCenter.Send<SingleMatch, string>(_parent, "Start", "Start");


        }

    }
}