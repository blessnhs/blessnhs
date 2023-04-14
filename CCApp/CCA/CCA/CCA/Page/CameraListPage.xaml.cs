using DependencyHelper;
using Google.Protobuf.Collections;
using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xamarin.Essentials;
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

            public string Ip { get; set; }
        }

        public CameraListPage(RepeatedField<global::CameraInfo> recvCamList)
        {
            InitializeComponent();

            var list = new List<Contact>();

            foreach (var cam in recvCamList)
            {
                string state = cam.VarPlayerId == 0 ? "Off" : "On";

                list.Add(new Contact { Name = cam.VarMachineName, 
                    Status = cam.VarMachineName + " " + state, 
                    MachineId = cam.VarMachineId,
                    Content = cam.VarMachineId,
                    Ip = cam.VarIp
                });

                User.CamDic[cam.VarMachineId] = new RegCam() { Ip = cam.VarIp,MachineId = cam.VarMachineId, MachineName = cam.VarMachineName,PlayerId = cam.VarPlayerId };
            }

            list.Sort((a, b) => a.Status.CompareTo(b.Status));

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
                    //if (camera.PlayerId != 0 && item.Ip != "")
                    //{

                    //    //해당 아이피로 접속 시도
                    //    IPAddress ip = IPAddress.Parse(item.Ip);
                    //    IPEndPoint remoteEP = new IPEndPoint(ip, 1801);

                    //    Client client = new Client();
                    //    client.StartClient2(item.Ip, 1801);

                    //    Thread.Sleep(2000);

                    //    if ( client.socket.Connected)
                    //    {

                    //        PopupNavigation.Instance.PushAsync(new CameraViewer(camera.MachineId, camera.PlayerId, client));
                    //    }
                    //    else
                    //    {
                    //        client.socket.Close();
                    //        client.socket.Dispose();
                    //        client.socket = null;

                    //        NetProcess.SendWakeUpCamera(camera.PlayerId, MachinId);
                    //    }

                    //}
                    //else
                    //{
                    //    NetProcess.SendWakeUpCamera(camera.PlayerId, MachinId);
                    //}

                    NetProcess.SendWakeUpCamera(camera.PlayerId, MachinId);
                }

                listView.SelectedItem = null;
            };
        }

        private async void OnClose(object sender, EventArgs e)
        {
           
            await PopupNavigation.Instance.PopAsync();
        }
        private async void OnBuy(object sender, EventArgs e)
        {
            DependencyService.Get<MethodExt>().Purchase("camera_unlimited");

            await PopupNavigation.Instance.PopAsync();
        }
        private async void OnDelete(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            if (btn != null)
            {
                Grid obj = (Grid)btn.Parent;
                if(obj != null)
                {
                    string machineid = ((Label)obj.Children[0]).Text;
                    NetProcess.SendDelCamera(machineid);

                    User.CamDic.Remove(machineid);

                    var list = new List<Contact>();

                    foreach (var cam in User.CamDic.Values)
                    {
                        string state = cam.PlayerId == 0 ? "Off" : "On";

                        list.Add(new Contact
                        {
                            Name = cam.MachineName,
                            Status = cam.MachineName + " " + state,
                            MachineId = cam.MachineId,
                            Content = cam.MachineId,
                            Ip = cam.Ip
                        });
                    }

                    list.Sort((a, b) => a.Status.CompareTo(b.Status));

                    listView.ItemsSource = list;
                }
            }                    
        }


    }
}