using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Net.Json;


    namespace UDPEchoClient
    {
       public  class UDPClient
        {
            public int ServerPortNumber = 0;
            public UdpClient udpSocket;
            public IPEndPoint remoteEP;

            public UDPClient()
            {
            }

            public void Init()
            {
                try
                {
                    udpSocket = new UdpClient();

                    IPAddress ipAddress;
                    IPAddress ip = IPAddress.Parse("127.0.0.1");

                    remoteEP = new IPEndPoint(ip, ServerPortNumber);

                  //  udpSocket.BeginReceive(udpReceiveCallback, udpSocket);
                }
                catch (SocketException se)
                {
                    Console.WriteLine(se.Message);
                }
            }

            void udpReceiveCallback(IAsyncResult ar)
            {
                
                {
                    UdpClient udpSocket = ar.AsyncState as UdpClient;
                    IPEndPoint remoteEndPoint = null;

                    byte[] receiveBytes = udpSocket.EndReceive(ar, ref remoteEndPoint);

                    string receiveData = Encoding.UTF8.GetString(receiveBytes);

                    Console.WriteLine(receiveData);
                }
            }

            public void Send(int protocol,int playerid,int groupidx)
            {
          
                JsonObjectCollection collection = new JsonObjectCollection();
                collection.Add(new JsonNumericValue("Id", playerid));
                collection.Add(new JsonNumericValue("GroupIdx", groupidx));

                Int32 PacketLength = sizeof(Int16) + sizeof(Int16);

                byte[] TempBuffer = new byte[PacketLength + collection.ToString().Length];

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, 0, sizeof(Int16));

                Buffer.BlockCopy(System.Text.Encoding.UTF8.GetBytes(collection.ToString()), 0, TempBuffer, sizeof(Int16),collection.ToString().Length );

                udpSocket.Send(TempBuffer, TempBuffer.Length, remoteEP);
            }

           public void Send(string Ip,int Port)
            {

                IPAddress ipAddress;
                IPAddress ip = IPAddress.Parse(Ip);

                remoteEP = new IPEndPoint(ip, Port);

                Int32 PacketLength = sizeof(Int16) + sizeof(Int16);

                byte[] TempBuffer = new byte[PacketLength];

                int protocol = 1;

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, 0, sizeof(Int16));

                udpSocket.Send(TempBuffer, TempBuffer.Length, remoteEP);


                udpSocket.Receive(ref remoteEP);
            }
           
        }
    }

