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
using Antioch.View;
using static Antioch.MainChatView;
using DependencyHelper;

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
                //string ip = "192.168.0.9";
                string ip = GetIPAddress("blessnhs.iptime.org");

                client.StartClient(ip, 20000);

                check_time = DateTime.Now.AddSeconds(5);


            }

            if (notice_time < DateTime.Now)
            {

                SendMailList();

                SendAlaram();

                SendPrayList();

                SendRoomList();

                notice_time = DateTime.Now.AddSeconds(10);
            }
        }

        public static ConcurrentQueue<StreamWrapper> JpegStream = new ConcurrentQueue<StreamWrapper>();
        public static ConcurrentQueue<StreamWrapper> AudioStream = new ConcurrentQueue<StreamWrapper>();

        public static ConcurrentQueue<StreamWrapper> Mpeg2Stream = new ConcurrentQueue<StreamWrapper>();

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

                                double myversion;
                                if(double.TryParse(currentVersion,out myversion) == false)
                                { 
                                }

                                const Double Eps = 0.000000000000001;

                                User.Version = res.VarVersion;

                                if (Math.Abs(res.VarVersion - myversion) > Eps)
                                {
                                    //첫 검수라 임시 주석 2번째 패치 부터는 활성화
                                    if (Device.RuntimePlatform == Device.Android && User.OnceVersionNotify == false)
                                    {
                                        Device.BeginInvokeOnMainThread(() =>
                                        {
                                            User.OnceVersionNotify = true;
                                            DependencyService.Get<Toast>().Notification("New Version Updated");
                                        });
                                    }
                                }


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

                                    User.LoginSuccess = true;
                                    SQLLiteDB.Upsert(User.CacheData.FontSize, User.CacheData.BibleName, User.CacheData.Chapter, User.CacheData.Verse,
                                        User.CacheData.UserName, User.CacheData.Passwd);

                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        mainpage.setting.UpdateLoginState("(Success)");
                                        mainpage.setting.UpdateNameEntry(User.CacheData.UserName);

                                        var page = mainpage.CurrentView();
                                        var settingview = page as SettingView;

                                        if (settingview == null)
                                        {
                                            mainpage.LoadLobby();
                                        }
                                    });

                                }
                                else
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        mainpage.setting.UpdateLoginState("(Failed)");
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

                                    RoomsPageView roompage = mainpage.lobby.roompage as RoomsPageView;

                                    roompage?.LoadRoomList(res);
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktNoticeRes:
                            {

                                NOTICE_RES res = new NOTICE_RES();
                                res = NOTICE_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    mainpage.alarm.LoadList(res);
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktMailListRes:
                            {

                                MAIL_LIST_RES res = new MAIL_LIST_RES();
                                res = MAIL_LIST_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    mainpage.mail.LoadList(res);
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

                                if (res.VarCode != ErrorCode.Success)
                                    break;

                                Device.BeginInvokeOnMainThread(() =>
                                {

                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    RoomsPageView roompage = mainpage.lobby.roompage as RoomsPageView;

                                    User.RoomIdList.Add(res.VarRoomId);

                                    {
                                        User.CurrentChatViewNumber = res.VarRoomId;
                                        var chatview = new MainChatView();
                                        mainpage.lobby.chatpage.Add(res.VarRoomId, chatview);
                                        MainChatView chatpage = chatview;

                                        mainpage.LoadView(chatpage);


                                    }


                                });

                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);
                                
                                if (res.VarCode != ErrorCode.DuplicateEnterRoom && res.VarCode != ErrorCode.Success)
                                    break;

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    try
                                    {
                                        var mainpage = (MainPage)Application.Current.MainPage;

                                        RoomsPageView roompage = mainpage.lobby.roompage as RoomsPageView;

                                        User.RoomIdList.Add(res.VarRoomId);
                                        {
                                            User.CurrentChatViewNumber = res.VarRoomId;

                                            MainChatView outivew;

                                            mainpage.lobby.chatpage.TryGetValue(res.VarRoomId, out outivew);
                                            if (outivew == null) //없으면 새로 만들어서 넣는다.
                                            {
                                                var chatview = new MainChatView();
                                                mainpage.lobby.chatpage.Add(res.VarRoomId, chatview);
                                                mainpage.LoadView(chatview);

                                                foreach (var msg in res.VarMessages)
                                                {
                                                   chatview.ReceiveMessage(msg.VarMessage, msg.VarName, msg.VarTime);
                                                }

                                                chatview.ScrollEnd();
                                            }
                                            else  //이미 존재하는 방이면 해당 방에 넣는다. 
                                            {
                                                mainpage.LoadView(outivew);
                                                outivew.ScrollEnd();
                                            }
                                        }
                                    }
                                    catch (Exception e)
                                    {

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


                                    MainChatView outivew;

                                    mainpage.lobby.chatpage.TryGetValue(res.VarRoomNumber, out outivew);
                                    if (outivew == null)
                                        return;

                                    foreach (var msg in res.VarMessages)
                                    {
                                        outivew.ReceiveMessage(msg.VarMessage, msg.VarName, msg.VarTime);
                                    }

                                    outivew.ScrollEnd();
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

                                    MainChatView outivew;

                                    mainpage.lobby.chatpage.TryGetValue(res.VarRoomNumber, out outivew);
                                    if (outivew == null)
                                        return;

                                    outivew.UserList.Remove(res.VarName);

                                    if (res.VarName == User.CacheData.UserName)
                                    {
                                        User.RoomIdList.Remove(res.VarRoomNumber);

                                        mainpage.lobby.chatpage.Remove(res.VarRoomNumber);

                                        mainpage.lobby.LoadRoomPageView();
                                    }
                                    else
                                    {
                                        MainChatView chatpage = outivew as MainChatView;
                                        chatpage.ReceiveMessage(res.VarName + " leaved.",
                                               res.VarName, Message.type.Info);

                                    }
                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktAudioMessageRes:
                            {
                                AUDIO_MESSAGE_RES res = new AUDIO_MESSAGE_RES();
                                res = AUDIO_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.pos = res.VarPos;
                                    AudioStream.Enqueue(wra);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktBitmapMessageRes:
                            {
                                BITMAP_MESSAGE_RES res = new BITMAP_MESSAGE_RES();
                                res = BITMAP_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.pos = res.VarPos;
                                    wra.type = res.VarType;
                                    JpegStream.Enqueue(wra);
                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktMpeg2TsMessageRes:
                            {
                                MPEG2TS_MESSAGE_RES res = new MPEG2TS_MESSAGE_RES();
                                res = MPEG2TS_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.type = res.VarType;
                                    Mpeg2Stream.Enqueue(wra);
                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;


                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    MainChatView outivew;

                                    mainpage.lobby.chatpage.TryGetValue(res.VarRoomUser.VarRoomNumber, out outivew);
                                    if (outivew == null)
                                        return;

                                    outivew.UserList.Add(res.VarRoomUser.VarName);

                                    if (res.VarType == 1)
                                    {
                                        if (res.VarRoomUser.VarName != User.CacheData.UserName)
                                            outivew.ReceiveMessage(res.VarRoomUser.VarName + " entered.",
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

        static public void SendRoomBITMAPMessage(ConcurrentQueue<System.IO.MemoryStream> list, int type)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                message.VarType = type;
                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };


                client.WritePacket((int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }


        static public void SendRoomMPEG2TSMessage(System.IO.MemoryStream stream, int type)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                MPEG2TS_MESSAGE_REQ message = new MPEG2TS_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                message.VarType = type;
                message.VarMessage.Add(ByteString.CopyFrom(stream.ToArray()));

                client.WritePacket((int)PROTOCOL.IdPktMpeg2TsMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendAudioMessage(ConcurrentQueue<System.IO.MemoryStream> list)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                AUDIO_MESSAGE_REQ message = new AUDIO_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };

                client.WritePacket((int)PROTOCOL.IdPktAudioMessageReq, message.ToByteArray(), message.ToByteArray().Length);

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
                VarRoomNumber = id,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktEnterRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendAlaram()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            NOTICE_REQ person = new NOTICE_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktNoticeReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
        static public void SendMailList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            MAIL_LIST_REQ person = new MAIL_LIST_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktMailListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLeaveRoom(int id)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            LEAVE_ROOM_REQ person = new LEAVE_ROOM_REQ
            {
                VarId = User.CacheData.Id,
                VarRoomNumber = id
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
                VarMessageInt = 0,
                VarRoomNumber = User.CurrentChatViewNumber,
                VarTime = DateTime.Now.ToString("MM/dd/yyyy H:mm")
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
    }
}

