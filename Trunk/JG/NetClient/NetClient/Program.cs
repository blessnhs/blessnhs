using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Net.Json;
using System.IO;
using UDPEchoClient;

using Google.Protobuf;
using NetClient;

public class Process
{
    public Client client = new Client();
    public void start(int id)
    {
        int _id = id;
        client.StartClient("192.168.0.4", 20000);

    }

    public void loop(int _id)
    {
        {
            if (client.socket.Connected == false)
                return;

            client.Update();
            Thread.Sleep(300);

            if (client.PacketQueue.Count > 0)
            {
                CompletePacket data;
                if (client.PacketQueue.TryDequeue(out data) == true)
                {
                    switch (data.Protocol)
                    {
                        case (int)PROTOCOL.IdPktVersionRes:
                            {

                                VERSION_RES res = new VERSION_RES();
                                res = VERSION_RES.Parser.ParseFrom(data.Data);


                                LOGIN_REQ person = new LOGIN_REQ
                                {
                                    Id = PROTOCOL.IdPktLoginRes,
                                    VarId = "nhs" + _id,
                                    VarPasswd = "nhs" + _id,
                                };
                                using (MemoryStream stream = new MemoryStream())
                                {
                                    person.WriteTo(stream);

                                    client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
                                }

                            }

                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                Console.WriteLine("IdPktLoginRes " + res.VarCode.ToString());
                                {
                                    MATCH_REQ person = new MATCH_REQ
                                    {

                                    };
                                    using (MemoryStream stream = new MemoryStream())
                                    {
                                        person.WriteTo(stream);

                                        client.WritePacket((int)PROTOCOL.IdPktMatchReq, stream.ToArray(), stream.ToArray().Length);
                                    }
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                Console.WriteLine("IdPktCreateRoomRes " + res.VarCode.ToString());
                                return;
                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);

                                Console.WriteLine("IdPktNewUserInRoomNty " + res.VarCode.ToString());
                            }
                            break;
                        case (int)PROTOCOL.IdPktBroadcastRoomMessageRes:
                            {
                                BROADCAST_ROOM_MESSAGE_RES res = new BROADCAST_ROOM_MESSAGE_RES();
                                res = BROADCAST_ROOM_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                Console.WriteLine("IdPktBroadcastRoomMessageRes " + res.VarName + " " + res.VarMessage);

                                BROADCAST_ROOM_MESSAGE_REQ req = new BROADCAST_ROOM_MESSAGE_REQ
                                {
                                    VarMessage = "message"
                                };
                                using (MemoryStream stream = new MemoryStream())
                                {
                                    req.WriteTo(stream);

                                    client.WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                BROADCAST_ROOM_MESSAGE_REQ req = new BROADCAST_ROOM_MESSAGE_REQ
                                {
                                    VarMessage = "test message"
                                };
                                using (MemoryStream stream = new MemoryStream())
                                {
                                    req.WriteTo(stream);

                                    client.WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
                                }
                            }
                            break;
                    }
                }

            }
        }
    }

}

public class AsynchronousClient
{
  

   public static int Main(String[] args)
    {

        Process[] array = new Process[3];
        for (int i=1;i<2;i++)
        {
            array[i - 1] = new Process();
            array[i - 1].start(i);
            Thread.Sleep(300);
        }

        Console.WriteLine("job end");


        while (true)
        {
            for (int i = 1; i < 2; i++)
            {
                array[i - 1].loop(i);
                Thread.Sleep(100);
            }
            Thread.Sleep(100);
        };

        return 0;
    }

 }

