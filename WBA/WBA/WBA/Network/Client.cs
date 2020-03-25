using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using WBA.Network;

namespace NetClient
{
    public class CompletePacket
    {
        public int Protocol { get; set; }
        public byte[] Data { get; set; }
        public int Length { get; set; }
    }

    public class StateObject
    {
        // Client socket.
        public Socket workSocket = null;
        // Size of receive buffer.
        public static int MTU = 1024 * 20;

        // Receive buffer.
        public byte[] buffer = new byte[MTU];
        // Received data string.
        public StringBuilder sb = new StringBuilder();

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

        private int m_RemainLength = 0;
        private byte[] m_PacketBuffer = new byte[StateObject.MTU];

        public bool GetPacket(ref int protocol, ref byte[] packet, ref int dataLength)
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

                if (PacketLength <= m_RemainLength)         //제대로된 패킷이 왔다
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

        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {

                // Retrieve the socket from the state object.
                Socket client = (Socket)ar.AsyncState;

                // Complete the connection.
                client.EndConnect(ar);

                // Signal that the connection has been made.
                connectDone.Set();

                NetProcess.SendVersion();

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

        public ConcurrentQueue<CompletePacket> PacketQueue = new ConcurrentQueue<CompletePacket>();

        public void OnRecvThreadProc()
        {
            int Protocol = 0;
            int PacketLength = 0;

            byte[] mCompletePacketBuffer = null;

            while (GetPacket(ref Protocol, ref mCompletePacketBuffer, ref PacketLength))
            {
                CompletePacket complete = new CompletePacket();
                complete.Protocol = Protocol;
                complete.Length = PacketLength;
                complete.Data = new byte[PacketLength];

                Buffer.BlockCopy(mCompletePacketBuffer, 0, complete.Data, 0, PacketLength);

                PacketQueue.Enqueue(complete);
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

                    lock (this)
                    {
                        Buffer.BlockCopy(state.buffer, 0, m_PacketBuffer, m_RemainLength, bytesRead);

                        m_RemainLength += bytesRead;

                   
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
}
