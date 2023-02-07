using System;
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Xamarin.Forms;


namespace CCA
{
    public class CompletePacket
    {
        public int Protocol { get; set; }
        public byte[] Data { get; set; }
        public int Length { get; set; }
    }

    public class RecvPacketBuffer
    {
        public static int MTU = 1024 * 1024 * 5;

        // Receive buffer.
        public byte[] buffer = new byte[MTU];
    }

    public class Client
    {
        public void ClearSocket()
        {
            socket.Close();
        }

        public Socket socket = null;

        public void StartClient(string address, int port)
        {
            // Connect to a remote device.
            try
            {
                IPAddress ip = IPAddress.Parse(address);

                IPEndPoint remoteEP = new IPEndPoint(ip, port);

                if (socket != null)
                // Create a TCP/IP socket.
                {
                    if (socket.Connected == true)
                    {
                        //이미 접속은 했는데 로그인 실패면 다시 한다.
                        if (User.LoginSuccess == true)
                        {
                            return;
                        }
                    }

                    socket.Close();
                    socket.Dispose();
                        
                    socket = null;
                    socket = new Socket(AddressFamily.InterNetwork,
                                SocketType.Stream, ProtocolType.Tcp);

                }
                else
                {
                    socket = new Socket(AddressFamily.InterNetwork,
                         SocketType.Stream, ProtocolType.Tcp);
                }


                socket.ReceiveTimeout = 10;
                socket.SendTimeout = 2000;

                //이미 로그인했다가 풀렸다 이때는 초기화
                if (User.LoginSuccess == true)
                    User.Clear();

                var result = socket.BeginConnect(remoteEP, null, null);

                bool success = result.AsyncWaitHandle.WaitOne(500, true);
                if (success)
                {
                    socket.EndConnect(result);
                }

                if (socket.Connected == true)
                {
                    NetProcess.SendVersion();
                }

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

                // Signal that the connection has been made.
                //   NetProcess.SendVersion();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private int m_RemainLength = 0;
        private byte[] m_PacketBuffer = new byte[RecvPacketBuffer.MTU];

        public bool GetPacket(ref int protocol, ref byte[] packet, ref int dataLength, ref byte compressflag)
        {
            compressflag = 0;

            if (m_RemainLength <= 4)
                return false;

            Int32 PacketLength = BitConverter.ToInt32(m_PacketBuffer, 0);

            if (PacketLength > RecvPacketBuffer.MTU || PacketLength <= 0) // Invalid Packet
            {
                m_RemainLength = 0;
                return false;
            }

            if (PacketLength <= m_RemainLength)         //제대로된 패킷이 왔다
            {
                dataLength = PacketLength - sizeof(Int32) - sizeof(Int16) - sizeof(Int16) - sizeof(Int32) - sizeof(Byte) - sizeof(long);
                packet = new byte[dataLength];

                protocol = BitConverter.ToInt16(m_PacketBuffer, sizeof(Int32));
                compressflag = (byte)BitConverter.ToChar(m_PacketBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32));

                Buffer.BlockCopy(m_PacketBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32) + sizeof(Byte) + sizeof(long), packet, 0, dataLength);

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

        public void PacketRecvSync()
        {
            try
            {
                if (socket == null || socket.Connected == false)
                    return;

                int bytesRead = socket.Receive(state.buffer);

                if (bytesRead > 0)
                {
                    Buffer.BlockCopy(state.buffer, 0, m_PacketBuffer, m_RemainLength, bytesRead);

                    m_RemainLength += bytesRead;

                    OnRecvPacketProc();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }


        public void PacketRecvSync2()
        {
            try
            {
                if (socket == null || socket.Connected == false)
                    return;

                var readEvent = new AutoResetEvent(false);
                var recieveArgs = new SocketAsyncEventArgs()
                {
                    UserToken = readEvent
                };


                recieveArgs.SetBuffer(state.buffer, 0, RecvPacketBuffer.MTU);
                recieveArgs.Completed += recieveArgs_Completed;
                socket.ReceiveAsync(recieveArgs);

                if (recieveArgs.BytesTransferred == 0)
                {
                    if (recieveArgs.SocketError != SocketError.Success)
                        throw new SocketException((int)recieveArgs.SocketError);
                    //        throw new CommunicationException();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        void recieveArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            try
            {
                lock (this)
                {
                    var are = (AutoResetEvent)e.UserToken;

                    Buffer.BlockCopy(state.buffer, 0, m_PacketBuffer, m_RemainLength, e.BytesTransferred);

                    m_RemainLength += e.BytesTransferred;

                    OnRecvPacketProc();

                    are.Set();
                }
            }
            catch (Exception ee)
            {
                Console.WriteLine(ee.ToString());
            }

        }


        RecvPacketBuffer state = new RecvPacketBuffer();

        public ConcurrentQueue<CompletePacket> PacketQueue = new ConcurrentQueue<CompletePacket>();

        public void OnRecvPacketProc()
        {
            int Protocol = 0;
            int PacketLength = 0;
            byte compressflag = 0;
            byte[] mCompletePacketBuffer = null;

            while (GetPacket(ref Protocol, ref mCompletePacketBuffer, ref PacketLength, ref compressflag))
            {
                if (compressflag == 1)
                {
                    var byteout = GZip.Decompress(mCompletePacketBuffer);

                    CompletePacket complete = new CompletePacket();
                    complete.Protocol = Protocol;
                    complete.Length = byteout.Length;
                    complete.Data = new byte[byteout.Length];

                    Buffer.BlockCopy(byteout, 0, complete.Data, 0, byteout.Length);

                    PacketQueue.Enqueue(complete);
                }
                else
                {
                    CompletePacket complete = new CompletePacket();
                    complete.Protocol = Protocol;
                    complete.Length = PacketLength;
                    complete.Data = new byte[PacketLength];

                    Buffer.BlockCopy(mCompletePacketBuffer, 0, complete.Data, 0, PacketLength);

                    PacketQueue.Enqueue(complete);
                }
            }
        }

        private const int CheckCompressSize = 750;
        private byte mCompressFlag = 0;

        public bool WritePacket(int protocol, byte[] packet, int payloadsize)
        {
            if (socket == null || socket.Connected == false)
                return false;



            if (payloadsize > CheckCompressSize)
            {
                var compress = GZip.Compress(packet);

                Int32 PacketLength = sizeof(Int32) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    sizeof(byte) +
                    sizeof(long) +
                    compress.Length;

                mCompressFlag = 1;

                byte[] TempBuffer = new byte[PacketLength];

                byte[] byteslegnth = BitConverter.GetBytes((Int32)PacketLength);
                Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int32));

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int32), sizeof(Int16));

                byte[] bytesPacketNumber = BitConverter.GetBytes((byte)mCompressFlag);
                Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), sizeof(byte));

                Buffer.BlockCopy(compress, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32) + sizeof(byte) + sizeof(long), compress.Length);

                try
                {
                    socket.Send(TempBuffer);
                }
                catch (SocketException e)
                {
                    // 10035 == WSAEWOULDBLOCK
                    if (!e.NativeErrorCode.Equals(10035))
                        Console.Write("Disconnected: error code :" + e.NativeErrorCode + "(" + e.Message + ")");
                }

                TempBuffer = null;
            }
            else
            {
                Int32 PacketLength = sizeof(Int32) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    sizeof(byte) +
                    sizeof(long) +
                   payloadsize;

                mCompressFlag = 0;

                byte[] TempBuffer = new byte[PacketLength];

                byte[] byteslegnth = BitConverter.GetBytes((Int32)PacketLength);
                Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int32));

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int32), sizeof(Int16));

                var ss = sizeof(byte);
                ss = sizeof(long);

                byte[] bytesPacketNumber = BitConverter.GetBytes((byte)mCompressFlag);
                Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), sizeof(byte));

                Buffer.BlockCopy(packet, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32) + sizeof(byte) + sizeof(long), payloadsize);

                try
                {
                    socket.Send(TempBuffer);
                }
                catch (SocketException e)
                {
                    // 10035 == WSAEWOULDBLOCK
                    if (!e.NativeErrorCode.Equals(10035))
                        Console.Write("Disconnected: error code :" + e.NativeErrorCode + "(" + e.Message + ")");
                }

                TempBuffer = null;
            }

            packet = null;

            return true;
        }
    }
}
