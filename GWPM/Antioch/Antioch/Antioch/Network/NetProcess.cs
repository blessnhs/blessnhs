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

                                ROOM_LIST_RES res = new ROOM_LIST_RES();
                                res = ROOM_LIST_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    RoomsPage roompage = mainpage.lobby.roompage as RoomsPage;

                                    roompage?.LoadRoomList(res);
                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    RoomsPage roompage = mainpage.lobby.roompage as RoomsPage;


                                    {
                                        mainpage.lobby.chatpage = null;
                                        mainpage.lobby.chatpage = new MainChatPage();
                                        MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                        roompage.Navigation.PushModalAsync(chatpage);
                                    }

                                    
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    RoomsPage roompage = mainpage.lobby.roompage as RoomsPage;


                                    {
                                        mainpage.lobby.chatpage = null;
                                        mainpage.lobby.chatpage = new MainChatPage();
                                        MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                        roompage.Navigation.PushModalAsync(chatpage);
                                    }
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomPassThroughRes:
                            {
                                ROOM_PASS_THROUGH_RES res = new ROOM_PASS_THROUGH_RES();
                                res = ROOM_PASS_THROUGH_RES.Parser.ParseFrom(data.Data);
                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;

                                    chatpage.ReceiveMessage(Helper.ToStr(res.VarMessage.ToByteArray()));
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                var mainpage = (MainPage)Application.Current.MainPage;

                                MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                chatpage.Navigation.PopModalAsync();
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

        static public void SendMakeRoom(string Name)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            byte[] in_Name = Helper.ToByteString(Name);

            CREATE_ROOM_REQ person = new CREATE_ROOM_REQ
            {
                VarName = ByteString.CopyFrom(in_Name),
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCreateRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendEnterRoom(int id)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            ENTER_ROOM_REQ person = new ENTER_ROOM_REQ
            {
                VarId = id,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktEnterRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLeaveRoom(int id)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            LEAVE_ROOM_REQ person = new LEAVE_ROOM_REQ
            {
                VarId = id,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLeaveRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomMessage(string msg)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            ROOM_PASS_THROUGH_REQ message = new ROOM_PASS_THROUGH_REQ
            {
                VarMessage = ByteString.CopyFrom(bytearray),
                VarMessageInt = 0
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
    }
}

