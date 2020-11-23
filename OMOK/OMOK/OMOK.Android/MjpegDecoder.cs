using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using Android.App;
using Android.Content;
using Android.Graphics;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace FullCameraApp.Droid
{
    public class FrameReadyEventArgs : EventArgs
    {
        public byte[] FrameBuffer;
        public Bitmap Bitmap;
    }

    public class MJPEGDecoder
    {
        public Bitmap bitmap { get; set; }

        // magic 2 byte header for JPEG images
        private byte[] JpegHeader = new byte[] { 0xff, 0xd8 };
        // you can replace boundaryBytes with JpegEnd
        //private byte[] JpegEnd = new byte[] { 0xff, 0xd9 }; 

        // pull down 1024 bytes at a time
        private const int ChunkSize = 1024;

        // used to cancel reading the stream
        private bool _streamActive;

        // current encoded JPEG image
        public byte[] CurrentFrame { get; private set; }

        // used to marshal back to UI thread
        private SynchronizationContext _context;

        // event to get the buffer above handed to you
        public event EventHandler<FrameReadyEventArgs> FrameReady;

        public MJPEGDecoder()
        {
            _context = SynchronizationContext.Current;
        }

        public void ParseStream(Uri uri)
        {
            ParseStream(uri, null, null);
        }

        public void ParseStream(Uri uri, string username, string password)
        {
            HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create(uri);
            //if (!string.IsNullOrEmpty(username) || !string.IsNullOrEmpty(password))
            //    request.Credentials = new NetworkCredential(username, password);

            // asynchronously get a response
            request.BeginGetResponse(OnGetResponse, request);
        }

        public void StopStream()
        {
            _streamActive = false;
        }

        private void OnGetResponse(IAsyncResult asyncResult)
        {
            HttpWebResponse resp;
            byte[] buff;
            byte[] imageBuffer = new byte[1024 * 1024];
            Stream s;

            // get the response
            HttpWebRequest req = (HttpWebRequest)asyncResult.AsyncState;
            resp = (HttpWebResponse)req.EndGetResponse(asyncResult);

            // find our magic boundary value
            string contentType = resp.Headers["Content-Type"];
            if (!string.IsNullOrEmpty(contentType) && !contentType.Contains("="))
                throw new Exception("Invalid content-type header.  The camera is likely not returning a proper MJPEG stream.");
            string boundary = resp.Headers["Content-Type"].Split('=')[1].Replace("\"", "");
            byte[] boundaryBytes = Encoding.UTF8.GetBytes(boundary.StartsWith("--") ? boundary : "--" + boundary);

            s = resp.GetResponseStream();
            BinaryReader br = new BinaryReader(s);

            _streamActive = true;

            buff = br.ReadBytes(ChunkSize);

            while (_streamActive)
            {
                int size;

                // find the JPEG header
                int imageStart = GetArrayPosition(buff, JpegHeader);

                if (imageStart != -1)
                {
                    // copy the start of the JPEG image to the imageBuffer
                    size = buff.Length - imageStart;
                    Array.Copy(buff, imageStart, imageBuffer, 0, size);

                    while (true)
                    {
                        buff = br.ReadBytes(ChunkSize);

                        // find the boundary text
                        int imageEnd = GetArrayPosition(buff, boundaryBytes);
                        if (imageEnd != -1)
                        {
                            // copy the remainder of the JPEG to the imageBuffer
                            Array.Copy(buff, 0, imageBuffer, size, imageEnd);
                            size += imageEnd;

                            // create a single JPEG frame
                            CurrentFrame = new byte[size];
                            Array.Copy(imageBuffer, 0, CurrentFrame, 0, size);

                            ProcessFrame(CurrentFrame);

                            // copy the leftover data to the start
                            Array.Copy(buff, imageEnd, buff, 0, buff.Length - imageEnd);

                            // fill the remainder of the buffer with new data and start over
                            byte[] temp = br.ReadBytes(imageEnd);

                            Array.Copy(temp, 0, buff, buff.Length - imageEnd, temp.Length);
                            break;
                        }

                        // copy all of the data to the imageBuffer
                        Array.Copy(buff, 0, imageBuffer, size, buff.Length);
                        size += buff.Length;
                    }
                }
            }
            resp.Close();
        }

        private void ProcessFrame(byte[] frameBuffer)
        {
            _context.Post(delegate
            {
                // create a simple GDI+ happy Bitmap
                //bitmap = new Bitmap(new MemoryStream(frameBuffer));

                bitmap = BitmapFactory.DecodeByteArray(frameBuffer, 0, frameBuffer.Length);

                // tell whoever's listening that we have a frame to draw
                FrameReady?.Invoke(this, new FrameReadyEventArgs { FrameBuffer = CurrentFrame, Bitmap = bitmap });
            }, null);
        }

        private int GetArrayPosition(byte[] mbyte, byte[] content)
        {
            int position = -1;

            for (int i = 0; i <= mbyte.Length - content.Length; i++)
            {
                if (mbyte[i] == content[0])
                {
                    bool isRight = true;
                    for (int j = 1; j < content.Length; j++)
                    {
                        if (mbyte[i + j] != content[j])
                        {
                            isRight = false;
                            break;
                        }
                    }
                    if (isRight) return i;
                }
            }

            return position;
        }

    }
}