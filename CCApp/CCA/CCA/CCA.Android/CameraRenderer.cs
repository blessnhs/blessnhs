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
using Xamarin.Forms.Platform.Android;
using Rg.Plugins.Popup.Services;
using Xamarin.Essentials;
using CCA.Page;
using CCA;
using rtaNetworking.Streaming;
using CCA.Droid;
using DependencyHelper;
using DependencyHelper.Droid;
using Android.Content.PM;
using System.Linq;

[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraPage), typeof(CameraPageRenderer))]
namespace FullCameraApp.Droid
{

    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
    {

        public mPreviewCallback(CameraPageRenderer _renderer)
        {
            renderer = _renderer;
        }

        public long total_bytes_sent = 0;

        public CameraPageRenderer renderer;


        DateTime checktime = DateTime.Now;
        DateTime checktimeBattery = DateTime.Now;
        //그냥 놔두면 0~100ms 단위로 호출이 되기 때문에 부하가 심하다

        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
        {
            try
            {
 
                if (data == null)
                    return;

                if (camera == null || camera.GetParameters() == null)
                    return;

                var imageformat = camera.GetParameters()?.PreviewFormat;

                if (renderer == null)
                    return;

                if (renderer.isDestroy == true)
                    return;

                //다음 버퍼예약
                camera.AddCallbackBuffer(renderer.camera_buffer);


                if (checktime > DateTime.Now)
                {
                    return;
                }

                checktime = checktime.AddMilliseconds(20);

                if (checktimeBattery < DateTime.Now)
                {
                    checktimeBattery = checktimeBattery.AddMilliseconds(1000 * 5);
                    NetProcess.SendMachineStatus(MainActivity.BatteryLevel);
                }

                if (NetProcess.TargetPlayerId.Count == 0 && MainActivity.server._Clients.Count == 0)
                {
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        renderer.isDestroy = true;

                        PopupNavigation.Instance.PopAsync();
                        return;
                    });
                   
                }

                switch (imageformat)
                {
                    case ImageFormatType.Nv16:
                    case ImageFormatType.Nv21:
                    case ImageFormatType.Yuy2:
                    case ImageFormatType.Yv12:
                        {
                            int width = camera.GetParameters().PreviewSize.Width;
                            int height = camera.GetParameters().PreviewSize.Height;

                            using (YuvImage img = new YuvImage(data, camera.GetParameters().PreviewFormat, width, height, null))
                            {

                                System.IO.MemoryStream outStream = new System.IO.MemoryStream();
                                {

                                    if (img.CompressToJpeg(new Rect(0, 0, width, height), renderer.quality, outStream) == false)
                                        return;

                                    renderer.Frames.Enqueue(outStream);


                                    renderer.textViewMain.Text = outStream.Length.ToString();

                                    //서버쪽은 임시 주석
                                    if (MainActivity.server.ImagesSource.Count > 500)
                                        MainActivity.server.ImagesSource.Clear();
                                    if (MainActivity.server._Clients.Count > 0)
                                        MainActivity.server.ImagesSource.Enqueue(outStream);

                                    if (renderer.Frames.Count > 10)
                                    {
                                       // Task.Run(() =>
                                      //  {
                                            total_bytes_sent += outStream.Length;

                                            if (NetProcess.TargetPlayerId.Count > 0)
                                            {
                                                NetProcess.SendRoomBITMAPMessage(renderer.Frames, 0);
                                            }

                                        renderer.Frames.Clear();
                                      //  });
                                    }
                                }
                            }

                        }
                        break;
                    case ImageFormatType.Jpeg:

                        renderer.Frames.Enqueue(new System.IO.MemoryStream(data));

                        if (renderer.Frames.Count > 10)
                        {
                            if (MainActivity.server._Clients.Count > 0)
                            {
                                foreach (var frame in renderer.Frames)
                                {
                                    MainActivity.server.ImagesSource.Enqueue(frame);
                                }
                            }

                            if (NetProcess.TargetPlayerId.Count > 0)
                            {
                                NetProcess.SendRoomBITMAPMessage(renderer.Frames, 0);
                            }

                            renderer.Frames.Clear();

                        }
                        break;
                }

            }
            catch (System.Exception ex)
            {
                Method_Android.NotificationException(ex);
            }
        }
    }



    public class CameraPageRenderer : PageRenderer, TextureView.ISurfaceTextureListener
    {

        public int quality = 65;
        public int target_pos = 0;

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
        Dictionary<int, ImageView> imageViewDic = new Dictionary<int, ImageView>();

        //퇴장버튼
        public Button exitButton;

        //카메라 스위치
        Button switchButton;

        //가운대 화면 감추기 버튼
        Button mainScreenButton;


        //퀄리티 업 다운
        Button qualityUp;
        Button qualityDown;

        public TextView textViewMain;


        Android.Hardware.Camera camera;

        AudioManagerM audiomgr = new AudioManagerM();

        Activity Activity => this.Context as Activity;

        CameraPage page;

        public bool isDestroy = false;
        bool isFlashOn = true;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);

            page = (CameraPage)e.NewElement;


            page.OnControl += Element_OnControl;


            SetupUserInterface();
            SetupEventHandlers();

            Frames.Clear();
        }

        int half_width;
        int half_height;

    

        void AddImageView(int pos)
        {
            ///////////////////////////////////////////////////////////////////////////////
            var imageView = new ImageView(Context);
            imageView.Click += async (s, e) =>
            {
                target_pos = pos;
            };

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


        public void Torch(bool on)
        {
            if (!this.Context.PackageManager.HasSystemFeature(PackageManager.FeatureCameraFlash))
            {
                Android.Util.Log.Info("ZXING", "Flash not supported on this device");
                return;
            }

            if (camera == null)
            {
                Android.Util.Log.Info("ZXING", "NULL Camera");
                return;
            }

            var p = camera.GetParameters();
            var supportedFlashModes = p.SupportedFlashModes;

            if (supportedFlashModes == null)
                supportedFlashModes = new List<string>();

            var flashMode = string.Empty;

            if (on)
            {
                if (supportedFlashModes.Contains(Android.Hardware.Camera.Parameters.FlashModeTorch))
                    flashMode = Android.Hardware.Camera.Parameters.FlashModeTorch;
                else if (supportedFlashModes.Contains(Android.Hardware.Camera.Parameters.FlashModeOn))
                    flashMode = Android.Hardware.Camera.Parameters.FlashModeOn;
            }
            else
            {
                if (supportedFlashModes.Contains(Android.Hardware.Camera.Parameters.FlashModeOff))
                    flashMode = Android.Hardware.Camera.Parameters.FlashModeOff;
            }

            if (!string.IsNullOrEmpty(flashMode))
            {
                p.FlashMode = flashMode;
                camera.SetParameters(p);
            }
        }

        Android.Hardware.Camera.CameraInfo cameraInfo = new Android.Hardware.Camera.CameraInfo();



        int camera_preview_pos = 0;

        public void SiwtchCamera()
        {
            StopCamera();

            if (currentFacing == Android.Hardware.CameraFacing.Front)
                currentFacing = Android.Hardware.CameraFacing.Back;
            else
                currentFacing = Android.Hardware.CameraFacing.Front;

            {
                int cameraCount = Android.Hardware.Camera.NumberOfCameras;
                int cameraId = 0;
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

               
                var previewSize = parameters.SupportedPreviewSizes
                                         .OrderBy(s => Math.Abs(s.Width * s.Height))
                                         .First();

                int bitsperpixel = ImageFormat.GetBitsPerPixel(parameters.PreviewFormat);
                int byteperpixel = bitsperpixel / 8;


                camera_buffer = null;
                camera_buffer = new byte[(previewSize.Width * previewSize.Height) * 2];

                parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
                camera.SetParameters(parameters);
                camera.SetPreviewTexture(_surface);
                StartCamera();
            }
        }

        public void QualityUp()
        {
            quality += 10;
            if (quality > 100)
                quality = 100;

            qualityUp.Text = quality.ToString();
        }

        public void QualityDown()
        {
            quality -= 10;
            if (quality < 0)
                quality = 0;

            qualityUp.Text = quality.ToString();
        }

        void AddImageView(int pos, int width, int height)
        {
            ///////////////////////////////////////////////////////////////////////////////
            var imageView = new ImageView(Context);

            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            imageViewParams.Height = height;
            imageViewParams.Width = width;

            imageView.Rotation = 0;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);
            imageViewDic.Add(pos, imageView);
            ///////////////////////////////////////////////////////////////////////////////
        }

        //버튼 안보이게 하기
        bool disable_button = false;

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

            //중앙 스크린
            AddImageView(6, metrics.WidthPixels, half_height * 2);

            ///////////////////////////////////////////////////////////////////////////////
            textViewMain = new TextView(Context);
            RelativeLayout.LayoutParams TextViewParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent,
            RelativeLayout.LayoutParams.WrapContent);
            textViewMain.LayoutParameters = TextViewParams;
            textViewMain.Text = "123124";
            textViewMain.SetTextColor(Color.White);
            textViewMain.Click += async (s, e) =>
            {
                textViewMain.Text = "";
            };

            if (disable_button == false)
                mainLayout.AddView(textViewMain);
            ///////////////////////////////////////////////////////////////////////////////
            ///

            ///////////////////////////////////////////////////////////////////////////////
            mainScreenButton = new Button(Context);
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            mainScreenButton.LayoutParameters = ButtonParams;
            mainScreenButton.Text = "Hide";
            mainScreenButton.Click += async (s, e) =>
            {
                ImageView view;
                if (imageViewDic.TryGetValue(6, out view) == true)
                {
                    if (view.Visibility == ViewStates.Visible)
                    {
                        view.Visibility = ViewStates.Invisible;
                        mainScreenButton.Text = "Show";
                    }
                    else
                    {
                        view.Visibility = ViewStates.Visible;
                        mainScreenButton.Text = "Hide";
                    }
                }
            };
            if (disable_button == false)
                mainLayout.AddView(mainScreenButton);

            ///////////////////////////////////////////////////////////////////////////////
            qualityUp = new Button(Context);
            qualityUp.LayoutParameters = ButtonParams;
            qualityUp.Text = "Up";
            qualityUp.Click += async (s, e) =>
            {
                QualityUp();

            };
            if (disable_button == false)
                mainLayout.AddView(qualityUp);
            ///////////////////////////////////////////////////////////////////////////////
            qualityDown = new Button(Context);
            qualityDown.LayoutParameters = ButtonParams;
            qualityDown.Text = "Down";
            qualityDown.Click += async (s, e) =>
            {
                QualityDown();
            };
            if (disable_button == false)
                mainLayout.AddView(qualityDown);
            ///////////////////////////////////////////////////////////////////////////////
            exitButton = new Button(Context);
            exitButton.LayoutParameters = ButtonParams;
            exitButton.Text = "EXIT";
            exitButton.Click += async (s, e) =>
            {
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    PopupNavigation.Instance.PopAsync();
                });
            };
            if (disable_button == false)
                mainLayout.AddView(exitButton);
            ////////////////////////////////////////////////////////////DrawLayout///////////////////
            switchButton = new Button(Context);
            switchButton.LayoutParameters = ButtonParams;
            switchButton.Text = "Switch";
            switchButton.Click += async (s, e) =>
            {
                SiwtchCamera();
            };
            if (disable_button == false)
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
            foreach (var image in imageViewDic)
            {
                image.Value.SetX(posx % 2 == 0 ? 0 : half_width);
                image.Value.SetY((half_height * posy));

                //마지막 뷰는 0으로 고정 하단 큰뷰
                if (posx == 7)
                {
                    image.Value.SetX(0);
                    image.Value.SetY((half_height * 1));
                }

                posx++;
                posy = posx / 2;
            }


            textViewMain.SetX(half_width);
            textViewMain.SetY(metrics.HeightPixels - 70);

            qualityUp.SetX(half_width);
            qualityUp.SetY(metrics.HeightPixels - 300);

            qualityDown.SetX(half_width + 250);
            qualityDown.SetY(metrics.HeightPixels - 300);

            mainScreenButton.SetX(half_width - 200);
            mainScreenButton.SetY(metrics.HeightPixels - 200);

            exitButton.SetX(0);
            exitButton.SetY(metrics.HeightPixels - 100);

            switchButton.SetX(half_width + 250);
            switchButton.SetY(metrics.HeightPixels - 200);
        }


        async void Element_OnControl(string parameter, Action<string> action)
        {

            switch (parameter)
            {
                case "switch_camera":
                    SiwtchCamera();
                    break;
                case "Flash":
                    {
                        Torch(isFlashOn);

                        isFlashOn = (isFlashOn == true ? false : true);
                    }
                    break;
                case "MIC":
                    audiomgr.isOn = true;
                    break;
            }

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

            camera?.SetPreviewCallbackWithBuffer(null);
            camera?.StopPreview();
            camera?.Release();
            camera = null;

            camera_buffer = null;
        }

        private void SetDisplayOrientation()
        {
            var rotation = this.Activity.WindowManager.DefaultDisplay.Rotation;
            int degrees = 0;
            switch (rotation)
            {
                case SurfaceOrientation.Rotation0:
                    degrees = 0;
                    break;
                case SurfaceOrientation.Rotation90:
                    degrees = 90;
                    break;
                case SurfaceOrientation.Rotation180:
                    degrees = 180;
                    break;
                case SurfaceOrientation.Rotation270:
                    degrees = 270;
                    break;
            }
            int result;
            if (cameraInfo.Facing == Android.Hardware.CameraFacing.Front)
            {
                result = (cameraInfo.Orientation + degrees) % 360;
                result = (360 - result) % 360; // compensate the mirror
            }
            else
            {   // back-facing
                result = (cameraInfo.Orientation - degrees + 360) % 360;
            }
            camera.SetDisplayOrientation(result);
        }


        public byte[] camera_buffer;
        private void StartCamera()
        {
            try
            {
                camera.AddCallbackBuffer(camera_buffer);
                camera.SetPreviewCallbackWithBuffer(new mPreviewCallback(this));
                camera.StartPreview();

                //SetDisplayOrientation();
            }
            catch (Exception e)
            { 
            }
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

                if(camera == null)
                {
                    return;
                }


                var parameters = camera.GetParameters();
                if (parameters == null)
                    return;


                var aspect = ((decimal)height) / ((decimal)width);

                // Find the preview aspect ratio that is closest to the surface aspect
                var previewSize = parameters.SupportedPreviewSizes
                                            .OrderBy(s => Math.Abs(s.Width * s.Height))
                                            .First();

                //var previewSize = parameters.SupportedPreviewSizes[2];
                //     mainLayout.LayoutParameters.Height = previewSize.Height;
                //     mainLayout.LayoutParameters.Width = previewSize.Width;

                int bitsperpixel = ImageFormat.GetBitsPerPixel(parameters.PreviewFormat);
                float byteperpixel = bitsperpixel / 8;

          
                camera_buffer = null;
                camera_buffer = new byte[(previewSize.Width * previewSize.Height) * 2];

                parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
                camera.SetParameters(parameters);
                camera.SetPreviewTexture(surface);
                StartCamera();

                ////caemra page render
                //Task.Run(() =>
                //{
                //    NetProcess.JpegStream.Clear();

                //    DateTime chk = DateTime.Now;
                //    while (isDestroy == false)
                //    {
                //        try
                //        {
                //            if (NetProcess.JpegStream.Count == 0)
                //                continue;

                //            if (chk < DateTime.Now)
                //            {
                //                exitButton.Text = "exit";

                //                chk = DateTime.Now.AddSeconds(3);
                //            }



                //            StreamWrapper ms;
                //            while (NetProcess.JpegStream.TryDequeue(out ms) == true)
                //            {
                //                if (ms == null)
                //                    continue;

                //                var bitmap = BitmapFactory.DecodeByteArray(ms?.stream.ToArray(), 0, ms.stream.ToArray().Length);

                //                MainThread.BeginInvokeOnMainThread(() =>
                //                {

                //                    ImageView imageView;
                //                    if (imageViewDic.TryGetValue(ms.pos, out imageView) == true)
                //                        imageView?.SetImageBitmap(bitmap);
                //                    else
                //                    {
                //                        AddImageView(ms.pos);

                //                        if (imageViewDic.TryGetValue(ms.pos, out imageView) == true)
                //                            imageView?.SetImageBitmap(bitmap);
                //                    }


                //                    //제일 하단 큰 스크린

                //                    if (ms.pos == target_pos)
                //                    {
                //                        ImageView imageView2;
                //                        if (imageViewDic.TryGetValue(6, out imageView2) == true)
                //                            imageView2?.SetImageBitmap(bitmap);
                //                        else
                //                        {
                //                            AddImageView(ms.pos);

                //                            if (imageViewDic.TryGetValue(6, out imageView2) == true)
                //                                imageView2?.SetImageBitmap(bitmap);
                //                        }
                //                    }

                //                });
                //            }


                //            Thread.Sleep(1);

                //        }
                //        catch (Exception e)
                //        {
                //            Method_Android.NotificationException(e.Message + e.Source);
                //        }
                //    }
                //});


                //caemra page render
                //Task.Run(() =>
                //{
                //    //오디오는 임시 주석 처리
                //    //2022-10-24
                //    audiomgr?.record();
                //});


                //Task.Run(() =>
                //{
                //    while (isDestroy == false)
                //    {
                //        StreamWrapper ms;
                //        if (NetProcess.AudioStream.TryDequeue(out ms) == true)
                //        {
                //            if (ms == null)
                //                continue;

                //            audiomgr?.play(ms.stream.ToArray());

                //        }

                //        Thread.Sleep(1);
                //    }

                //});

            }

        }

        // used to marshal back to UI thread
        private SynchronizationContext _context;


        public bool OnSurfaceTextureDestroyed(Android.Graphics.SurfaceTexture surface)
        {

            try
            {
                isDestroy = true;

                StopCamera();
                Thread.Sleep(500);

                audiomgr?.Clear();

                Torch(false);

                Frames.Clear();

               
            }
            catch(Exception ex)
            {
                Method_Android.NotificationException(ex);
            }

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