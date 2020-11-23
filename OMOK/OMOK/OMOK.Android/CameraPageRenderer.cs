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
using Android.Support.V4.App;
using Android.Support.V4.Content;
using Android.Views;
using Android.Widget;
using FullCameraApp;
using FullCameraApp.Droid;
using OMOK.Network;
using OMOK.Views;
using Plugin.CurrentActivity;
using rtaNetworking.Streaming;
using Xamarin.Forms.Platform.Android;

[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraPage), typeof(CameraPageRenderer))]
namespace FullCameraApp.Droid
{

    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
    {
        public ImageStreamingServer server = new ImageStreamingServer();

        bool once = false;
        DateTime checktime = DateTime.Now;
        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
        {
  //          if (once == true)
 //               return;

            once = true;

            var paras = camera.GetParameters();
            var imageformat = paras.PreviewFormat;

            switch (imageformat)
            {
                case ImageFormatType.Nv16:
                case ImageFormatType.Nv21:
                case ImageFormatType.Yuy2:
                case ImageFormatType.Yv12:
                    {
                        if (checktime < DateTime.Now)
                        {
                            Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
                                    imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);

                            System.IO.MemoryStream outStream = new System.IO.MemoryStream();

                            bool didIt = img.CompressToJpeg(new Rect(0, 0, paras.PreviewSize.Width, paras.PreviewSize.Height), 30, outStream);

                            server.ImagesSource.Enqueue(outStream);
                            NetProcess.SendRoomMessage(outStream.ToArray());


                            checktime = DateTime.Now.AddMilliseconds(310);
                        }
                    }
                    break;
                case ImageFormatType.Jpeg:
                    server.ImagesSource.Enqueue(new System.IO.MemoryStream(data));
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
        }

        RelativeLayout mainLayout;
        TextureView liveView;
        ImageView imageView;
        ImageButton button;

        Android.Hardware.Camera camera;

        Activity Activity => this.Context as Activity;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);
            SetupUserInterface();
            SetupEventHandlers();
        }

        void ButtonClicked(object sender, EventArgs args)
        {
            test();
        }

        void SetupUserInterface()
        {
            mainLayout = new RelativeLayout(Context);
             //RelativeLayout.LayoutParams mainLayoutParams = new RelativeLayout.LayoutParams(
            //	RelativeLayout.LayoutParams.MatchParent,
            //	RelativeLayout.LayoutParams.MatchParent);
            //mainLayout.LayoutParameters = mainLayoutParams;

            liveView = new TextureView(Context);
            imageView = new ImageView(Context);
            button = new ImageButton(Context);

            RelativeLayout.LayoutParams ViewParams = new RelativeLayout.LayoutParams(
                400,
                400);
            liveView.LayoutParameters = ViewParams;

            button.LayoutParameters = ViewParams;

            RelativeLayout.LayoutParams imageParams = new RelativeLayout.LayoutParams(
              800,
              800);
            imageView.LayoutParameters = imageParams;

            mainLayout.AddView(liveView);
            mainLayout.AddView(imageView);
            mainLayout.AddView(button);

            AddView(mainLayout);

            button.Click += ButtonClicked;
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
        }

        public void SetupEventHandlers()
        {
            liveView.SurfaceTextureListener = this;
        }

        public override bool OnKeyDown(Keycode keyCode, KeyEvent e)
        {
            if (keyCode == Keycode.Back)
            {
                return false;
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
            camera.StopPreview();
            camera.Release();
        }

        mPreviewCallback callbackcamera = new mPreviewCallback();

        private void StartCamera()
        {
            camera.SetDisplayOrientation(90);
            camera.SetPreviewCallback(callbackcamera);
            camera.StartPreview();

            callbackcamera.server.Start();
        }

        private void test()
        {
            var mjpeg = new MJPEGDecoder();
            mjpeg.FrameReady += Mjpeg_FrameReady;
            mjpeg.ParseStream(new Uri("http://192.168.232.2:8080"), "JR", "123");
        }
        private void Mjpeg_FrameReady(object sender, FrameReadyEventArgs e)
        {
            button.SetImageBitmap(e.Bitmap);
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

                    if(camera == null)
                        camera = Android.Hardware.Camera.Open();
                }
               

                var parameters = camera.GetParameters();
                var aspect = ((decimal)height) / ((decimal)width);

                // Find the preview aspect ratio that is closest to the surface aspect
                //var previewSize = parameters.SupportedPreviewSizes
                //                            .OrderBy(s => Math.Abs(s.Width / (decimal)s.Height - aspect))
                //                            .First();

                var previewSize = parameters.SupportedPreviewSizes[0];

                System.Diagnostics.Debug.WriteLine($"Preview sizes: {parameters.SupportedPreviewSizes.Count}");

                mainLayout.LayoutParameters.Height = previewSize.Height;
                mainLayout.LayoutParameters.Width = previewSize.Width;

                parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
                camera.SetParameters(parameters);
                camera.SetPreviewTexture(surface);
                StartCamera();

                System.Timers.Timer timer = new System.Timers.Timer();
                timer.Interval = 10;
                timer.Elapsed += OnTimedEvent;
                timer.Enabled = true;

                void OnTimedEvent(object sender, System.Timers.ElapsedEventArgs e)
                {
                     Activity.RunOnUiThread(() =>
                    {
                        lock(this)
                        {
                            if(NetProcess.JpegStream.Count == 0)
                                return;



                            MemoryStream ms;
                            if(NetProcess.JpegStream.TryDequeue(out ms) == true)
                            {
                                _context.Post(delegate
                                {
                                    var bitmap = BitmapFactory.DecodeByteArray(ms.ToArray(), 0, ms.ToArray().Length);

                                    imageView.SetImageBitmap(bitmap);

                                }, null);
                            }


                            MemoryStream ms2;
                            if (callbackcamera.server.ImagesSource.TryDequeue(out ms2) == true)
                            {
                                _context.Post(delegate
                                {
                                    var bitmap = BitmapFactory.DecodeByteArray(ms2.ToArray(), 0, ms2.ToArray().Length);

                                    button.SetImageBitmap(bitmap);

                                }, null);
                            }


                            var result = ms.ToArray().SequenceEqual(ms2.ToArray());

                            if (result == false)
                            {
                                return;
                            }
                        }                     
                    });
                }
            }
        }

        // used to marshal back to UI thread
        private SynchronizationContext _context;

        // event to get the buffer above handed to you
        public event EventHandler<FrameReadyEventArgs> FrameReady;


        public bool OnSurfaceTextureDestroyed(Android.Graphics.SurfaceTexture surface)
        {
            StopCamera();
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