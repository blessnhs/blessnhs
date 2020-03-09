using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Net.Json;
using System.IO;
using UDPEchoClient;

using Google.Protobuf;

public class StateObject
{
    // Client socket.
    public Socket workSocket = null;
    // Size of receive buffer.
    public static int MTU = 200;
 
    // Receive buffer.
    public byte[] buffer = new byte[MTU];
    // Received data string.
    public StringBuilder sb = new StringBuilder();

    public AsynchronousClient Me;
}

public class Client
{
  // The port number for the remote device.
   // private const int port = 8599;
    //private const string address = "127.0.0.1";

    // ManualResetEvent instances signal completion.
    private ManualResetEvent connectDone =
        new ManualResetEvent(false);
    private ManualResetEvent sendDone =
        new ManualResetEvent(false);
    private ManualResetEvent receiveDone =
        new ManualResetEvent(false);

    // The response from the remote device.
    private String response = String.Empty;
    public Socket socket = null;

    public Client(int Index)
    {
        sequence = Index;
    }

    public void StartClient(string address, int port)
    {
        // Connect to a remote device.
        try
        {
            IPAddress ipAddress;
            IPAddress ip = IPAddress.Parse(address);

            IPEndPoint remoteEP = new IPEndPoint(ip, port);

            // Create a TCP/IP socket.
            socket = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);

            // Connect to the remote endpoint.
            socket.BeginConnect(remoteEP,
                new AsyncCallback(ConnectCallback), socket);
            connectDone.WaitOne();

 
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

   public int m_RemainLength = 0;
   public byte[] m_PacketBuffer = new byte[StateObject.MTU];

    public bool GetPacket(ref int protocol, ref  byte[] packet, ref int dataLength)
    {
      //  lock (this)
        {
            if (m_RemainLength <= 4)
                return false;

            Int32 PacketLength = 0;

            PacketLength = BitConverter.ToInt16(m_PacketBuffer, 0);

            if (PacketLength > StateObject.MTU || PacketLength <= 0) // Invalid Packet
            {
                m_RemainLength = 0;
                return false;
            }

            if (PacketLength <= m_RemainLength)			//제대로된 패킷이 왔다
            {
                dataLength = PacketLength - sizeof(Int16) - sizeof(Int16) - sizeof(Int16) - sizeof(Int32);
                packet = new byte[dataLength];
                Int32 PacketNumber = 0;

                protocol = BitConverter.ToInt16(m_PacketBuffer, sizeof(Int16));
                PacketNumber = BitConverter.ToInt32(m_PacketBuffer, sizeof(Int16) + sizeof(Int16) + sizeof(Int16));

                Buffer.BlockCopy(m_PacketBuffer, sizeof(Int16) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), packet, 0, dataLength);

                if (m_RemainLength - PacketLength > 0)
                {
                    Buffer.BlockCopy(m_PacketBuffer, PacketLength, m_PacketBuffer,
                        0, m_RemainLength - PacketLength);
                }
                m_RemainLength -= PacketLength;

                if (m_RemainLength <= 0)
                {
                    m_RemainLength = 0;
                    //memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
                }
                return true;
            }
            else
                return false;


        }
    }

    public int sequence = 1;
    public Client Front;
    public int isconnected = 0;

    string id; 
    string sessionkey; 

    private void ConnectCallback(IAsyncResult ar)
    {
        try
        {

            // Retrieve the socket from the state object.
            Socket client = (Socket)ar.AsyncState;

            // Complete the connection.
            client.EndConnect(ar);

            Console.WriteLine("Socket connected to {0}",
                client.RemoteEndPoint.ToString());

            // Signal that the connection has been made.
            connectDone.Set();

            isconnected = 1;

            lock (this)
            {
                if (ConnectPos == 0)
                {
                    string read = Console.ReadLine();
                    LOGIN_REQ person = new LOGIN_REQ
                    {
                        Id = PROTOCOL.IdPktLoginRes,
                        VarId = read,
                        VarPasswd = read,
                    };
                    using (MemoryStream stream = new MemoryStream())
                    {
                        person.WriteTo(stream);

                        WritePacket((int)PROTOCOL.IdPktLoginReq,stream.ToArray(), stream.ToArray().Length);
                    }
                    
               
                    ConnectPos = 1;



                }
            }

 //          WritePacket(30000, System.Text.Encoding.UTF8.GetBytes(SrcBuffer), SrcBuffer.Length);
 
         }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public void Update()
    {
           Receive(socket);
    }


    private void Receive(Socket client)
    {
        try
        {
            // Create the state object.
            StateObject state = new StateObject();
            state.workSocket = client;

            // Begin receiving the data from the remote device.
            client.BeginReceive(state.buffer, 0, StateObject.MTU, 0,
                new AsyncCallback(ReceiveCallback), state);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public byte ConnectPos = 0;
    public UDPClient udpsocket = null;

    void udpReceiveCallback(IAsyncResult ar)
    {
       
        {
            UdpClient udpSocket = ar.AsyncState as UdpClient;
            IPEndPoint remoteEndPoint = null;

            byte[] receiveBytes = udpSocket.EndReceive(ar, ref remoteEndPoint);

            Console.Write("{0}", receiveBytes);
        }
    }

    public int repeatcount = 0;

    public void OnRecvThreadProc()
    {
        int Protocol = 0;
        int PacketLength = 0;
       
        byte[] mCompletePacketBuffer = null;

        while (GetPacket(ref Protocol, ref mCompletePacketBuffer, ref PacketLength))
        {
                switch (Protocol)
                {
                    case (int)PROTOCOL.IdPktLoginRes:
                        {
                             LOGIN_RES res = new LOGIN_RES();
                             res = LOGIN_RES.Parser.ParseFrom(mCompletePacketBuffer);

                             Console.WriteLine("IdPktLoginRes " + res.VarCode.ToString());

                       string readkey = Console.ReadLine();
                        if (readkey == "0")
                        {
                            CREATE_ROOM_REQ person = new CREATE_ROOM_REQ
                            {
                                VarName = "nhs1_createRoom",
                                VarPassword = "nhs1",
                            };
                            using (MemoryStream stream = new MemoryStream())
                            {
                                person.WriteTo(stream);

                                WritePacket((int)PROTOCOL.IdPktCreateRoomReq, stream.ToArray(), stream.ToArray().Length);
                            }
                        }
                        else
                        {
                            ENTER_ROOM_REQ person = new ENTER_ROOM_REQ
                            {
                                VarId = 1,
                                VarPassword = "1"
                            };
                            using (MemoryStream stream = new MemoryStream())
                            {
                                person.WriteTo(stream);

                                WritePacket((int)PROTOCOL.IdPktEnterRoomReq, stream.ToArray(), stream.ToArray().Length);
                            }

                        }


                    }
                        break;
                case (int)PROTOCOL.IdPktCreateRoomRes:
                    {
                        CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                        res = CREATE_ROOM_RES.Parser.ParseFrom(mCompletePacketBuffer);

                        Console.WriteLine("IdPktCreateRoomRes " + res.VarCode.ToString());
                    }
                    break;
                case (int)PROTOCOL.IdPktNewUserInRoomNty:
                    {
                        NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                        res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(mCompletePacketBuffer);

                        Console.WriteLine("IdPktNewUserInRoomNty " + res.VarCode.ToString());
                    }
                    break;
                case (int)PROTOCOL.IdPktBroadcastRoomMessageRes:
                    {
                        BROADCAST_ROOM_MESSAGE_RES res = new BROADCAST_ROOM_MESSAGE_RES();
                        res = BROADCAST_ROOM_MESSAGE_RES.Parser.ParseFrom(mCompletePacketBuffer);

                        Console.WriteLine("IdPktBroadcastRoomMessageRes " + res.VarName + " " + res.VarMessage);

                        string message = Console.ReadLine();

                        BROADCAST_ROOM_MESSAGE_REQ req = new BROADCAST_ROOM_MESSAGE_REQ
                        {
                          VarMessage = message
                        };
                        using (MemoryStream stream = new MemoryStream())
                        {
                            req.WriteTo(stream);

                            WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
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

                            WritePacket((int)PROTOCOL.IdPktBroadcastRoomMessageReq, stream.ToArray(), stream.ToArray().Length);
                        }
                    }
                    break;
            }
   
         }  
    }

    private void ReceiveCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the state object and the client socket 
            // from the asynchronous state object.
            StateObject state = (StateObject)ar.AsyncState;
            Socket client = state.workSocket;

            // Read data from the remote device.
            int bytesRead = client.EndReceive(ar);

            if (bytesRead > 0)
            {
                // There might be more data, so store the data received so far.
                state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));

                Buffer.BlockCopy(state.buffer, 0, m_PacketBuffer, m_RemainLength, bytesRead);

                m_RemainLength += bytesRead;

                lock (this)
                {
                    OnRecvThreadProc();
                }
     
                // Get the rest of the data.
                client.BeginReceive(state.buffer, 0, StateObject.MTU, 0,
                    new AsyncCallback(ReceiveCallback), state);
            }
            else
            {
                // All the data has arrived; put it in response.
                if (state.sb.Length > 1)
                {
                    response = state.sb.ToString();
                }
                // Signal that all bytes have been received.
                receiveDone.Set();
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }


    public int mCurrentPacketNumber = 0;

    public bool WritePacket(int protocol, byte[] packet, int packetLength)
    {
      //  lock (this)
        {
           Int32 PacketLength = sizeof(Int16) +
               sizeof(Int16) +
               sizeof(Int16) +
               sizeof(Int32) +
               packetLength;

            mCurrentPacketNumber++;

            byte[] TempBuffer = new byte[PacketLength];
            byte[] byteslegnth = BitConverter.GetBytes((Int16)PacketLength);
            Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int16));

            byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
            Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int16), sizeof(Int16));

            byte[] bytesPacketNumber = BitConverter.GetBytes((Int32)mCurrentPacketNumber);
            Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int16) + sizeof(Int16) + sizeof(Int16), sizeof(Int32));
            Buffer.BlockCopy(packet, 0, TempBuffer, sizeof(Int16) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), packetLength);

            try
            {
                // Begin sending the data to the remote device.
                socket.BeginSend(TempBuffer, 0, PacketLength, 0,
                    new AsyncCallback(SendCallback), socket);
            }
            catch (SocketException e)
            {
                // 10035 == WSAEWOULDBLOCK
                if (!e.NativeErrorCode.Equals(10035))
                    Console.Write("Disconnected: error code :" + e.NativeErrorCode + "(" + e.Message + ")");
            }
        }
            return true;
    }


    private void Send(Socket client, String data)
    {
        // Convert the string data to byte data using ASCII encoding.
        byte[] byteData = Encoding.ASCII.GetBytes(data);

        // Begin sending the data to the remote device.
        client.BeginSend(byteData, 0, byteData.Length, 0,
            new AsyncCallback(SendCallback), client);
    }

    private void SendCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.
            Socket client = (Socket)ar.AsyncState;

            // Complete sending the data to the remote device.
            int bytesSent = client.EndSend(ar);
     
            // Signal that all bytes have been sent.
            sendDone.Set();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

}

public class Process
{
    public int max = 2;

    public Client[] login;
    public Client[] front;

     public int i = 1;

    public void start()
    {

     //   Thread t3 = new Thread(delegate ()
    //    {
            Client login = new Client(i);
            login.sequence = i;

            login.ConnectPos = 0;
            login.StartClient("127.0.0.1", 20000);

    //    });
     //   t3.Start();
        Thread.Sleep(100);


        while (true)
        {
            login.Update();
            Thread.Sleep(100);
        }

    }
            
      
     


    


  /*      login = new Client[max]; 
        front = new Client[max];
        for (int i = 1; i < max; i++)
        {
            login[i] = new Client(i);
            login[i].sequence = i;

            front[i] = new Client(i);
            front[i].sequence = i;

            login[i].Front = front[i];

            //릴레이 서버 테스트 인자;
            login[i].ConnectPos = 0;
            login[i].StartClient("127.0.0.1", 8599);

            Thread.Sleep(1);
        }

     
        while (true)
        {
            for (int i = 1; i < max; i++)
            {
                login[i].Update();
        //        front[i].Update();

             
                Thread.Sleep(100);

            }
        }*/
    
}



public class AsynchronousClient
{
  

   public static int Main(String[] args)
    {
        Process clt = new Process();

        clt.start();

        return 0;
    }

 }

