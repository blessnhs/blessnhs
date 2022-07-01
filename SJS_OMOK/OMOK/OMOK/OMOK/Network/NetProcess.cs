using Google.Protobuf;
using NetClient;
using OMOK.Views;
using System;
using System.IO;
using Xamarin.Essentials;
using System.Text;
using System.Net;
using System.Threading;
using System.Collections.Concurrent;
using Xamarin.Forms;

namespace OMOK.Network
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


            //연결중이면 안한다. 
            if (client.socket == null || client.socket.Connected == false)
            {
                if ((DateTime.Now - time).TotalSeconds > 5)
                {
                    time = DateTime.Now;
                    if(User.Token != null && User.Token != "")
                        client.StartClient(ip, 1982);
                }
            }

            if ((DateTime.Now - notice_time).TotalSeconds > 30)
            {
                if (client.socket != null && client.socket.Connected == true)
                {
                    notice_time = DateTime.Now;
                    SendNoticeReq();
                    SendReqRoomList();
                }
            }
        }

        public static ConcurrentQueue<MemoryStream> JpegStream = new ConcurrentQueue<MemoryStream>();
        public static ConcurrentQueue<MemoryStream> AudioStream = new ConcurrentQueue<MemoryStream>();

        static Room pRoom = null;

        static public void Loop(Lobby page)
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

                                float myversion = float.Parse(currentVersion);
                                const Double Eps = 0.000000000000001;

                                if (Math.Abs(res.VarVersion - myversion) > Eps)
                                {
                                    //첫 검수라 임시 주석 2번째 패치 부터는 활성화
                                 //   Xamarin.Essentials.Browser.OpenAsync("https://play.google.com/store/apps/details?id=com.companyname.OMOK");

                                //    return;
                                }

                                NetProcess.SendLogin(User.Uid, User.Token);

                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    page.ClosePopup();
                                });

                                if (res.VarCode == ErrorCode.Success)
                                {

                                    User.IsLogin = true;

                                    User.Id = res.VarIndex;
                                    User.myInfo.win = res.VarWin;
                                    User.myInfo.lose = res.VarLose;
                                    User.myInfo.draw = res.VarDraw;
                                    User.myInfo.score = res.VarScore;
                                    User.myInfo.rank = res.VarRank;
                                    User.myInfo.level = res.VarLevel;
                                    User.myInfo.NickName = Helper.ToStr(res.VarName.ToByteArray()); ;
                                    User.Locale = Helper.ToStr(res.VarLocale.ToByteArray());

                                    User.state = PlayerState.Lobby;

                                    SendNoticeReq();

                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        page.UpdatePlayerInfo();
                                        page.UpdateLocalMenu();

                                        if (User.myInfo.NickName == "")
                                        {
                                            page.NickNamePopup();
                                        }
                                        else
                                        {

                                            if (User.Auto == true)
                                                page.MatchAuto();
                                        }
                                    });

                                   
                                }
                                else
                                {
                                    //강제 exception 발생후 종료 후 재 실행
                                    //System.Diagnostics.Process.GetCurrentProcess().Close();
                                    //page.LoginInformation("Failed Login.");
                                    throw new NullReferenceException();
                                }


                            }
                            break;

                        case (int)PROTOCOL.IdPktNoticeRes:
                            {
                                NOTICE_RES res = new NOTICE_RES();
                                res = NOTICE_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    page.SetNotice(Helper.ToStr(res.VarMessage.ToByteArray()));
                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktCheckNicknameRes:
                            {
                                CHECK_NICKNAME_RES res = new CHECK_NICKNAME_RES();
                                res = CHECK_NICKNAME_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (res.VarCode != ErrorCode.Success)
                                        page.SendNickNamePopupMessage();
                                    else
                                    {
                                        User.myInfo.NickName = Helper.ToStr(res.VarName.ToByteArray());

                                        page.CloseAllPopup();

                                        page.UpdatePlayerInfo();
                                    }
                                });

                            }
                            break;

                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        page.ClosePopup();

                                        User.Color = eTeam.Black;
                                        User.IsMyTurn = true;
                              
                                        User.state = PlayerState.Room;

                                        pRoom = new Room();
                                        page.PushRoomPopup(pRoom);
                                    });

                                }
                                else
                                {

                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);
                             
                                {
                                    //상대방이름
                                    //나갈때 초기화한다. 

                                    User.OppInfo.PhotoPath = Encoding.UTF8.GetString(res.VarRoomUser.PictureUri.ToByteArray());

                                    User.OppInfo.NickName = Helper.ToStr(res.VarRoomUser.VarName.ToByteArray());
                                }
                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    pRoom.UpdateBattleInfo();
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktBitmapMessageRes:
                            {
                                BITMAP_MESSAGE_RES res = new BITMAP_MESSAGE_RES();
                                res = BITMAP_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    JpegStream.Enqueue(new MemoryStream(msg.ToByteArray()));
                                }

                            }
                            break;
                          case (int)PROTOCOL.IdPktRoomPassThroughRes:
                            {
                                ROOM_PASS_THROUGH_RES res = new ROOM_PASS_THROUGH_RES();
                                res = ROOM_PASS_THROUGH_RES.Parser.ParseFrom(data.Data);
                                if(res.VarCode == ErrorCode.Success)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        pRoom.ProcReceivePutStoneMessage(res);
                                    });
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    page.UpdateMessage(data);
                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktAudioMessageRes:
                            {
                                AUDIO_MESSAGE_RES res = new AUDIO_MESSAGE_RES();
                                res = AUDIO_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    AudioStream.Enqueue(new MemoryStream(msg.ToByteArray()));
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktRankRes:
                            {
                                RANK_RES res = new RANK_RES();
                                res = RANK_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    page.CreateRankPage(res.VarRankList);
                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                LEAVE_ROOM_RES res = new LEAVE_ROOM_RES();
                                res = LEAVE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if(res.VarIndex == User.Id)
                                {
                                    User.state = PlayerState.Lobby;

                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        pRoom.ShowLeaveAd();

                                        page.PopRoomPopup();
                                    });

                                    if (User.Auto == true)
                                    {
                                        Thread.Sleep(1000);

                                        Device.BeginInvokeOnMainThread(() =>
                                        {
                                            page.MatchAuto();
                                        });
                                    }
                                }

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    page.UpdatePlayerInfo();
                                });

                            }
                            break;

                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        page.ClosePopup();

                                        pRoom = new Room();

                                        User.Color = eTeam.White;
                                        User.IsMyTurn = false;

                                        User.state = PlayerState.Room;
                                        page.PushRoomPopup(pRoom);
                                    });
                                }
                            }
                            break;

                        case (int)PROTOCOL.IdPktGameResultNty:
                            {
                                GAME_RESULT_NTY res = new GAME_RESULT_NTY();
                                res = GAME_RESULT_NTY.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (User.Auto == true)
                                    {
                                        pRoom.GameResult(res);
                                        pRoom.SendLeaveRoom();
                                    }
                                    else
                                    {
                                        pRoom.GameResult(res);
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

        static public void SendNoticeReq()
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

        static public void SendCheckNickName(string Name)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            byte[] in_Name = Helper.ToByteString(Name);

            CHECK_NICKNAME_REQ person = new CHECK_NICKNAME_REQ
            {
                VarName = ByteString.CopyFrom(in_Name)
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCheckNicknameReq, stream.ToArray(), stream.ToArray().Length);
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

        static public void SendReqRoomList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            ROOM_LIST_REQ snd = new ROOM_LIST_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                snd.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendMatch()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            MATCH_REQ person = new MATCH_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktMatchReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRank()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            RANK_REQ person = new RANK_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRankReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLogin(string uid, string token)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            LOGIN_REQ person = new LOGIN_REQ
            {
               VarUid = uid,
                VarToken = token,
  //              VarChannel = 1
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomBITMAPMessage(ConcurrentQueue<System.IO.MemoryStream> list)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();

                foreach(var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };

   
                client.WritePacket((int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }
             
        }

        static public void SendAudioMessage(ConcurrentQueue<System.IO.MemoryStream> list)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                AUDIO_MESSAGE_REQ message = new AUDIO_MESSAGE_REQ();

                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };

                client.WritePacket((int)PROTOCOL.IdPktAudioMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendQNS(string msg)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            QNS_REQ message = new QNS_REQ
            {
                VarMessage = ByteString.CopyFrom(bytearray),

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktQnsReq, stream.ToArray(), stream.ToArray().Length);
            }
        }


        static public void SendPassThroughMessage(int x, int y, eTeam team, string msg="0")
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            ROOM_PASS_THROUGH_REQ message = new ROOM_PASS_THROUGH_REQ
            {
                VarMessage = ByteString.CopyFrom(bytearray),
            };

            int flag = 0;

            Helper.SET_X_Y_COLOR((sbyte)x, (sbyte)y, (byte)(team == eTeam.White ? 0 : 1),ref flag);

            message.VarMessageInt = flag;

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendCancelMatchMessage()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            CANCEL_MATCH_REQ message = new CANCEL_MATCH_REQ
            {
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCancelMatchReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
    }
}

