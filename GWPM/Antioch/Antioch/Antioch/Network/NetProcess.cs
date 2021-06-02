using System;
using System.IO;
using Xamarin.Essentials;
using System.Text;
using System.Net;
using System.Threading;
using System.Collections.Concurrent;
using Xamarin.Forms;

namespace Antioch
{
    public static class NetProcess
    {
        static public Client client = new Client();

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

        private static DateTime time = new DateTime();
   
        private static DateTime notice_time = new DateTime();
   
        static public void start()
        {
            string ip = "211.212.37.238";


            //연결중이면 안한다. 
            if (client.socket == null || client.socket.Connected == false)
            {
                if ((DateTime.Now - time).TotalSeconds > 5)
                {
                    time = DateTime.Now;
                    //if(User.Token != null && User.Token != "")
                    //    client.StartClient(ip, 20000);
                }
            }

            if ((DateTime.Now - notice_time).TotalSeconds > 30)
            {
                if (client.socket != null && client.socket.Connected == true)
                {
                    notice_time = DateTime.Now;
                }
            }
        }

        public static ConcurrentQueue<MemoryStream> JpegStream = new ConcurrentQueue<MemoryStream>();
        public static ConcurrentQueue<MemoryStream> AudioStream = new ConcurrentQueue<MemoryStream>();


        static public void Loop()
        {
            
            if (client.socket == null || client.socket.Connected == false)
                return;
         
            CompletePacket data;
            if (client.PacketQueue.TryDequeue(out data) == true)
            {
                try
                {
                    switch (data.Protocol)
                    {
                        case (int)0:
                            {
                               
                            }
                            break;
                    }
                }
                catch (Exception ex)
                {
                    Console.Write(ex.ToString());
                }
            }
        }

        static public void SendVersion()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            //VERSION_REQ person = new VERSION_REQ
            //{
            //};
            //using (MemoryStream stream = new MemoryStream())
            //{
            //    person.WriteTo(stream);

            //    client.WritePacket((int)PROTOCOL.IdPktVersionReq, stream.ToArray(), stream.ToArray().Length);
            //}
        }

    }
}

