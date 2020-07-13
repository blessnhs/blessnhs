using Google.Protobuf;
using NetClient;
using OMOK.Views;
using System;
using System.IO;
using Xamarin.Essentials;
using Xamarin.Forms;
using System.Text;
using System.Net;

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

        static public void start()
        {
            string ip="192.168.0.9";

        //    ip = GetIPAddress("blessnhs.iptime.org");
            //연결중이면 안한다. 
            if (client.socket == null || client.socket.Connected == false)
            {
                if((DateTime.Now - time).TotalSeconds > 5)
                {
                    time = DateTime.Now;
                    client.StartClient(ip, 20000);
                }
            }
        }

        static public bool IsSuccessAuth = false;
        static public string UserId;

        static public void Loop(MainPage page)
        {
            
            if (client.socket.Connected == false)
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
                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    IsSuccessAuth = true;
                                    User.Id = res.VarIndex;
                                    User.myInfo.win = res.VarWin;
                                    User.myInfo.lose = res.VarLose;
                                    User.myInfo.draw = res.VarDraw;
                                    User.myInfo.score = res.VarScore;
                                    User.myInfo.rank = res.VarRank;
                                    User.myInfo.level = res.VarLevel;

                                    ((Lobby)page.Children[0]).UpdatePlayerInfo();

                                    User.state = PlayerState.Lobby;
                                }
                                else
                                    ((Lobby)page.Children[0]).LoginInformation("로그인에 실패했습니다.");
                            }
                            break;
                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    ((Lobby)page.Children[0]).CloseMatchInfoPopup();

                                    User.Color = eTeam.Black;
                                    page.CurrentPage = page.Children[1];
                                    User.IsMyTurn = true;
                              
                                    User.state = PlayerState.Room;

                                    var Room = (Room)page.Children[1];
                                    Room.UpdateTurnBackground(User.Color);
                                    Room.RefreshAim();
                                    Room.InitTimer();
                                }
                                else
                                {

                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                var lobby = (Lobby)page.Children[0];
                                
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);
                                var Room = (Room)page.Children[1];
                                {
                                    //상대방이름
                                    //나갈때 초기화한다. 

                                    User.OppInfo.PhotoPath = Encoding.UTF8.GetString(res.VarRoomUser.PictureUri.ToByteArray());

                                    User.OppInfo.NickName = Helper.ToStr(res.VarRoomUser.VarName.ToByteArray());
                                }

                                Room.ClearBoard();

                                Room.UpdateBattleInfo();

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

                                var Room = (Room)page.Children[1];

                                Room.ProcReceivePutStoneMessage(res);

                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                                var lobby = (Lobby)page.Children[0];

                                lobby.UpdateMessage(data);
                            }
                            break;

                        case (int)PROTOCOL.IdPktRankRes:
                            {
                                RANK_RES res = new RANK_RES();
                                res = RANK_RES.Parser.ParseFrom(data.Data);

                                ((Lobby)page.Children[0]).CreateRankPage(res.VarRankList);
                            }
                            break;

                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                LEAVE_ROOM_RES res = new LEAVE_ROOM_RES();
                                res = LEAVE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if(res.VarIndex == User.Id)
                                {
                                    User.state = PlayerState.Lobby;

                                    page.CurrentPage = page.Children[0];

                                    ((Room)page.Children[1]).ClearBoard();

                                    ((Room)page.Children[1]).ShowLeaveAd();

                                }

                                ((Lobby)page.Children[0]).UpdatePlayerInfo();

                            }
                            break;

                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    ((Lobby)page.Children[0]).CloseMatchInfoPopup();

                                    User.Color = eTeam.White;
                                    page.CurrentPage = page.Children[1];
                                    User.IsMyTurn = false;

                                    User.state = PlayerState.Room;

                                    var Room = (Room)page.Children[1];
                                    Room.UpdateTurnBackground(User.Color);
                                    Room.RefreshAim();
                                    Room.InitTimer();
                                }
                            }
                            break;

                        case (int)PROTOCOL.IdPktGameResultNty:
                            {
                                GAME_RESULT_NTY res = new GAME_RESULT_NTY();
                                res = GAME_RESULT_NTY.Parser.ParseFrom(data.Data);


                                ((Room)page.Children[1]).GameResult(res);
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
            if (client.socket.Connected == false)
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
            UserId = uid;

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
    }
}

