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
using Android.Content.PM;
using FullCameraApp.Droid;
using Antioch;
using Antioch.Droid;
using Plugin.CurrentActivity;
using rtaNetworking.Streaming;
using Xamarin.Forms.Platform.Android;
using Antioch.View;
using Rg.Plugins.Popup.Services;
using Xamarin.Essentials;
using Android.Media;
using Android.OS;
using Java.IO;
using System.IO;
using static Android.Hardware.Camera;
using System.Linq;


[assembly: Xamarin.Forms.ExportRenderer(typeof(QualityCam), typeof(QualityCamRenderer))]
namespace FullCameraApp.Droid
{
    public class StreamMediaDataSource : MediaDataSource
    {
        private volatile byte[] mBuffer;    // byte array for whole media

        private int fileSize;

        private int writeIndex;

        public StreamMediaDataSource(int fileSize)
        {
            this.fileSize = fileSize;

            mBuffer = new byte[this.fileSize];

        }

        public void inputData(byte[] data, int length)
        {

            if (mBuffer != null)
            {

                Buffer.BlockCopy(data, 0, mBuffer, writeIndex, length);

                writeIndex += length;

            }

        }

        public override long Size
            => fileSize;

        public override void Close()
        {
            mBuffer = null;
        }

        public override int ReadAt(long position, byte[] buffer, int offset, int size)
        {
            if (position == writeIndex)
            {

                return -1;

            }

            if (position + size > writeIndex)
            {

                if (fileSize == writeIndex)
                {

                    int rest = (int)(writeIndex - position);

                    Buffer.BlockCopy(mBuffer, (int)position, buffer, offset, rest);

                    return rest;

                }
                else
                {

                    // loading data is faster than downloading data.

                    try
                    {

                        Thread.Sleep(300);    // wait a second for downloading.

                        // or MediaPlayer.pause();

                    }
                    catch (Exception e)
                    {
                    }

                }

            }

            if (mBuffer != null)
            {

                Buffer.BlockCopy(mBuffer, (int)position, buffer, offset, size);

                return size;

            }

            return 0;
        }
    }

    public class mPreviewCallback2 : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
    {
        public long total_bytes_sent = 0;

        public QualityCamRenderer renderer;

        public ConcurrentQueue<System.IO.MemoryStream> Frames = new ConcurrentQueue<System.IO.MemoryStream>();

        DateTime checktime = DateTime.Now;
        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
        {


        }
    }



    public class QualityCamRenderer : PageRenderer, TextureView.ISurfaceTextureListener
    {
        Activity CurrentContext => CrossCurrentActivity.Current.Activity;

        public ImageStreamingServer server = new ImageStreamingServer();

        public int quality = 0;
        public int target_pos = 0;

        public QualityCamRenderer(Context context) : base(context)
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
        Button exitButton;

        //카메라 스위치
        Button switchButton;

        //가운대 화면 감추기 버튼
        Button mainScreenButton;

        VideoView video_view;
        VideoView video_view2;


        //퀄리티 업 다운
        Button qualityUp;
        Button qualityDown;

        public TextView textViewMain;


        Android.Hardware.Camera camera;

        AudioManagerM audiomgr = new AudioManagerM();

        Activity Activity => this.Context as Activity;

        QualityCam page;

        bool isDestroy = false;

        MediaRecorder recorder;

        Parameters parameters;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);

            page = (QualityCam)e.NewElement;

            SetupUserInterface();
            SetupEventHandlers();
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

        bool cameraon = true;
        string dw_path = Android.OS.Environment.GetExternalStoragePublicDirectory(Android.OS.Environment.DirectoryDownloads).AbsolutePath;

        Dictionary<int, byte[]> check = new Dictionary<int, byte[]>();

        Java.IO.File file()
        {


            var path = System.IO.Path.Combine(dw_path, "1.ts");
            System.IO.File.Delete(path);

            Java.IO.File f = new Java.IO.File(path);

            ParcelFileDescriptor pipe = ParcelFileDescriptor.Open(f,
                             ParcelFileMode.Create
                            | ParcelFileMode.Append
                            | ParcelFileMode.ReadWrite);



            byte[] buffer = new byte[4024];
            int length;
            int total = 0;
            Task.Run(() =>
            {
                InputStream fileStream = new FileInputStream(pipe.FileDescriptor);
                int order = 1;
                while (cameraon == true)
                {
                    length = fileStream.Read(buffer);
                    if (length > 0)
                    {
                        byte[] c = new byte[length];
                        Buffer.BlockCopy(buffer, 0, c, 0, length);
                        check[order] = c;
                        NetProcess.SendRoomMPEG2TSMessage(new MemoryStream(buffer, 0, length), order++);

                        total += length;
                        mainScreenButton.Text = (total / 1024).ToString();

                    }
                }

            });
            return f;
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

        string CreateNewPlayFile(ParcelFileDescriptor pipe)
        {
            var pathw = System.IO.Path.Combine(dw_path, "3.ts");


            byte[] buffer = new byte[1024 * 40];
            int length;

            InputStream fileStream = new FileInputStream(pipe.FileDescriptor);
            var stream = new FileStream(pathw, FileMode.Create);
            while ((length = fileStream.Read(buffer)) > 0)
            {
                stream.Write(buffer, 0, length);
                stream.Flush();
            }
            stream.Close();

            return pathw;


        }

        void play()
        {
           
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
            mainLayout.AddView(mainScreenButton);


            ////////////////////////////////////////////////////////////////////////////////
            /// 
            video_view = new VideoView(Context);
            RelativeLayout.LayoutParams VideoViewParams = new RelativeLayout.LayoutParams(half_width * 2, half_height * 2);
            video_view.LayoutParameters = VideoViewParams;


            mainLayout.AddView(video_view);


            video_view2 = new VideoView(Context);
            video_view2.LayoutParameters = VideoViewParams;


            mainLayout.AddView(video_view2);

            ///////////////////////////////////////////////////////////////////////////////
            qualityUp = new Button(Context);
            qualityUp.LayoutParameters = ButtonParams;
            qualityUp.Text = "Up";
            qualityUp.Click += async (s, e) =>
            {
                quality++;
                if (quality > 100)
                    quality = 100;

                qualityUp.Text = quality.ToString();

                var ct = Android.App.Application.Context;
                var audiosvr = ((AudioManager)ct.GetSystemService("audio"));
                audiosvr.SetStreamMute(Android.Media.Stream.Alarm, true);


                this.recorder = new MediaRecorder();
                this.camera.StopPreview();
                this.camera.Unlock();

                recorder = new MediaRecorder();
                recorder.SetCamera(camera);

                recorder.SetVideoSource(VideoSource.Camera);
                recorder.SetAudioSource(AudioSource.Camcorder);
                recorder.SetOutputFormat(OutputFormat.Mpeg2Ts);


                recorder.SetVideoSize(parameters.SupportedVideoSizes[0].Width, parameters.SupportedVideoSizes[0].Height);

                recorder.SetVideoEncoder(VideoEncoder.H264);
                recorder.SetAudioEncoder(AudioEncoder.Aac);
                recorder.SetVideoEncodingBitRate(3000000);

                recorder.SetPreviewDisplay(video_view.Holder.Surface);

                recorder.SetOutputFile(file());

                recorder.Prepare();
                recorder.Start();
            };
            mainLayout.AddView(qualityUp);
            ///////////////////////////////////////////////////////////////////////////////
            qualityDown = new Button(Context);
            qualityDown.LayoutParameters = ButtonParams;
            qualityDown.Text = "Down";
            qualityDown.Click += async (s, e) =>
            {

                qualityUp.Text = (quality).ToString();

                try
                {
                    var path = System.IO.Path.Combine(dw_path, "2.ts");

                    Java.IO.File f = new Java.IO.File(path);

                    ParcelFileDescriptor pipe = ParcelFileDescriptor.Open(f,
                                     ParcelFileMode.ReadOnly);

                    var mediaPlayer = new MediaPlayer();

                    video_view2.StopPlayback();

                    ISurfaceHolder holder = video_view2.Holder;
                    mediaPlayer.SetDisplay(holder);


                    //////////////////////////////////////////////////////////////////////
                    ///새파일 쓰기
                    ///

                    var newPipe = CreateNewPlayFile(pipe);


                    //////////////////////////////////////////////////////////////////////
                    ///
                    mediaPlayer.SetDataSource(newPipe);


                    mediaPlayer.Prepare();
                    mediaPlayer.Start();

                    mediaPlayer.Completion += (sender, args) =>
                    {
                        video_view2.StopPlayback();

                        int pos = mediaPlayer.CurrentPosition;

                        var Pipew2 = CreateNewPlayFile(pipe);

                        mediaPlayer.Reset();
                        mediaPlayer.SetDisplay(holder);
                        mediaPlayer.SetDataSource(Pipew2);
                        mediaPlayer.Prepare();
                        mediaPlayer.SeekTo(pos);

                        mediaPlayer.Prepared += (sa, a) =>
                        {
                            mediaPlayer.Start();
                        };




                    };

                }
                catch (Exception eb)
                {
                }
            };
            mainLayout.AddView(qualityDown);
            ///////////////////////////////////////////////////////////////////////////////
            exitButton = new Button(Context);
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

                    parameters = camera.GetParameters();

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

            video_view2.SetX(0);
            video_view2.SetY(half_height);


            textViewMain.SetX(half_width);
            textViewMain.SetY(metrics.HeightPixels - 70);

            qualityUp.SetX(half_width);
            qualityUp.SetY(metrics.HeightPixels - 300);

            qualityDown.SetX(half_width + 250);
            qualityDown.SetY(metrics.HeightPixels - 300);

            mainScreenButton.SetX(half_width - 200);
            mainScreenButton.SetY(metrics.HeightPixels - 200);

            exitButton.SetX(half_width);
            exitButton.SetY(metrics.HeightPixels - 200);

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



        private void StartCamera()
        {
            mPreviewCallback2 callbackcamera = new mPreviewCallback2();

            camera.SetPreviewCallback(callbackcamera);
            camera.StartPreview();
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


                parameters = camera.GetParameters();
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
                    NetProcess.Mpeg2Stream.Clear();

                    var path2 = System.IO.Path.Combine(dw_path, "2.ts");
                    System.IO.File.Delete(path2);

                    var stream = new FileStream(path2, FileMode.Create);
                    int order = 1;

                    while (isDestroy == false)
                    {
                        StreamWrapper ms;
                        if (NetProcess.Mpeg2Stream.TryDequeue(out ms) == true)
                        {
                            if (ms == null)
                                continue;

                            if (ms.type != order)
                                ;

                            order++;

                            var r1 = check[ms.type];
                            var r2 = ms.stream.ToArray();

                            var r = r1.SequenceEqual(r2);
                            if (r == false)
                                ;

                            quality += (int)ms.stream.Length;
                            stream.Write(ms.stream.ToArray(), 0, (int)ms.stream.Length);
                            stream.Flush();


                            textViewMain.Text = (quality / 1024).ToString();
                        }
                    }

                    stream.Close();
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