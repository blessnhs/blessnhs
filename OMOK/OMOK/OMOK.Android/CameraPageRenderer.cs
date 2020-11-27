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
using Android.Support.Design.Widget;
using Android.Support.V4.App;
using Android.Support.V4.Content;
using Android.Views;
using Android.Widget;
using FullCameraApp;
using FullCameraApp.Droid;
using OMOK;
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
                        if (checktime < DateTime.Now)
                        {
                            Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
                                                           imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);

                            System.IO.MemoryStream outStream = new System.IO.MemoryStream();

                            img.CompressToJpeg(new Rect(0, 0, paras.PreviewSize.Width, paras.PreviewSize.Height), 30, outStream);

                            Frames.Enqueue(outStream);
                            
                            foreach (var frame in Frames)
                            {

                                server.ImagesSource.Enqueue(outStream);
                                NetProcess.SendRoomMessage(outStream.ToArray());
                            }

                            checktime = DateTime.Now.AddMilliseconds(250);

                            Frames.Clear();
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

        //    Activity.SetContentView(OMOK.Droid.Resource.Layout.layout_camera);

         
        }

        RelativeLayout mainLayout;
        TextureView liveView;
        ImageView imageView;
        Button ExitButton;

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
            var page = (Element as CameraPage);

            page.Close();
        }

        void SetupUserInterface()
        {
            mainLayout = new RelativeLayout(Context);
            mainLayout.SetBackgroundColor(Color.Black);

            liveView = new TextureView(Context);

            RelativeLayout.LayoutParams liveViewParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MatchParent,
                RelativeLayout.LayoutParams.MatchParent);

            liveViewParams.Height = 450;
            liveViewParams.Width = 450;
            liveView.LayoutParameters = liveViewParams;
            liveView.Rotation = 90;
            mainLayout.AddView(liveView);

            TextView textview1 = new TextView(Context);
            textview1.Text = User.myInfo.NickName;
            textview1.SetX(450/2);
            textview1.SetY(460);
            textview1.SetTextColor(Color.White);
            mainLayout.AddView(textview1);




            imageView = new ImageView(Context);
       
            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.MatchParent,
             RelativeLayout.LayoutParams.MatchParent);
            imageViewParams.Height = 450;
            imageViewParams.Width = 450;

            imageView.Rotation = 270;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);

            ExitButton = new Button(Context);
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.MatchParent,
            RelativeLayout.LayoutParams.MatchParent);
            ButtonParams.Height = 50;
            ButtonParams.Width = 150;
            ExitButton.LayoutParameters = ButtonParams;

            AddView(mainLayout);

            //liveView = Activity.FindViewById<TextureView>(OMOK.Droid.Resource.Id.textureView1);
            //imageView = Activity.FindViewById<ImageView>(OMOK.Droid.Resource.Id.imageView1);
            //var button = Activity.FindViewById<Button>(OMOK.Droid.Resource.Id.button1);
            //button.Click += ButtonClicked;


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

            liveView.SetX(0);
            liveView.SetY(0);

            imageView.SetX(mainLayout.Width - 450);
            imageView.SetY(0);

            ExitButton.SetX((r - l)/2);
            ExitButton.SetY((b - t) - 100);

        }

        public void SetupEventHandlers()
        {
            liveView.SurfaceTextureListener = this;
        }

        public override bool OnKeyDown(Keycode keyCode, KeyEvent e)
        {
            if (keyCode == Keycode.Back)
            {               
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

        private void test()
        {
            var mjpeg = new MJPEGDecoder();
            mjpeg.FrameReady += Mjpeg_FrameReady;
            mjpeg.ParseStream(new Uri("http://192.168.232.2:8080"), "JR", "123");
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

                    if(camera == null)
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


                Task.Run(() =>
                {
                    while (true)
                    {
                        if (NetProcess.JpegStream.Count == 0)
                            continue;

                        Thread.Sleep(5);

                        MemoryStream ms;
                        if(NetProcess.JpegStream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                            _context.Post(delegate
                            {
                                var bitmap = BitmapFactory.DecodeByteArray(ms?.ToArray(), 0, ms.ToArray().Length);

                                imageView.SetImageBitmap(bitmap);

                            }, null);
                        }


                        MemoryStream ms2;
                        if(callbackcamera.server.ImagesSource.TryDequeue(out ms2) == true)
                        {
                            if (ms2 == null)
                                continue;

                            _context.Post(delegate
                            {
                                var bitmap = BitmapFactory.DecodeByteArray(ms2.ToArray(), 0, ms2.ToArray().Length);


                            }, null);
                        }
                    }
                });

               
            }
        }

        // used to marshal back to UI thread
        private SynchronizationContext _context;

   

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