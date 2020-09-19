using Google.Protobuf;
using NetClient;
using OMOK.Views;
using System;
using System.IO;
using Xamarin.Essentials;
using Xamarin.Forms;
using System.Text;
using System.Net;
using OMOK.Popup;
using Xamarin.Forms.PlatformConfiguration;

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

            ip = GetIPAddress("blessnhs.iptime.org");


            //연결중이면 안한다. 
            if (client.socket == null || client.socket.Connected == false)
            {
               // if ((DateTime.Now - time).TotalSeconds > 1)
                {
                    time = DateTime.Now;
                    if(User.Token != null && User.Token != "")
                        client.StartClient(ip, 20000);
                }
            }

            if ((DateTime.Now - notice_time).TotalSeconds > 30)
            {
                if (client.socket != null && client.socket.Connected == true)
                {
                    notice_time = DateTime.Now;
                    SendNociticeReq();
                }
            }
        }

        static Room pRoom = new Room();

        static public void Loop(Lobby page)
        {
            
            if (client.socket == null || client.socket.Connected == false)
                return;

            client.Update();

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
                                    Xamarin.Essentials.Browser.OpenAsync("https://play.google.com/store/apps/details?id=com.blessnhs.BAC");

                                    return;
                                }

                                NetProcess.SendLogin(User.Uid, User.Token);

                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

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

                                    page.UpdatePlayerInfo();
                                    page.UpdateLocalMenu();

                                    User.state = PlayerState.Lobby;

                                    SendNociticeReq();

                                    if (User.myInfo.NickName == "")
                                    {
                                        page.NickNamePopup();
                                    }
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

                                page.SetNotice(Helper.ToStr(res.VarMessage.ToByteArray()));
                            }
                            break;

                        case (int)PROTOCOL.IdPktCheckNicknameRes:
                            {
                                CHECK_NICKNAME_RES res = new CHECK_NICKNAME_RES();
                                res = CHECK_NICKNAME_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode != ErrorCode.Success)
                                    page.SendNickNamePopupMessage();
                                else
                                {
                                    page.CloseAllPopup();

                                    page.UpdatePlayerInfo();
                                }

                                User.myInfo.NickName = Helper.ToStr(res.VarName.ToByteArray());
                            }
                            break;

                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    page.CloseMatchInfoPopup();

                                    User.Color = eTeam.Black;
                                    User.IsMyTurn = true;
                              
                                    User.state = PlayerState.Room;

                                    pRoom = new Room();
                                    pRoom.ClearBoard();

                                    //방 생성자는 무조건 흑이며 후에 입장자는 백이다.
                                    pRoom.UpdateTurnBackground(eTeam.Black);
                                    pRoom.RefreshAim();
                                    pRoom.InitTimer();

                                    page.PushRoomPopup(pRoom);

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

                                pRoom.UpdateBattleInfo();

                            }
                            break;
                        case (int)PROTOCOL.IdPktBroadcastRoomMessageRes:
                            {
                                BROADCAST_ROOM_MESSAGE_RES res = new BROADCAST_ROOM_MESSAGE_RES();
                                res = BROADCAST_ROOM_MESSAGE_RES.Parser.ParseFrom(data.Data);
                               
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomPassThroughRes:
                            {
                                ROOM_PASS_THROUGH_RES res = new ROOM_PASS_THROUGH_RES();
                                res = ROOM_PASS_THROUGH_RES.Parser.ParseFrom(data.Data);
                                if(res.VarCode == ErrorCode.Success)
                                {
                                    pRoom.ProcReceivePutStoneMessage(res);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                                page.UpdateMessage(data);
                            }
                            break;

                        case (int)PROTOCOL.IdPktRankRes:
                            {
                                RANK_RES res = new RANK_RES();
                                res = RANK_RES.Parser.ParseFrom(data.Data);

                                page.CreateRankPage(res.VarRankList);
                            }
                            break;

                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                LEAVE_ROOM_RES res = new LEAVE_ROOM_RES();
                                res = LEAVE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if(res.VarIndex == User.Id)
                                {
                                    User.state = PlayerState.Lobby;

                              
                                    pRoom.ClearBoard();

                                    pRoom.ShowLeaveAd();

                                    page.PopRoomPopup();

                                }

                                page.UpdatePlayerInfo();

                            }
                            break;

                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    page.CloseMatchInfoPopup();
                         
                                    pRoom = new Room();

                                    User.Color = eTeam.White;
                                    User.IsMyTurn = false;

                                    User.state = PlayerState.Room;

                                    pRoom.ClearBoard();

                                    //방 생성자는 무조건 흑이므로 흑에 표시
                                    pRoom.UpdateTurnBackground(eTeam.Black);
                                    pRoom.RefreshAim();
                                    pRoom.InitTimer();

                                    page.PushRoomPopup(pRoom);
                                }
                            }
                            break;

                        case (int)PROTOCOL.IdPktGameResultNty:
                            {
                                GAME_RESULT_NTY res = new GAME_RESULT_NTY();
                                res = GAME_RESULT_NTY.Parser.ParseFrom(data.Data);


                                pRoom.GameResult(res);
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
            VERSION_REQ person = new VERSION_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktVersionReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendNociticeReq()
        {
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
            if (client.socket == null || client.socket.Connected == false)
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
            if (client.socket.Connected == false)
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
            if (client.socket.Connected == false)
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

            LOGIN_REQ person = new LOGIN_REQ
            {
               VarUid = uid,
                VarToken = token,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomMessage(string msg)
        {

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            BROADCAST_ROOM_MESSAGE_REQ message = new BROADCAST_ROOM_MESSAGE_REQ
            {
                VarMessage =  ByteString.CopyFrom(bytearray),
              
            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendQNS(string msg)
        {
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

