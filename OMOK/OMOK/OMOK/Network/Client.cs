﻿using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using OMOK;
using OMOK.Network;

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
        public static int MTU = 1024 * 1024 * 5;

        // Receive buffer.
        public byte[] buffer = new byte[MTU];
      
      
        public void clear()
        {
            Buffer.BlockCopy(buffer, 0, buffer, 0, buffer.Length);
        }

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
                IPAddress ip = IPAddress.Parse(address);

                IPEndPoint remoteEP = new IPEndPoint(ip, port);

                if (socket != null)
                // Create a TCP/IP socket.
                {
                    socket.Dispose();
                    socket = new Socket(AddressFamily.InterNetwork,
                                SocketType.Stream, ProtocolType.Tcp);
                }
                else
                {
                       socket = new Socket(AddressFamily.InterNetwork,
                            SocketType.Stream, ProtocolType.Tcp);
                }

                // Connect to the remote endpoint.
                socket.BeginConnect(remoteEP,
                    new AsyncCallback(ConnectCallback), socket);
             //   connectDone.WaitOne();

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

                PacketLength = BitConverter.ToInt32(m_PacketBuffer, 0);

                if (PacketLength > StateObject.MTU || PacketLength <= 0) // Invalid Packet
                {
                    m_RemainLength = 0;
                    return false;
                }

                if (PacketLength <= m_RemainLength)         //제대로된 패킷이 왔다
                {
                    dataLength = PacketLength - sizeof(Int32) - sizeof(Int16) - sizeof(Int16) - sizeof(Int32);
                    packet = new byte[dataLength];
                    Int32 PacketNumber = 0;

                    protocol = BitConverter.ToInt16(m_PacketBuffer, sizeof(Int32));
                    PacketNumber = BitConverter.ToInt32(m_PacketBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16));

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

        StateObject state = new StateObject();


        private void Receive(Socket client)
        {
            try
            {
                if (client == null || client.Connected == false)
                    return;

                state.clear();
        
                int bytesRead = client.Receive(state.buffer);

                if (bytesRead > 0)
                {

                    Buffer.BlockCopy(state.buffer, 0, m_PacketBuffer, m_RemainLength, bytesRead);

                    m_RemainLength += bytesRead;


                    OnRecvThreadProc();

                }
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

        public int mCurrentPacketNumber = 0;

        public bool WritePacket(int protocol, byte[] packet, int packetLength)
        {
            //  lock (this)
            {
                Int32 PacketLength = sizeof(Int32) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    packetLength;

                mCurrentPacketNumber++;

                byte[] TempBuffer = new byte[PacketLength];
                
                byte[] byteslegnth = BitConverter.GetBytes((Int32)PacketLength);
                Buffer.BlockCopy(byteslegnth, 0, TempBuffer, 0, sizeof(Int32));

                byte[] bytesProtocol = BitConverter.GetBytes((Int16)protocol);
                Buffer.BlockCopy(bytesProtocol, 0, TempBuffer, sizeof(Int32), sizeof(Int16));

                byte[] bytesPacketNumber = BitConverter.GetBytes((Int32)mCurrentPacketNumber);
                Buffer.BlockCopy(bytesPacketNumber, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16), sizeof(Int32));
                Buffer.BlockCopy(packet, 0, TempBuffer, sizeof(Int32) + sizeof(Int16) + sizeof(Int16) + sizeof(Int32), packetLength);

                try
                {
                    // Begin sending the data to the remote device.
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

        public void Send(byte[] packet, int packetLength)
        {

            // Begin sending the data to the remote device.
            socket.BeginSend(packet, 0, packetLength, 0,
                new AsyncCallback(SendCallback), socket);
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
