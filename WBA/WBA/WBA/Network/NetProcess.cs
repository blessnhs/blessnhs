using Android.OS;
using Android.Widget;
using Google.Protobuf;
using Google.Protobuf.WellKnownTypes;
using NetClient;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using WBA.MainTabbedPage;
using WBA.Navigation;
using Xamarin.Forms;

namespace WBA.Network
{
    public static class NetProcess
    {
        static bool isSendVersion = false;
        static public Client client = new Client();
        static public void start()
        {
            client.StartClient("192.168.0.4", 20000);
        }

        public static Page GetPageByTitle(MainPage page, string name)
        {
            foreach (var child in page.Children)
            {
                if (child.Title == name)
                    return child;
            }

            return null;
        }

        static public Page ChatPage = null;


        static public void Loop(MainPage page)
        {
           // while(true)
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
                                }
                                break;
                            case (int)PROTOCOL.IdPktLoginRes:
                                {
                                    MessagingCenter.Send<Setting, CompletePacket>((Setting)GetPageByTitle(page,"설정"), "setting", data);
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
                                            ChatPage = new MainChatView(res.VarRoomId);
                                            page.Navigation.PushModalAsync(new NavigationPage(ChatPage));
                                        });
                                    }

                                }
                                break;
                            case (int)PROTOCOL.IdPktNewUserInRoomNty:
                                {
                                    NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                    res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);
                                }
                                break;
                            case (int)PROTOCOL.IdPktBroadcastRoomMessageRes:
                                {
                                    if (ChatPage != null)
                                    {

                                        BROADCAST_ROOM_MESSAGE_RES res = new BROADCAST_ROOM_MESSAGE_RES();
                                        res = BROADCAST_ROOM_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                        // AddMessage(res.VarName, res.VarMessage);

                                        ((MainChatView)ChatPage).UpdateMessage(data);

                                        // MessagingCenter.Send<MainChatView, CompletePacket>((MainChatView)ChatPage, "maintchatview", data);
                                    }
                                }
                                break;
                            case (int)PROTOCOL.IdPktRoomListRes:
                                {
                                    MessagingCenter.Send<Community, CompletePacket>((Community)GetPageByTitle(page, "포럼"), "community", data);
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
                                            ChatPage = new MainChatView(res.VarRoomId);
                                            page.Navigation.PushModalAsync(new NavigationPage(ChatPage));
                                        });
                                    }
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

        static public void SendRoomMessage(string msg)
        {
            BROADCAST_ROOM_MESSAGE_REQ message = new BROADCAST_ROOM_MESSAGE_REQ
            {
                VarMessage = msg
            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendReqRoomList()
        {
            if (client.socket.Connected == false)
                return;

            ROOM_LIST_REQ person = new ROOM_LIST_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLogin(string id,string pwd)
        {
            LOGIN_REQ person = new LOGIN_REQ
            {
                VarId = id,
                VarPasswd = pwd,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
    }
}
