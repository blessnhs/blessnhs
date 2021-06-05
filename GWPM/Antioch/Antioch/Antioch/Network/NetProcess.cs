using Google.Protobuf;
using System;
using System.IO;
using Xamarin.Essentials;
using System.Text;
using System.Net;
using System.Threading;
using System.Collections.Concurrent;
using Xamarin.Forms;
using System.Linq;

namespace Antioch
{
    public static class NetProcess
    {
        static public Client client = new Client();

        public static string GetIPAddress(string hostname)
        {
            IPHostEntry host;
            host = Dns.GetHostEntry(hostname);

            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                {
                    //System.Diagnostics.Debug.WriteLine("LocalIPadress: " + ip);
                    return ip.ToString();
                }
            }
            return string.Empty;
        }

        private static DateTime time = new DateTime();
   
        private static DateTime notice_time = new DateTime();
   
        static public void start()
        {
            string ip = "192.168.0.9";

            client.StartClient(ip, 20000);
        }

        public static ConcurrentQueue<MemoryStream> JpegStream = new ConcurrentQueue<MemoryStream>();
        public static ConcurrentQueue<MemoryStream> AudioStream = new ConcurrentQueue<MemoryStream>();


        static public void Loop()
        {
            
            if (client.socket == null || client.socket.Connected == false)
                return;
         
            CompletePacket data;
            if (client.PacketQueue.TryDequeue(out data) == true)
            {
                try
                {
                    switch (data.Protocol)
                    {
                        case (int)PROTOCOL.IdPktVersionRes:
                            {
                                VERSION_RES res = new VERSION_RES();
                                res = VERSION_RES.Parser.ParseFrom(data.Data);

                                var currentVersion = VersionTracking.CurrentVersion;

                                //var myversion = double.Parse(currentVersion);
                                //const Double Eps = 0.000000000000001;

                                //if (Math.Abs(res.VarVersion - myversion) > Eps)
                                //{
                                //    //첫 검수라 임시 주석 2번째 패치 부터는 활성화
                                //    //   Xamarin.Essentials.Browser.OpenAsync("https://play.google.com/store/apps/details?id=com.companyname.OMOK");

                                //    //    return;
                                //}


                                SQLLiteDB.LoadCacheData();
                                if(User.CacheData.UserName != null)
                                NetProcess.SendLogin(User.CacheData.UserName, User.CacheData.Passwd);
                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    User.LoginSuccess = true;
                                    SQLLiteDB.Upsert(User.CacheData.FontSize, User.CacheData.BibleName, User.CacheData.Chapter, User.CacheData.Verse, 
                                        User.Username == null ?  User.CacheData.UserName : User.Username, User.Password == null ? User.CacheData.Passwd : User.Password);
                                }
                                else
                                    App.Current.MainPage.DisplayAlert("Help", "사용자 정보가 다릅니다.", "OK");
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                                Device.BeginInvokeOnMainThread(() =>
                                {

                                    if (Application.Current.MainPage.Navigation.NavigationStack.Count > 0)
                                    {
                                        //LIFO is the only game in town! - so send back the last page

                                        int index = Application.Current.MainPage.Navigation.NavigationStack.Count - 1;

                                        var currPage = Application.Current.MainPage.Navigation.NavigationStack[index];

                                    }

                                    Page currentPage = Application.Current.MainPage.Navigation.NavigationStack.LastOrDefault();

                                    RoomsPage pg = currentPage as RoomsPage;


                                    pg.LoadRoomList();
                                });
                            }
                            break;
                    }
                }
                catch (Exception ex)
                {
                    Console.Write(ex.ToString());
                }
            }
        }

        static public void SendVersion()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            VERSION_REQ person = new VERSION_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktVersionReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLogin(string id,string pwd)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var data = new LOGIN_REQ
            {
                VarId = id,
                VarPwd = pwd
            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var data = new ROOM_LIST_REQ
            {
               
            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

    }
}

