using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.Graphics;
using Android.Views;
using Android.Widget;
using FullCameraApp.Droid;
using Antioch;
using Antioch.Droid;
using Plugin.CurrentActivity;
using rtaNetworking.Streaming;
using Xamarin.Forms.Platform.Android;
using Antioch.View;
using Rg.Plugins.Popup.Services;
using Xamarin.Essentials;

[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraPage), typeof(CameraPageRenderer))]
namespace FullCameraApp.Droid
{
    //카메라 프레임 정보 구조체
    public class CameraFrameInfo
    {
        public byte[] data;
        public ImageFormatType type;
        public int width;
        public int height;
    };
        

    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
    {
        public CameraPageRenderer renderer;

        DateTime checktime = DateTime.Now;
        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
        {
            try
            {
                if (DateTime.Now > checktime)
                {
                    var paras = camera.GetParameters();
                    var imageformat = paras.PreviewFormat;

                    //프레임 마다정보를 저장
                    CameraFrameInfo info = new CameraFrameInfo();
                    info.data = new byte[data.Length];
                    Buffer.BlockCopy(data, 0, info.data, 0, data.Length);
                    info.width = paras.PreviewSize.Width;
                    info.height = paras.PreviewSize.Height;
                    info.type = imageformat;
                    renderer.BytesQueue.Enqueue(info);

                    checktime = DateTime.Now.AddMilliseconds(100);
                };

            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine(ex.Message);
            }
        }
    }



    public class CameraPageRenderer : PageRenderer, TextureView.ISurfaceTextureListener
    {
        Activity CurrentContext => CrossCurrentActivity.Current.Activity;

        public ImageStreamingServer server = new ImageStreamingServer();
        public ConcurrentQueue<CameraFrameInfo> BytesQueue = new ConcurrentQueue<CameraFrameInfo>();
        public ConcurrentQueue<System.IO.MemoryStream> Frames = new ConcurrentQueue<System.IO.MemoryStream>();

        public CameraPageRenderer(Context context) : base(context)
        {
            _context = SynchronizationContext.Current;

            NetProcess.JpegStream.Clear();
            NetProcess.AudioStream.Clear();
        }

        RelativeLayout mainLayout;
        
        //내 정보 이미지 카메라
        TextureView liveView;
       
        // 상대방정보들
        Dictionary<int,ImageView> imageViewDic = new Dictionary<int, ImageView>();

        //퇴장버튼
        Button exitButton;

        //카메라 스위치
        Button switchButton;

        Android.Hardware.Camera camera;

        AudioManagerM audiomgr = new AudioManagerM();

        Activity Activity => this.Context as Activity;

        CameraPage page;

        bool isDestroy = false;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);

            page = (CameraPage)e.NewElement;

            SetupUserInterface();
            SetupEventHandlers();
        }

        int half_width;
        int half_height;


        void AddImageView(int pos)
        {
            ///////////////////////////////////////////////////////////////////////////////
            var imageView = new ImageView(Context);

            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            imageViewParams.Height = half_height;
            imageViewParams.Width = half_width;

            imageView.Rotation = 0;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);
            imageViewDic.Add(pos, imageView);
            ///////////////////////////////////////////////////////////////////////////////
        }

        void SetupUserInterface()
        {
            var metrics = Resources.DisplayMetrics;

            half_width = metrics.WidthPixels / 2;
            half_height = metrics.HeightPixels / 4;


            mainLayout = new RelativeLayout(Context);
            mainLayout.SetBackgroundColor(Color.Black);

            /////////////////////////////////////////////////////////////////////////////////
            liveView = new TextureView(Context);

            RelativeLayout.LayoutParams liveViewParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MatchParent,
                RelativeLayout.LayoutParams.MatchParent);

            liveViewParams.Height = half_height;
            liveViewParams.Width = half_width;
            liveView.LayoutParameters = liveViewParams;
            liveView.Rotation = 90;
            mainLayout.AddView(liveView);

            //TextView textview1 = new TextView(Context);
            //textview1.Text = "";
            //textview1.SetX(half_width / 2);
            //textview1.SetY(half_height + 10);
            //textview1.SetTextColor(Color.White);
            //mainLayout.AddView(textview1);
            ///////////////////////////////////////////////////////////////////////////////

            ///////////////////////////////////////////////////////////////////////////////
            AddImageView(0);
            AddImageView(1);
            AddImageView(2);
            AddImageView(3);
            AddImageView(4);
            AddImageView(5);
            ///////////////////////////////////////////////////////////////////////////////
            exitButton = new Button(Context);
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent,
            RelativeLayout.LayoutParams.WrapContent);
            ButtonParams.Height = 150;
            ButtonParams.Width  = 150;
            exitButton.LayoutParameters = ButtonParams;
            exitButton.Text = "EXIT";
            exitButton.Click += async (s, e) =>
            {
                var page = Element as CameraPage;

                PopupNavigation.Instance.PopAsync();
            };
            mainLayout.AddView(exitButton);
            ////////////////////////////////////////////////////////////DrawLayout///////////////////
            switchButton = new Button(Context);
            switchButton.LayoutParameters = ButtonParams;
            switchButton.Text = "Switch";
            switchButton.Click += async (s, e) =>
            {
                StopCamera();

                if (currentFacing == Android.Hardware.CameraFacing.Front)
                    currentFacing = Android.Hardware.CameraFacing.Back;
                else
                    currentFacing = Android.Hardware.CameraFacing.Front;

                {
                    int cameraCount = Android.Hardware.Camera.NumberOfCameras;
                    int cameraId = 0;
                    Android.Hardware.Camera.CameraInfo cameraInfo = new Android.Hardware.Camera.CameraInfo();
                    for (int camIdx = 0; camIdx < cameraCount; camIdx++)
                    {
                        Android.Hardware.Camera.GetCameraInfo(camIdx, cameraInfo);
                        if (cameraInfo.Facing == currentFacing)
                        {
                            camera = Android.Hardware.Camera.Open(camIdx);
                            cameraId = camIdx;
                            break;
                        }
                    }

                    if (camera == null)
                        camera = Android.Hardware.Camera.Open();

                    var parameters = camera.GetParameters();

                    // Find the preview aspect ratio that is closest to the surface aspect
                    //var previewSize = parameters.SupportedPreviewSizes
                    //                            .OrderBy(s => Math.Abs(s.Width / (decimal)s.Height - aspect))
                    //                            .First();

                    var previewSize = parameters.SupportedPreviewSizes[2];
                    //     mainLayout.LayoutParameters.Height = previewSize.Height;
                    //     mainLayout.LayoutParameters.Width = previewSize.Width;

                    parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
                    camera.SetParameters(parameters);
                    camera.SetPreviewTexture(_surface);
                    StartCamera();
                }
            };
            mainLayout.AddView(switchButton);

            ////////////////////////////////////////////////////////////DrawLayout///////////////////

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


            ////내화면
            //liveView.SetX(0);
            //liveView.SetY(0);

            int posx = 1, posy = 0;
            foreach(var image in imageViewDic)
            {
                image.Value.SetX(posx % 2  == 0 ? 0 : half_width);
                image.Value.SetY((half_height * posy));

                posx++;
                posy = posx / 2;
            }       
       
            exitButton.SetX(half_width);
            exitButton.SetY(metrics.HeightPixels-200);

            switchButton.SetX(half_width + 250);
            switchButton.SetY(metrics.HeightPixels - 200);
        }

        public void SetupEventHandlers()
        {
            liveView.SurfaceTextureListener = this;
        }

        public override bool OnKeyDown(Keycode keyCode, KeyEvent e)
        {
            if (keyCode == Keycode.Back)
            {
                //var page = Element as CameraPage;

                //page.Close();
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
            camera = null;
        }

        private void DrawFrame()
        {
            //caemra page render
            Task.Run(() =>
            {
                NetProcess.JpegStream.Clear();

                DateTime chk = DateTime.Now;
                while (isDestroy == false)
                {
                    try
                    {

                        if (NetProcess.JpegStream.Count >= 3000)
                        {
                            NetProcess.JpegStream.Clear();
                            continue;
                        }
                         
                        if (NetProcess.JpegStream.Count == 0)
                            continue;

                        //if (NetProcess.JpegStream.Count > 100)
                        //{
                        //    NetProcess.JpegStream.Clear();
                        //    continue;
                        //}

                        if (chk < DateTime.Now)
                        {
                            exitButton.Text = "exit";// ((callbackcamera.total_bytes_sent / callbackcamera.count_sent) / 1024).ToString() + "k";

                            chk = DateTime.Now.AddSeconds(3);
                        }

                        StreamWrapper ms;
                        while (NetProcess.JpegStream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                            var bitmap = BitmapFactory.DecodeByteArray(ms?.stream.ToArray(), 0, ms.stream.ToArray().Length);

                            ImageView imageView;
                            if (imageViewDic.TryGetValue(ms.pos, out imageView) == true)
                                imageView.SetImageBitmap(bitmap);
                            else
                            {
                                AddImageView(ms.pos);

                                if (imageViewDic.TryGetValue(ms.pos, out imageView) == true)
                                    imageView.SetImageBitmap(bitmap);
                            }


                            Thread.Sleep(50);
                        }

                    }
                    catch (Exception e)
                    {

                    }
                }
            });

        }

        private void ProcCameraFrame()
        {
            //caemra page render
            Task.Run(() =>
            {
                while (true)
                {
                    CameraFrameInfo frm;
                    if(BytesQueue.TryDequeue(out frm) == true)
                    {
                        switch (frm.type)
                        {
                            case ImageFormatType.Nv16:
                            case ImageFormatType.Nv21:
                            case ImageFormatType.Yuy2:
                            case ImageFormatType.Yv12:
                                {
                                    //  if (checktime < DateTime.Now)
                                    {

                                        YuvImage img = new YuvImage(frm.data, frm.type, frm.width, frm.height, null);

                                        System.IO.MemoryStream outStream = new System.IO.MemoryStream();

                                        if (img.CompressToJpeg(new Rect(0, 0, frm.width, frm.height), 80, outStream) == false)
                                            return;

                                        var frameToStream = outStream.ToArray();
                                        var bitmap = BitmapFactory.DecodeByteArray(frameToStream, 0, frameToStream.Length);
                                        if (bitmap == null)
                                            return;

                                        var sbitmap = Bitmap.CreateScaledBitmap(bitmap, 320, 240, true);

                                        var mat = new Matrix();

                                        if (currentFacing == Android.Hardware.CameraFacing.Front)
                                            mat.PostRotate(-90);
                                        else
                                            mat.PostRotate(90);

                                        var rbitmap = Bitmap.CreateBitmap(sbitmap, 0, 0, sbitmap.Width, sbitmap.Height, mat, true);
                                        if (rbitmap == null)
                                            return;

                                        var soutStream = new System.IO.MemoryStream();
                                        if (rbitmap.Compress(Bitmap.CompressFormat.Jpeg, 80, soutStream) == false)
                                            return;

                                        Frames.Enqueue(soutStream);


                                        //서버쪽은 임시 주석
                                        if (server.ImagesSource.Count > 1000)
                                            server.ImagesSource.Clear();
                                        if (server._Clients.Count > 0)
                                            server.ImagesSource.Enqueue(outStream);

                                        if (Frames.Count > 0)
                                        {
                                            NetProcess.SendRoomBITMAPMessage(Frames);

                                            Frames.Clear();
                                        }
                                    }
                                }
                                break;
                            case ImageFormatType.Jpeg:

                                Frames.Enqueue(new System.IO.MemoryStream(frm.data));

                                if (server._Clients.Count > 0)
                                {
                                    foreach (var frame in Frames)
                                    {
                                        server.ImagesSource.Enqueue(frame);
                                    }
                                }

                                Frames.Clear();
                                break;
                        }
                    }
                }

            });
        }

        private void StartCamera()
        {
            mPreviewCallback callbackcamera = new mPreviewCallback();

            camera.SetPreviewCallback(callbackcamera);
            camera.StartPreview();

            server.Start(1801);

            callbackcamera.renderer = this;

            ProcCameraFrame();

            DrawFrame();
        }

        private void Mjpeg_FrameReady(object sender, FrameReadyEventArgs e)
        {
       //    imageView.SetImageBitmap(e.Bitmap);

       }

        #region TextureView.ISurfaceTextureListener implementations

        public Android.Hardware.CameraFacing currentFacing;
        SurfaceTexture _surface;
        public void OnSurfaceTextureAvailable(SurfaceTexture surface, int width, int height)
        {
            {
                _surface = surface;
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

                            currentFacing = Android.Hardware.CameraFacing.Front;
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
                    audiomgr?.record();
                });


                Task.Run(() =>
                {
                    while (isDestroy == false)
                    {
                        StreamWrapper ms;
                        if (NetProcess.AudioStream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                      //      _context.Post(delegate
                     //       {
                                audiomgr?.play(ms.stream.ToArray());

                        //    }, null);
                        }

                        ms = null;

                        Thread.Sleep(10);
                    }
                   
                });


            }

        }

        // used to marshal back to UI thread
        private SynchronizationContext _context;

   

        public bool OnSurfaceTextureDestroyed(Android.Graphics.SurfaceTexture surface)
        {
            isDestroy = true;

            Thread.Sleep(1000);

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