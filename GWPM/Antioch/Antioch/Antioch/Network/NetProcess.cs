﻿using Google.Protobuf;
using System;
using System.IO;
using Xamarin.Essentials;
using System.Text;
using System.Net;
using System.Threading;
using System.Collections.Concurrent;
using Xamarin.Forms;
using System.Linq;
using Antioch.View;

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

        private static DateTime check_time = new DateTime();

        private static DateTime notice_time = new DateTime();

        static public void start()
        {
            if (check_time < DateTime.Now)
            {
                // string ip = "211.212.37.238";//"192.168.0.9"
                string ip = "192.168.0.9";

                client.StartClient(ip, 20000);

                check_time = DateTime.Now.AddSeconds(5);
            }
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
                                if (User.CacheData.UserName != null)
                                    NetProcess.SendLogin(User.CacheData.UserName, User.CacheData.Passwd);
                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;

                                if (res.VarCode == ErrorCode.Success)
                                {

                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        mainpage.setting.UpdateLoginState(User.Username, "(접속 성공)접속중 입니다.");
                                    });

                                    User.LoginSuccess = true;
                                    SQLLiteDB.Upsert(User.CacheData.FontSize, User.CacheData.BibleName, User.CacheData.Chapter, User.CacheData.Verse,
                                        User.Username == null ? User.CacheData.UserName : User.Username, User.Password == null ? User.CacheData.Passwd : User.Password);


                                    User.CacheData.UserName = User.Username;
                                    User.CacheData.Passwd = User.Password;
                                }
                                else
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        mainpage.setting.UpdateLoginState(User.Username, "(인증 실패)");
                                    });

                                    User.LoginSuccess = false;
                                }
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
                        case (int)PROTOCOL.IdPktPrayMessageRes:
                            {

                                PRAY_MESSAGE_RES res = new PRAY_MESSAGE_RES();
                                res = PRAY_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    PrayView prayview = mainpage.lobby.praypage as PrayView;

                                    prayview?.LoadPrayList(res);
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

                                    chatpage.ReceiveMessage(res.VarMessage, res.VarName);
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                LEAVE_ROOM_RES res = new LEAVE_ROOM_RES();
                                res = LEAVE_ROOM_RES.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (res.VarName == User.CacheData.UserName)
                                    {
                                        MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                        chatpage.Navigation.PopModalAsync();
                                    }
                                    else
                                    {
                                        MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                        chatpage.ReceiveMessage(res.VarName + " leaved.",
                                               res.VarName, Message.type.Info);

                                    }
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    MainChatPage chatpage = mainpage.lobby.chatpage as MainChatPage;
                                    if (res.VarType == 1)
                                    {
                                        if (res.VarRoomUser.VarName != User.CacheData.UserName)
                                            chatpage.ReceiveMessage(res.VarRoomUser.VarName + " entered.",
                                            res.VarRoomUser.VarName, Message.type.Info);
                                    }

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

        static public void SendLogin(string id, string pwd)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;


            if (id == null || pwd == null || User.LoginSuccess == true)
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

        static public void SendMakePray(string Content)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
      
            PRAY_MESSAGE_REG_REQ person = new PRAY_MESSAGE_REG_REQ
            {
                VarMessage = Content,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktPrayMessageRegReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendPrayList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var data = new PRAY_MESSAGE_REQ
            {

            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktPrayMessageReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendQNS(string msg)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            QNA_REQ message = new QNA_REQ
            {
                VarMessage = msg,

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktQnaReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendMakeRoom(string Name)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            CREATE_ROOM_REQ person = new CREATE_ROOM_REQ
            {
                VarName = Name,
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

           // var bytearray = System.Text.Encoding.GetEncoding(949).GetBytes(msg);

           // var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            ROOM_PASS_THROUGH_REQ message = new ROOM_PASS_THROUGH_REQ
            {
                VarMessage = msg,
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
