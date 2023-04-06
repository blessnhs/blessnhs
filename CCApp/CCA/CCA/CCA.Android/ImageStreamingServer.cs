
// -------------------------------------------------
// Developed By : Ragheed Al-Tayeb
// e-Mail       : ragheedemail@gmail.com
// Date         : April 2012
// -------------------------------------------------
using CCA.Page;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Concurrent;
using System.IO;
using System.Threading;
using Xamarin.Essentials;

namespace rtaNetworking.Streaming
{


    /// <summary>
    /// Provides a streaming server that can be used to stream any images source
    /// to any client.
    /// </summary>
    public class ImageStreamingServer : System.IDisposable
    {

        public System.Collections.Generic.List<System.Net.Sockets.Socket> _Clients;
        private System.Threading.Thread _Thread;


        public ImageStreamingServer()
        //: this(Screen.Snapshots(600, 450, true))

        {
            _Clients = new System.Collections.Generic.List<System.Net.Sockets.Socket>();
            _Thread = null;
            this.Interval = 50;
        }


        public ImageStreamingServer(ConcurrentQueue<System.IO.MemoryStream> imagesSource)
        {

            _Clients = new System.Collections.Generic.List<System.Net.Sockets.Socket>();
            _Thread = null;

            this.ImagesSource = imagesSource;
            this.Interval = 10;

        }


        /// <summary>
        /// Gets or sets the source of images that will be streamed to the 
        /// any connected client.
        /// </summary>
        public ConcurrentQueue<System.IO.MemoryStream> ImagesSource = new ConcurrentQueue<System.IO.MemoryStream>();

        /// <summary>
        /// Gets or sets the interval in milliseconds (or the delay time) between 
        /// the each image and the other of the stream (the default is . 
        /// </summary>
        public int Interval { get; set; }

        /// <summary>
        /// Gets a collection of client sockets.
        /// </summary>
        public System.Collections.Generic.IEnumerable<System.Net.Sockets.Socket> Clients { get { return _Clients; } }

        /// <summary>
        /// Returns the status of the server. True means the server is currently 
        /// running and ready to serve any client requests.
        /// </summary>
        public bool IsRunning { get { return (_Thread != null && _Thread.IsAlive); } }


        /// <summary>
        /// Starts the server to accepts any new connections on the specified port.
        /// </summary>
        /// <param name="port"></param>
        public void Start(int port)
        {
            try
            {
                lock (this)
                {
                    _Thread = new System.Threading.Thread(new System.Threading.ParameterizedThreadStart(ServerThread));
                    _Thread.IsBackground = true;
                    _Thread.Start(port);
                }
            }
            catch(Exception e)
            {
                e.Message.ToString();
            }

        }

        public void Stop()
        {

            if (this.IsRunning)
            {
                try
                {
                    _Thread?.Join(300);
                    _Thread?.Abort();
                }
                finally
                {

                    lock (_Clients)
                    {

                        foreach (var s in _Clients)
                        {
                            try
                            {
                                s.Close();
                            }
                            catch { }
                        }
                        _Clients.Clear();

                    }

                    _Thread = null;
                }
            }

            ImagesSource.Clear();
        }


        /// <summary>
        /// This the main thread of the server that serves all the new 
        /// connections from clients.
        /// </summary>
        /// <param name="state"></param>
        private void ServerThread(object state)
        {
            try
            {
                System.Net.Sockets.Socket Server = new System.Net.Sockets.Socket(
                    System.Net.Sockets.AddressFamily.InterNetwork, System.Net.Sockets.SocketType.Stream, System.Net.Sockets.ProtocolType.IP);

                Server.Bind(new System.Net.IPEndPoint(System.Net.IPAddress.Any, (int)state));
                Server.Listen(10);

                System.Diagnostics.Debug.WriteLine(string.Format("Server started on port {0}.", state));

                foreach (System.Net.Sockets.Socket client in Server.IncommingConnectoins())
                {
                    System.Threading.ThreadPool.QueueUserWorkItem(new System.Threading.WaitCallback(ClientThread), client);


                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        if (PopupNavigation.Instance.PopupStack.Count == 0)
                        {
                            PopupNavigation.Instance.PushAsync(new CameraPage());
                        }
                        else
                        {
                            if (PopupNavigation.Instance.PopupStack[0].GetType() != typeof(CameraPage))
                            {
                                PopupNavigation.Instance.PopAsync();

                                PopupNavigation.Instance.PushAsync(new CameraPage());
                            }
                        }
                    });

                } // Next client 

            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine(ex.Message);
            }

            this.Stop();
        }


        /// <summary>
        /// Each client connection will be served by this thread.
        /// </summary>
        /// <param name="client"></param>
        private void ClientThread(object client)
        {
            System.Net.Sockets.Socket socket = (System.Net.Sockets.Socket)client;
            System.Diagnostics.Debug.WriteLine(string.Format("New client from {0}", socket.RemoteEndPoint.ToString()));


            lock (_Clients)
                _Clients.Add(socket);

            try
            {
                using (MjpegWriter wr = new MjpegWriter(new System.Net.Sockets.NetworkStream(socket, true)))
                {

                    // Writes the response header to the client.

                    wr.WriteHeader();


                    while (socket.Connected == true)
                    {
                        MemoryStream out_s;
                        while (ImagesSource.TryDequeue(out out_s) == true)
                        {

                            if (this.Interval > 0)
                                System.Threading.Thread.Sleep(this.Interval);


                            int count = wr.Write(out_s, socket);

                            out_s.Dispose();
                            out_s = null;

                            if (socket.Connected == false || count == 0)
                            {
                                ImagesSource.Clear();
                                socket.Close();
                                break;
                            }
                        }
                      
                    }
                }

            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine(ex.Message);
            }
            finally
            {
                lock (_Clients)
                    _Clients.Remove(socket);
            }
        }


        public void Dispose()
        {
            this.Stop();
        }


    }


    static class SocketExtensions
    {

        public static System.Collections.Generic.IEnumerable<System.Net.Sockets.Socket> IncommingConnectoins(this System.Net.Sockets.Socket server)
        {
            while (true)
                yield return server.Accept();
        }

    }
}
