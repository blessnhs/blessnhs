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
    public partial class Loading : PopupPage
    {
        string pattern = "mm-ss";

        public Loading()
        {
            InitializeComponent();
            DateTime begiTime = DateTime.Now;

            AnimatedTextControl.IsRunning = true;

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    try
                    {
                        int seconds = (int)((DateTime.Now - begiTime).TotalSeconds);


                        time.Text = string.Format("{0:00} : {1:00}", seconds / 60, seconds % 60);

                        if ((seconds / 60) > 1)
                            ClosePopup();
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                });

                return true;
            });
         
        }
        public void ClosePopup()
        {
            Navigation.PopModalAsync();
        }

        public void CreatePopup()
        {
            Navigation.PushPopupAsync(this);
        }


        public bool IsAnimationEnabled { get; private set; } = true;
        protected override void OnAppearing()
        {
            base.OnAppearing();

            FrameContainer.HeightRequest = -1;
            if (!IsAnimationEnabled)
            {
                CloseImage.Rotation = 0;
                CloseImage.Scale = 1;
                CloseImage.Opacity = 1;

                return;
            }

            CloseImage.Rotation = 380;
            CloseImage.Scale = 0.3;
            CloseImage.Opacity = 0;
        }

        protected override async void OnAppearingAnimationEnd()
        {
            if (!IsAnimationEnabled)
                return;

            var translateLength = 400u;

            await Task.WhenAll(
                (new Func<Task>(async () =>
                {
                    await Task.Delay(200);

                }))());

            await Task.WhenAll(
                CloseImage.FadeTo(1),
                CloseImage.ScaleTo(1, easing: Easing.SpringOut),
                CloseImage.RotateTo(0)
                );
        }

        protected override async void OnDisappearingAnimationBegin()
        {
            if (!IsAnimationEnabled)
                return;

            var taskSource = new TaskCompletionSource<bool>();

            var currentHeight = FrameContainer.Height;

            FrameContainer.Animate("HideAnimation", d =>
            {
                FrameContainer.HeightRequest = d;
            },
            start: currentHeight,
            end: 170,
            finished: async (d, b) =>
            {
                await Task.Delay(300);
                taskSource.TrySetResult(true);
            });

            await taskSource.Task;
        }

        private void OnCloseButtonTapped(object sender, EventArgs e)
        {
            ClosePopup();
        }

        protected override bool OnBackgroundClicked()
        {
            ClosePopup();

            return false;
        }
    }
}