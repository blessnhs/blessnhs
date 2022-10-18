using CCA.Popup;
using DependencyHelper;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Essentials;
using CCA.Page;
using CCA.CustomAdMobView;

namespace CCA
{
    public partial class MainPage : ContentPage
    {
        //전면광고
        iAd_IterstitialView iIterstitia;

        //보상광고
        iAd_RewardVideoView rewardVideo;
        public MainPage()
        {
            InitializeComponent();

            NetworkProcess();

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();
            rewardVideo = DependencyService.Get<iAd_RewardVideoView>();
        }

        protected override bool OnBackButtonPressed()
        {
           
            return true;
        }

        void OnTapped(object sender, EventArgs e)
        {
            try
            {
                var sndObject = sender as StackLayout;
                var grid = this.Parent as Grid;

                switch (sndObject.StyleId)
                {
                    case "RegCamera":
                        {
                            var machineid = DependencyService.Get<MethodExt>().MachineId();
                            string Model = DeviceInfo.Model;
                            NetProcess.SendRegCamera(Model,machineid);

 //                           PopupNavigation.Instance.PushAsync(new CameraPage());
                        }
                        break;
                    case "MyCamera":
                        {
                      //      rewardVideo.ShowAd();
                            NetProcess.SendReqCameraList();
                        }
                        break;
                    case "Qna":
                        {
                            iIterstitia.ShowAd();
                            PopupNavigation.Instance.PushAsync(new QnAPage());
                         
                        }
                        break;
                    case "Setting":
                        {
                            PopupNavigation.Instance.PushAsync(new SettingPage());
                        }
                        break;
                    case "Notice":
                        {
                            NetProcess.SendNOTICE();
                        }
                        break;
                        
                }
            }
            catch (Exception ex)
            {
               string str =  ex.Message;

            }
        }

        private void NetworkProcess()
        {
            //network
            {

                //network thread
                Task.Run(() =>
                {
                    while (true)
                    {
                        try
                        {
                            NetProcess.start();
                            NetProcess.client.PacketRecvSync();
                            NetProcess.Loop();

                        }
                        catch (Exception e)
                        {
                            DependencyService.Get<MethodExt>().Notification(e.Message);
                        }
                    }
                });
            }
        }

    }
}
