using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Net.Json;


namespace NetClient
{
    public class UDPClient
    {
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
        IPEndPoint remoteEP;


        public UDPClient()
        {
        }

        public void StartClient(string address, int port)
        {
            // Connect to a remote device.
            try
            {
                IPAddress ip = IPAddress.Parse(address);

                remoteEP = new IPEndPoint(ip, port);

                // Create a TCP/IP socket.
                socket = new Socket(AddressFamily.InterNetwork,
                    SocketType.Dgram, ProtocolType.Udp);

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


        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {

                // Retrieve the socket from the state object.
                Socket client = (Socket)ar.AsyncState;

                // Complete the connection.
                client.EndConnect(ar);

                Send(0, 1, 1);

                // Signal that the connection has been made.
                connectDone.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
     
        void udpReceiveCallback(IAsyncResult ar)
        {
            UdpClient udpSocket = ar.AsyncState as UdpClient;
            IPEndPoint remoteEndPoint = null;

            byte[] receiveBytes = udpSocket.EndReceive(ar, ref remoteEndPoint);

            string receiveData = Encoding.UTF8.GetString(receiveBytes);

            Console.WriteLine(receiveData);
            
        }

        public void Send(int protocol, int playerid, int roomid)
        {

            Int32 PacketLength = sizeof(Int16) + sizeof(Int16);

            byte[] TempBuffer = new byte[PacketLength];

            byte[] bytesProtocol = BitConverter.GetBytes((Int16)playerid);
            Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, 0, sizeof(Int16));

            byte[] bytesProtocol2 = BitConverter.GetBytes((Int16)roomid);
            Buffer.BlockCopy(bytesProtocol2, 0, TempBuffer, sizeof(Int16), sizeof(Int16));

            Send(0,TempBuffer, TempBuffer.Length, remoteEP);
        }

        public void Send(int protocol, byte[] packet, int packetLength, IPEndPoint remoteEP)
        {
  
            //  lock (this)
            {
                Int32 PacketLength = sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    packetLength;

                int mCurrentPacketNumber = 0;

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
                    socket.SendTo(TempBuffer, TempBuffer.Length, SocketFlags.None, remoteEP);
                }
                catch (SocketException e)
                {
                    // 10035 == WSAEWOULDBLOCK
                    if (!e.NativeErrorCode.Equals(10035))
                        Console.Write("Disconnected: error code :" + e.NativeErrorCode + "(" + e.Message + ")");
                }              
            }
        }
    }
}

