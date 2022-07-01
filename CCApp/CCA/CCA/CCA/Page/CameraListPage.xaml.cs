﻿using Google.Protobuf.Collections;
using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace CCA.Page
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class CameraListPage : PopupPage
    {
        public class Contact
        {
            public string Name { get; set; }
            public string Status { get; set; }
            public string MachineId { get; set; }

            public string Content { get; set; }
        }

        public CameraListPage(RepeatedField<global::CameraInfo> recvCamList)
        {
            InitializeComponent();

            var list = new List<Contact>();

        

            foreach (var cam in recvCamList)
            {
                string state = cam.VarPlayerId == 0 ? "off line" : "on line";

                list.Add(new Contact { Name = cam.VarMachineName, 
                    Status = state, 
                    MachineId = cam.VarMachineId,
                    Content = "Name " + cam.VarMachineName + " "+ state
                });

                User.CamDic[cam.VarMachineId] = new RegCam() { MachineId = cam.VarMachineId, MachineModel = cam.VarMachineName,PlayerId = cam.VarPlayerId };
            }

            listView.ItemsSource = list;
            listView.ItemSelected += (object sender, SelectedItemChangedEventArgs e) =>
            {
                if (listView.SelectedItem == null)
                    return;

                Contact item = (Contact)e.SelectedItem;
                var MachinId = item.MachineId;

                RegCam camera;
                if (User.CamDic.TryGetValue(MachinId, out camera) == true)
                {
                    if (camera.PlayerId != 0)
                    {
                        NetProcess.SendWakeUpCamera(camera.PlayerId);

                        PopupNavigation.Instance.PushAsync(new CameraViewer());
                    }
                }

                listView.SelectedItem = null;
            };
        }

        private async void OnCloseButtonTapped(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }

        private async void OnClose(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }
    }
}