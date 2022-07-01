using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Android;
using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.Graphics;
using Android.Media;
using Android.Support.Design.Widget;
using Android.Support.V4.App;
using Android.Support.V4.Content;
using Android.Views;
using Android.Widget;
using FullCameraApp;
using FullCameraApp.Droid;
using OMOK;
using OMOK.Droid;
using OMOK.Network;
using OMOK.Views;
using Plugin.CurrentActivity;
using rtaNetworking.Streaming;
using Xamarin.Forms.Platform.Android;

[assembly: Xamarin.Forms.ExportRenderer(typeof(OMOK.CameraPage), typeof(CameraPageRenderer))]
namespace FullCameraApp.Droid
{

    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
    {
        public ImageStreamingServer server = new ImageStreamingServer();

        public ConcurrentQueue<System.IO.MemoryStream> Frames = new ConcurrentQueue<System.IO.MemoryStream>();

        DateTime checktime = DateTime.Now;
        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
        {
            var paras = camera.GetParameters();
            var imageformat = paras.PreviewFormat;

            switch (imageformat)
            {
                case ImageFormatType.Nv16:
                case ImageFormatType.Nv21:
                case ImageFormatType.Yuy2:
                case ImageFormatType.Yv12:
                    {
                        //Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
                        //                               imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);

                        //System.IO.MemoryStream outStream = new System.IO.MemoryStream();

                        //img.CompressToJpeg(new Rect(0, 0, paras.PreviewSize.Width, paras.PreviewSize.Height), 30, outStream);

                        //Frames.Enqueue(outStream);

                        //if (checktime < DateTime.Now)
                        //{
                        //    NetProcess.SendRoomBITMAPMessage(Frames);
                        //    checktime = DateTime.Now.AddMilliseconds(50);
                        //    Frames.Clear();
                        //}

                        if (checktime < DateTime.Now)
                        {
                            Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
                                                           imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);

                            System.IO.MemoryStream outStream = new System.IO.MemoryStream();

                            img.CompressToJpeg(new Rect(0, 0, paras.PreviewSize.Width, paras.PreviewSize.Height), 30, outStream);

                            Frames.Enqueue(outStream);
                            
                            //서버쪽은 임시 주석
                            //server.ImagesSource.Enqueue(outStream);
                      
                            if (Frames.Count > 5)
                            {
                                NetProcess.SendRoomBITMAPMessage(Frames);

                                Frames.Clear();
                            }

                            checktime = DateTime.Now.AddMilliseconds(100);
                        }
                    }
                    break;
                case ImageFormatType.Jpeg:

                    Frames.Enqueue(new System.IO.MemoryStream(data));

                    if (checktime < DateTime.Now)
                    {
                        foreach (var frame in Frames)
                        {
                            server.ImagesSource.Enqueue(frame);
                        }

                        Frames.Clear();
                    }


                    break;
            }
        }
    }



    public class CameraPageRenderer : PageRenderer, TextureView.ISurfaceTextureListener
    {
        Activity CurrentContext => CrossCurrentActivity.Current.Activity;

        public CameraPageRenderer(Context context) : base(context)
        {
            _context = SynchronizationContext.Current;

            NetProcess.JpegStream.Clear();
            NetProcess.AudioStream.Clear();
        }

        RelativeLayout mainLayout;
        TextureView liveView;
        ImageView imageView;
        Button exitButton;
    
        Android.Hardware.Camera camera;

        AudioManagerM audiomgr = new AudioManagerM();

        Activity Activity => this.Context as Activity;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);
            SetupUserInterface();
            SetupEventHandlers();
        }

        int half_width;
        int half_height;

        void SetupUserInterface()
        {
            var metrics = Resources.DisplayMetrics;

            half_width = metrics.WidthPixels / 2;
            half_height = metrics.HeightPixels / 4;

            mainLayout = new RelativeLayout(Context);
            mainLayout.SetBackgroundColor(Color.Black);

            ///////////////////////////////////////////////////////////////////////////////
            liveView = new TextureView(Context);

            RelativeLayout.LayoutParams liveViewParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MatchParent,
                RelativeLayout.LayoutParams.MatchParent);

            liveViewParams.Height = half_height;
            liveViewParams.Width = half_width;
            liveView.LayoutParameters = liveViewParams;
            liveView.Rotation = 90;
            mainLayout.AddView(liveView);

            TextView textview1 = new TextView(Context);
            textview1.Text = User.myInfo.NickName;
            textview1.SetX(half_width / 2);
            textview1.SetY(half_height + 10);
            textview1.SetTextColor(Color.White);
            mainLayout.AddView(textview1);
            ///////////////////////////////////////////////////////////////////////////////




            ///////////////////////////////////////////////////////////////////////////////
            imageView = new ImageView(Context);

            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            imageViewParams.Height = half_height;
            imageViewParams.Width = half_width;

            imageView.Rotation = 270;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);
            ///////////////////////////////////////////////////////////////////////////////


            exitButton = new Button(Context);
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent,
            RelativeLayout.LayoutParams.WrapContent);
            ButtonParams.Height = 150;
            ButtonParams.Width  = 150;
            exitButton.LayoutParameters = ButtonParams;
            exitButton.Text = "EXIT";
            exitButton.Click += ButtonClicked;
            mainLayout.AddView(exitButton);
            void ButtonClicked(object sender, EventArgs args)
            {
                var page = Element as OMOK.CameraPage;

              

                StopCamera();
            }
            ///////////////////////////////////////////////////////////////////////////////

            AddView(mainLayout);
        }

     

        protected override void OnLayout(bool changed, int l, int t, int r, int b)
        {
            base.OnLayout(changed, l, t, r, b);
            if (!changed)
                return;
            
            var msw = MeasureSpec.MakeMeasureSpec(r - l, MeasureSpecMode.Exactly);
            var msh = MeasureSpec.MakeMeasureSpec(b - t, MeasureSpecMode.Exactly);
            mainLayout.Measure(msw, msh);
            mainLayout.Layout(0, 0, r - l, b - t);

            var metrics = Resources.DisplayMetrics;

      

            liveView.SetX(0);
            liveView.SetY(0);

            imageView.SetX(half_width);
            imageView.SetY(0);
       
            exitButton.SetX(half_width);
            exitButton.SetY(metrics.HeightPixels-200);

        }

        public void SetupEventHandlers()
        {
            liveView.SurfaceTextureListener = this;
        }

        public override bool OnKeyDown(Keycode keyCode, KeyEvent e)
        {
            if (keyCode == Keycode.Back)
            {
                var page = Element as OMOK.CameraPage;

              //  page.Close();
            }
            return base.OnKeyDown(keyCode, e);
        }

        public async Task<byte[]> TakePhoto()
        {
            camera.StopPreview();
            var ratio = ((decimal)Height) / Width;
            var image = Bitmap.CreateBitmap(liveView.Bitmap, 0, 0, liveView.Bitmap.Width, (int)(liveView.Bitmap.Width * ratio));
            byte[] imageBytes = null;
            using (var imageStream = new System.IO.MemoryStream())
            {
                await image.CompressAsync(Bitmap.CompressFormat.Jpeg, 50, imageStream);
                image.Recycle();
                imageBytes = imageStream.ToArray();
            }
            camera.StartPreview();
            return imageBytes;
        }

        private void StopCamera()
        {
            camera.SetPreviewCallback(null);

            camera.StopPreview();
            camera.Release();
            camera.Dispose();
            camera = null;
        }

        mPreviewCallback callbackcamera = new mPreviewCallback();

        private void StartCamera()
        {
            camera.SetPreviewCallback(callbackcamera);
            camera.StartPreview();

            callbackcamera.server.Start();
        }

        private void Mjpeg_FrameReady(object sender, FrameReadyEventArgs e)
        {
           imageView.SetImageBitmap(e.Bitmap);
        }

        #region TextureView.ISurfaceTextureListener implementations

        public void OnSurfaceTextureAvailable(SurfaceTexture surface, int width, int height)
        {
            {

                {
                    int cameraCount = Android.Hardware.Camera.NumberOfCameras;
                    int cameraId = 0;
                    Android.Hardware.Camera.CameraInfo cameraInfo = new Android.Hardware.Camera.CameraInfo();
                    for (int camIdx = 0; camIdx < cameraCount; camIdx++)
                    {
                        Android.Hardware.Camera.GetCameraInfo(camIdx, cameraInfo);
                        if (cameraInfo.Facing == Android.Hardware.CameraFacing.Front)
                        {
                            camera = Android.Hardware.Camera.Open(camIdx);

                            cameraId = camIdx;
                            break;
                        }
                    }

                    if (camera == null)
                        camera = Android.Hardware.Camera.Open();
                }


                var parameters = camera.GetParameters();
                var aspect = ((decimal)height) / ((decimal)width);

                // Find the preview aspect ratio that is closest to the surface aspect
                //var previewSize = parameters.SupportedPreviewSizes
                //                            .OrderBy(s => Math.Abs(s.Width / (decimal)s.Height - aspect))
                //                            .First();

                var previewSize = parameters.SupportedPreviewSizes[2];
                //     mainLayout.LayoutParameters.Height = previewSize.Height;
                //     mainLayout.LayoutParameters.Width = previewSize.Width;

                parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
                camera.SetParameters(parameters);
                camera.SetPreviewTexture(surface);
                StartCamera();


                //caemra page render
                Task.Run(() =>
                {
                    while (camera != null)
                    {
                        if (NetProcess.JpegStream.Count == 0)
                            continue;

                        MemoryStream ms;
                        if (NetProcess.JpegStream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                            _context.Post(delegate
                            {
                                var bitmap = BitmapFactory.DecodeByteArray(ms?.ToArray(), 0, ms.ToArray().Length);

                                imageView.SetImageBitmap(bitmap);

                            }, null);
                        }
                        Thread.Sleep(100);
                    }
                });


                //caemra page render
                Task.Run(() =>
                {
                    audiomgr?.record();
                });


                Task.Run(() =>
                {
                    while (audiomgr != null)
                    {
                        MemoryStream ms;
                        if (NetProcess.AudioStream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                      //      _context.Post(delegate
                     //       {
                                audiomgr?.play(ms.ToArray());

                        //    }, null);
                        }

                        Thread.Sleep(1);
                    }
                   
                });

            }

        }

        // used to marshal back to UI thread
        private SynchronizationContext _context;

   

        public bool OnSurfaceTextureDestroyed(Android.Graphics.SurfaceTexture surface)
        {
            StopCamera();
            audiomgr.Clear();
            return true;
        }

        public void OnSurfaceTextureSizeChanged(Android.Graphics.SurfaceTexture surface, int width, int height)
        {
        }

        public void OnSurfaceTextureUpdated(Android.Graphics.SurfaceTexture surface)
        {
        }
        #endregion
    }

 
}