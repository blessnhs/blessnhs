using System;
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using OMOK.Network;

namespace NetClient
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
                        return;
                    }

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

                socket.Connect(remoteEP);

                if (socket.Connected == true)
                    NetProcess.SendVersion();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private int m_RemainLength = 0;
        private byte[] m_PacketBuffer = new byte[RecvPacketBuffer.MTU];

        public bool GetPacket(ref int protocol, ref byte[] packet, ref int dataLength,ref int compressflag)
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
                dataLength = PacketLength - sizeof(Int32) - sizeof(Int16) - sizeof(Int16) - sizeof(Int32);
                packet = new byte[dataLength];
            
                protocol = BitConverter.ToInt16(m_PacketBuffer, sizeof(Int32));
                compressflag = BitConverter.ToInt32(m_PacketBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16));

                Buffer.BlockCopy(m_PacketBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), packet, 0, dataLength);

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

        RecvPacketBuffer state = new RecvPacketBuffer();

        public ConcurrentQueue<CompletePacket> PacketQueue = new ConcurrentQueue<CompletePacket>();

        public void OnRecvPacketProc()
        {
            int Protocol = 0;
            int PacketLength = 0;
            int compressflag = 0;
            byte[] mCompletePacketBuffer = null;

            while (GetPacket(ref Protocol, ref mCompletePacketBuffer, ref PacketLength,ref compressflag))
            {
                if(compressflag == 1)
                {
                    var byteout = CLZF2.Decompress(mCompletePacketBuffer);

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
        private int mCompressFlag = 0;

        public bool WritePacket(int protocol, byte[] packet, int payloadsize)
        {

            if(payloadsize > CheckCompressSize)
            {
                var compress = CLZF2.Compress(packet);

                Int32 PacketLength = sizeof(Int32) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    compress.Length;

                mCompressFlag = 1;

                byte[] TempBuffer = new byte[PacketLength];
                
                byte[] byteslegnth = BitConverter.GetBytes((Int32)PacketLength);
                Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int32));

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int32), sizeof(Int16));

                byte[] bytesPacketNumber = BitConverter.GetBytes((Int32)mCompressFlag);
                Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16), sizeof(Int32));
          
                Buffer.BlockCopy(compress, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), compress.Length);

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
                  payloadsize;

               mCompressFlag = 0;

               byte[] TempBuffer = new byte[PacketLength];

               byte[] byteslegnth = BitConverter.GetBytes((Int32)PacketLength);
               Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int32));

               byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
               Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int32), sizeof(Int16));

               byte[] bytesPacketNumber = BitConverter.GetBytes((Int32)mCompressFlag);
               Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16), sizeof(Int32));

               Buffer.BlockCopy(packet, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), payloadsize);

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

            return true;
        }
    }
}
