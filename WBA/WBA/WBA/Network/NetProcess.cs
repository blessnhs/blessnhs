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
                       MessagingCenter.Send<CompletePacket>(data, "recv_packet");
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
                VarPassword = ""
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCreateRoomReq, stream.ToArray(), stream.ToArray().Length);
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
