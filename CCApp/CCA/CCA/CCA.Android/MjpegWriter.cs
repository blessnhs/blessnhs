
// -------------------------------------------------
// Developed By : Ragheed Al-Tayeb
// e-Mail       : ragheedemail@gmail.com
// Date         : April 2012
// -------------------------------------------------
using CCA;
using Google.Protobuf;
using System;
using System.Collections.Concurrent;
using System.Net.Sockets;

namespace rtaNetworking.Streaming
{

    /// <summary>
    /// Provides a stream writer that can be used to write images as MJPEG 
    /// or (Motion JPEG) to any stream.
    /// </summary>
    public class MjpegWriter : System.IDisposable
    {

        // private static byte[] CRLF = new byte[] { 13, 10 };
        // private static byte[] EmptyLine = new byte[] { 13, 10, 13, 10};
        // private string _Boundary;


        public MjpegWriter(System.IO.Stream stream)
            : this(stream, "--boundary")
        {

        }


        public MjpegWriter(System.IO.Stream stream, string boundary)
        {

            this.Stream = stream;
            this.Boundary = boundary;
        }



        public string Boundary { get; private set; }
        public System.IO.Stream Stream { get; private set; }


        public void WriteHeader()
        {

            Write(
                    "HTTP/1.1 200 OK\r\n" +
                    "Content-Type: multipart/x-mixed-replace; boundary=" +
                    this.Boundary +
                    "\r\n"
                 );

            this.Stream.Flush();
        }


        //private static void hejHopp()
        //{
        //    //http://www.codeproject.com/Articles/7388/A-Simple-C-Wrapper-for-the-AviFile-Library

        //    //myReadyNAS device, got files via FTP from my webcam
        //    var jpgFileList = Directory.EnumerateFiles(sourcePath, "*.jpg");

        //    //load the first image
        //    Bitmap bitmap = (Bitmap)Image.FromFile(jpgFileList.First());

        //    //create a new AVI file
        //    AviManager aviManager = new AviManager(sourcePath + "\\tada.avi", false);

        //    //add a new video stream and one frame to the new file
        //    //set IsCompressed = false
        //    VideoStream aviStream = aviManager.AddVideoStream(false, 2, bitmap);

        //    jpgFileList.Skip(1).ToList().ForEach(file =>
        //    {
        //        bitmap = (Bitmap)Bitmap.FromFile(file);
        //        aviStream.AddFrame(bitmap);
        //        bitmap.Dispose();
        //    });

        //    aviManager.Close();
        //}

        public int Write(System.IO.MemoryStream Frame,System.Net.Sockets.Socket _Client)
        {
            BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();
            message.VarRoomNumber = User.CurrentChatViewNumber;
            message.VarType = 0;
           
            message.VarMessage.Add(ByteString.CopyFrom(Frame.ToArray()));

            return WritePacket(_Client, (int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);
        }

        public int Write(ConcurrentQueue<System.IO.MemoryStream> list, System.Net.Sockets.Socket _Client)
        {
            BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();
            message.VarRoomNumber = User.CurrentChatViewNumber;
            message.VarType = 0;

            foreach (var msg in list)
            {
                message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
            };

            return WritePacket(_Client, (int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);
        }

        public int WritePacket(System.Net.Sockets.Socket socket,int protocol, byte[] packet, int payloadsize)
        {
            if (socket == null || socket.Connected == false)
                return 0;

            int sendconut = 0;

            {
                Int32 PacketLength = sizeof(Int32) +
                    sizeof(Int16) +
                    sizeof(Int16) +
                    sizeof(Int32) +
                    sizeof(byte) +
                    sizeof(long) +
                   payloadsize;

                int mCompressFlag = 0;

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
                    sendconut = socket.Send(TempBuffer);
                }
                catch (SocketException e)
                {
                    // 10035 == WSAEWOULDBLOCK
                    if (!e.NativeErrorCode.Equals(10035))
                        Console.Write("Disconnected: error code :" + e.NativeErrorCode + "(" + e.Message + ")");


                    sendconut = 0;
                }

                TempBuffer = null;
            }

            packet = null;

            return sendconut;
        }

        public void Write(System.IO.MemoryStream imageStream)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.AppendLine();
            sb.AppendLine(this.Boundary);
            sb.AppendLine("Content-Type: image/jpeg");
            sb.AppendLine("Content-Length: " + imageStream.Length.ToString());
            sb.AppendLine();

            Write(sb.ToString());
            imageStream.WriteTo(this.Stream);
            Write("\r\n");

            this.Stream.Flush();
        }


        private void Write(byte[] data)
        {
            this.Stream.Write(data, 0, data.Length);
        }


        private void Write(string text)
        {
            byte[] data = BytesOf(text);
            this.Stream.Write(data, 0, data.Length);
        }


        private static byte[] BytesOf(string text)
        {
            return System.Text.Encoding.ASCII.GetBytes(text);
        }


        public string ReadRequest(int length)
        {

            byte[] data = new byte[length];
            int count = this.Stream.Read(data, 0, data.Length);

            if (count != 0)
                return System.Text.Encoding.ASCII.GetString(data, 0, count);

            return null;
        }


        public void Dispose()
        {

            try
            {

                if (this.Stream != null)
                    this.Stream.Dispose();

            }
            finally
            {
                this.Stream = null;
            }
        }


    }


}