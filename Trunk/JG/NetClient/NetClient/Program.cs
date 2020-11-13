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
using System.Collections.Generic;


public class Process
{

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


    static int testcid = 0;

    public Client client = new Client();
    public void start(int id)
    {
        var ip = GetIPAddress("blessnhs.iptime.org");

         client.StartClient(ip/*"211.212.37.238"*/, 20000);

        testcid = id;

    }


    public void loop()
    {
        {
            if (client.socket.Connected == false)
                return;

            client.Update();
            Thread.Sleep(1);

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
                                    VarToken = testcid + "000000000",
                                    VarUid = testcid.ToString()
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

                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                //string str = "asdfasdfasdf";

                                //BROADCAST_ROOM_MESSAGE_REQ req = new BROADCAST_ROOM_MESSAGE_REQ
                                //{
                                //};

                                //req.VarMessage.CopyTo(Encoding.UTF8.GetBytes(str), 0);

                                //using (MemoryStream stream = new MemoryStream())
                                //{
                                //    req.WriteTo(stream);

                                //    client.WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
                                //}
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
        int id = 1;
        List<Process> array = new List<Process>();


        while (true)
        {
            var cli = new Process();
            array.Add(cli);
            cli.start(id++);
            Thread.Sleep(1);


    //        foreach( var elem in array)
    //        {
    //            elem.loop();
                Thread.Sleep(5);

            //        }

            while (true) ;
            
            Thread.Sleep(1);
        }

        return 0;
    }

 }

