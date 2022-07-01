using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Net.Json;
using System.IO;

using Google.Protobuf;
using NetClient;
using System.Collections.Generic;
using System.Threading.Tasks;

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

        client.id = testcid = id;

        if (id % 2 == 0)
            client.StartClient("192.168.0.9", 23000);
        else
            client.StartClient("192.168.0.9", 23000);


    }



    public void loop()
    {
        {
            if (client.socket.Connected == false)
                return;
    
            if (client.PacketQueue.Count > 0)
            {
                CompletePacket data;
                if (client.PacketQueue.TryDequeue(out data) == true)
                {
                    switch (data.Protocol)
                    {
                        case (int)PROTOCOL.IdPktVersionRes:
                            {
                                System.Console.WriteLine(client.id);

                                VERSION_RES res = new VERSION_RES();
                                res = VERSION_RES.Parser.ParseFrom(data.Data);
                           
                                LOGIN_REQ person = new LOGIN_REQ
                                {
                                    Id = PROTOCOL.IdPktLoginReq,
                                    VarId = client.id.ToString(),
                                    VarPwd = client.id.ToString()
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

                                Console.WriteLine("IdPktLoginRes " + res.VarIndex +" ret " + res.VarCode.ToString());
                                {
                                    ROOM_LIST_REQ person = new ROOM_LIST_REQ
                                    {
                                    };
                                    using (MemoryStream stream = new MemoryStream())
                                    {
                                        person.WriteTo(stream);

                                        client.WritePacket((int)PROTOCOL.IdPktRoomListReq, stream.ToArray(), stream.ToArray().Length);
                                    }
                                }                                
                            }
                            break;
                        case (int)PROTOCOL.IdPktClientKick:
                            {
                                client.socket.Close();
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

                            }
                    
                            break;

                        case (int)PROTOCOL.IdPktRoomPassThroughRes:
                            {
                                ROOM_PASS_THROUGH_RES res = new ROOM_PASS_THROUGH_RES();
                                res = ROOM_PASS_THROUGH_RES.Parser.ParseFrom(data.Data);

                                Console.WriteLine("IdPktRoomPassThroughRes ");
                                ROOM_PASS_THROUGH_REQ req = new ROOM_PASS_THROUGH_REQ();
                                req.VarMessage = "chat test" + client.chat_count.ToString();
                                req.VarRoomNumber = client.room_number;
                                req.VarMessageInt = 0;
                                req.VarTime = DateTime.Now.ToString();
                                using (MemoryStream stream = new MemoryStream())
                                {
                                    req.WriteTo(stream);

                                    client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
                                }

                                client.chat_count++;

                                if(client.chat_count >= 100)
                                {
                                    Console.WriteLine("chat count 100 close socket ");

                                    //client.socket.Close();
                                    //client.socket.Dispose();
                                    //client.socket = null;

                                }
                            }
                            break;

                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode != ErrorCode.Success)
                                    break;

                                client.room_number = res.VarRoomId;

                                ROOM_PASS_THROUGH_REQ req = new ROOM_PASS_THROUGH_REQ();
                                req.VarMessage = "chat test" + client.chat_count.ToString();
                                req.VarRoomNumber = client.room_number;
                                req.VarMessageInt = 0;
                                req.VarTime = DateTime.Now.ToString();

                                using (MemoryStream stream = new MemoryStream())
                                {
                                    req.WriteTo(stream);

                                    client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
                                }

                            }
                            break;

                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                               
                                        ROOM_LIST_RES res = new ROOM_LIST_RES();
                                        res = ROOM_LIST_RES.Parser.ParseFrom(data.Data);

                                        Random rnd = new Random();

                                        //if (res.VarRoomList.Count == 0)
                                        //{
                                        //    CREATE_ROOM_REQ req = new CREATE_ROOM_REQ();
                                        //    req.VarName = "check" + client.id;
                                        //    using (MemoryStream stream = new MemoryStream())
                                        //    {
                                        //        req.WriteTo(stream);

                                        //        client.WritePacket((int)PROTOCOL.IdPktCreateRoomReq, stream.ToArray(), stream.ToArray().Length);
                                        //    }
                                        //}
                                        //else
                                        {

                                            int r = rnd.Next() % res.VarRoomList.Count;

                                            var room = res.VarRoomList[r];

                                            ENTER_ROOM_REQ req = new ENTER_ROOM_REQ();
                                            req.VarRoomNumber = room.VarId;
                                            using (MemoryStream stream = new MemoryStream())
                                            {
                                                req.WriteTo(stream);

                                                client.WritePacket((int)PROTOCOL.IdPktEnterRoomReq, stream.ToArray(), stream.ToArray().Length);
                                            }
                                        }
                               

                            }
                            break;
                }

            }

                data.Data = null;
        }
    }

}

    public class AsynchronousClient
    {


        public static int Main(String[] args)
        {
            int id = 1;
            List<Process> array = new List<Process>();

            List<Process> remove_array = new List<Process>();


        
            while (true)
            {
                if(array.Count == 0)
                {
                    for (int i = 0; i < 500; i++)
                    {
                        if (id > 3000)
                            id = 1;

                        var cli = new Process();
                        array.Add(cli);
                        cli.start(id++);
                    }

                }
                Thread.Sleep(100);

                foreach (var cl in array)
                {
                    if (cl == null || cl.client == null || cl.client.socket == null)
                    {
                        remove_array.Add(cl);
                        continue;
                    }

                 //   Task t2 = Task.Run(() =>
                 //   {
                        cl.client.Update();
                        cl.loop();
                  //  });
                  
                }

                Thread.Sleep(10);

                foreach (var cl in remove_array)
                {
                    array.Remove(cl);
                    cl.client = null;
                }

                Thread.Sleep(10);
            }

            return 0;
        }
    }

 }

