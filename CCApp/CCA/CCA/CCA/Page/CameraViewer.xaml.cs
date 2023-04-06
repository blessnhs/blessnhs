using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Rg.Plugins.Popup.Pages;
using System.IO;

namespace CCA.Page
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class CameraViewer : PopupPage
    {
        public string TargetBatteryLevel;

        public string MachinId;
        public Int64 PlayerId;

        Client _client;
        public bool IsP2P = false;
        public CameraViewer(string _MachinId,Int64 playerId, Client client)
        {
            InitializeComponent();

            MachinId = _MachinId;
            PlayerId = playerId;

            _client = client;

            if (client != null && client.socket != null)
            {
                if (client.socket?.Connected == true)
                    IsP2P = true;
                //연결 성공
                Task.Run(() =>
                {
                    while (client.socket.Connected == true)
                    {

                        client.PacketRecvSync();

                        CompletePacket data;
                        while (client.PacketQueue.TryDequeue(out data) == true)
                        {
                            {
                                BITMAP_MESSAGE_REQ res = new BITMAP_MESSAGE_REQ();
                                res = BITMAP_MESSAGE_REQ.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    NetProcess.JpegStream.Enqueue(wra);
                                }
                            }
                        }


                    }

                 

                });
            }

        }

        protected override void OnAppearing()
        {
            base.OnAppearing();

            StreamWrapper temp;
            while (NetProcess.AudioStream.TryDequeue(out temp));
            while (NetProcess.JpegStream.TryDequeue(out temp)) ;
            while (NetProcess.Mpeg2Stream.TryDequeue(out temp)) ;

        }

        protected override void OnDisappearing()
        {
            if(_client != null)
            {
                _client.socket?.Close();
                _client.socket?.Dispose();
                _client.socket = null;
                _client = null;

            }

            base.OnDisappearing();

            NetProcess.SendStopStream();
        }

    }
}