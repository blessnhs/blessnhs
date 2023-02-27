using DependencyHelper;
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

    static public class RecvPacketBuffer
    {
        public static int MTU = 1024 * 1024 * 5;

        // Receive buffer.
        public static byte[] buffer = new byte[MTU];
    }

   

    public class Client
    {
        public string IPCheck()
        {
            IPHostEntry host = Dns.GetHostEntry(Dns.GetHostName());

            string getIP = string.Empty;
            for (int i = 0; i < host.AddressList.Length; i++)
            {
                if (host.AddressList[i].AddressFamily == AddressFamily.InterNetwork)
                {
                    getIP = host.AddressList[i].ToString();
               //     break;  //볼일끝나면 바로 순환문을 나가게 해야 최적화에 도움이 되요.
                }
            }
            return getIP;

        }

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

                socket.BeginConnect(ip, port, ConnectCallback, socket);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }


        public void StartClient2(string address, int port)
        {
            // Connect to a remote device.
            try
            {

                var ii = IPCheck();

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

                socket.ReceiveTimeout = 500;
                socket.SendTimeout = 2000;

             
                var result = socket.BeginConnect(remoteEP, null, null);

                bool success = result.AsyncWaitHandle.WaitOne(500, true);
                if (success)
                {
                    socket.EndConnect(result);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void ConnectCallback(IAsyncResult IAR)
        {
            try
            {

                Socket tempSocket = (Socket)IAR.AsyncState;

                if (tempSocket == null || tempSocket.Connected == false)
                    return;

                tempSocket.EndConnect(IAR);

                //이미 로그인했다가 풀렸다 이때는 초기화
                if (User.LoginSuccess == true)
                    User.Clear();

                if (socket.Connected == true)
                {
                    NetProcess.SendVersion();
                }


               tempSocket.BeginReceive(RecvPacketBuffer.buffer, 0, RecvPacketBuffer.buffer.Length, SocketFlags.None,
               new AsyncCallback(recieveArgs_Completed), socket);
            }
            catch (SocketException se)
            {
                if (se.SocketErrorCode == SocketError.NotConnected)
                {
                   
                }
            }
        }

        private int m_RemainLength = 0;
        private byte[] m_PacketBuffer = new byte[RecvPacketBuffer.MTU];

        public bool GetPacket(ref int protocol, ref byte[] packet, ref int dataLength, ref byte compressflag)
        {
            try
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
            catch(Exception e)
            {
                DependencyService.Get<MethodExt>().Notification(e.StackTrace.ToString());
                return false;
            }
        }

        public void PacketRecvSync()
        {
            try
            {
                if (socket == null || socket.Connected == false)
                    return;

                int bytesRead = socket.Receive(RecvPacketBuffer.buffer);

                if (bytesRead > 0)
                {
                    Buffer.BlockCopy(RecvPacketBuffer.buffer, 0, m_PacketBuffer, m_RemainLength, bytesRead);

                    m_RemainLength += bytesRead;

                    OnRecvPacketProc();
                }
            }
            catch (Exception e)
            {
               
            }
        }


        public void PacketRecvSync2()
        {
            try
            {
                if (socket == null || socket.Connected == false)
                    return;

               socket?.BeginReceive(RecvPacketBuffer.buffer, 0, RecvPacketBuffer.buffer.Length, SocketFlags.None,
               new AsyncCallback(recieveArgs_Completed), socket);

            
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        void recieveArgs_Completed(IAsyncResult IAR)
        {
            try
            {
                Socket tempSock = (Socket)IAR.AsyncState;
                int nReadSize = tempSock.EndReceive(IAR);
                if (nReadSize > 0)
                {
                    Buffer.BlockCopy(RecvPacketBuffer.buffer, 0, m_PacketBuffer, m_RemainLength, nReadSize);

                    m_RemainLength += nReadSize;

                    OnRecvPacketProc();
                }
                  
            }
            catch (Exception ee)
            {
                Console.WriteLine(ee.ToString());
            }

        }

        public ConcurrentQueue<CompletePacket> PacketQueue = new ConcurrentQueue<CompletePacket>();

        public void OnRecvPacketProc()
        {
            try
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
            catch(Exception e)
            {
                DependencyService.Get<MethodExt>().Notification(e.StackTrace.ToString());
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

                TempBuffer = compress = byteslegnth = bytesProtocol = null;
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
