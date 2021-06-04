using System;
using System.Windows.Input;
using MvvmHelpers;
using Xamarin.Forms;

namespace Antioch
{
    public class RoomsViewModel : BaseViewModel
    {
        ITwilioMessenger twilioMessenger;

        public Command ConnectCommand { get; }

        public RoomsViewModel()
        {
            twilioMessenger = DependencyService.Get<ITwilioMessenger>();

            ConnectCommand = new Command(async () =>
            {
                var success = false;
                string message = string.Empty;
                try
                {
                    IsBusy = true;
                    success = true;// await twilioMessenger.InitializeAsync();
                }
                catch (Exception ex)
                {
                    message = ex.Message;
                }
                finally
                {
                    IsBusy = false;
                }

                if (success)
                {
                    await App.Current.MainPage.DisplayAlert("Success", "Now joining #general.", "OK");
                    await App.Current.MainPage.Navigation.PushModalAsync(new MainChatPage());
                }
                else
                {
                    await App.Current.MainPage.DisplayAlert("Sad Monkeys", $"Unable to join #general: {message}" , "OK"); 
                }
            });
        }



    }
}

