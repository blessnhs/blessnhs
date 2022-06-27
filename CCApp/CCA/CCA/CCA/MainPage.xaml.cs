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

namespace CCA
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();

            NetworkProcess();

            PopupNavigation.Instance.PushAsync(new LoginPopup());
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

                            PopupNavigation.Instance.PushAsync(new CameraPage());
                        }
                        break;
                    case "MyCamera":
                        {
                            NetProcess.SendReqCameraList();
                        }
                        break;
                }
            }
            catch (Exception)
            {

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

                        }
                    }
                });
            }
        }

    }
}
