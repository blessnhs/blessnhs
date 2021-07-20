//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using Android;
//using Android.App;
//using Android.Content;
//using Android.Content.PM;
//using Android.Graphics;
//using Android.Support.V4.App;
//using Android.Support.V4.Content;
//using Android.Views;
//using Android.Widget;
//using Antioch.Droid;
//using Xamarin.Forms.Platform.Android;

//using Plugin.CurrentActivity;

//[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraPage), typeof(CameraPageRenderer))]
//namespace Antioch.Droid
//{
//    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
//    {
      

//        DateTime time = DateTime.Now;
//        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
//        {
//            var paras = camera.GetParameters();
//            var imageformat = paras.PreviewFormat;
//            if (imageformat == Android.Graphics.ImageFormatType.Nv21)
//            {

//                  Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
//                      imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);

//                if (time <= DateTime.Now)
//                {
//                    using (System.IO.MemoryStream outStream = new System.IO.MemoryStream())
//                    {
//                        bool didIt = img.CompressToJpeg(new Rect(0, 0, paras.PreviewSize.Width, paras.PreviewSize.Height), 75, outStream);
//                        outStream.Seek(0, System.IO.SeekOrigin.Begin);
//                        //     Bitmap newBM = BitmapFactory.DecodeStream(outStream);

//                        NetProcess.SendRoomMessage(outStream.ToArray());
//                        time = DateTime.Now.AddMilliseconds(300);
//                    }
                      
//                }
//            }
//        }
//    }

//    public class CameraPageRenderer : PageRenderer, TextureView.ISurfaceTextureListener
//    {
//        Activity CurrentContext => CrossCurrentActivity.Current.Activity;

//        public CameraPageRenderer(Context context) : base(context)
//        {
           
//        }

//        RelativeLayout mainLayout;
//        TextureView liveView;

//        Android.Hardware.Camera camera;

//        Activity Activity => this.Context as Activity;

//        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
//        {
//            base.OnElementChanged(e);
//            SetupUserInterface();
//            SetupEventHandlers();
//        }

//        void SetupUserInterface()
//        {
//            mainLayout = new RelativeLayout(Context);

//            //RelativeLayout.LayoutParams mainLayoutParams = new RelativeLayout.LayoutParams(
//            //	RelativeLayout.LayoutParams.MatchParent,
//            //	RelativeLayout.LayoutParams.MatchParent);
//            //mainLayout.LayoutParameters = mainLayoutParams;

//            liveView = new TextureView(Context);

//            RelativeLayout.LayoutParams liveViewParams = new RelativeLayout.LayoutParams(
//                400,
//                400);
//            liveView.LayoutParameters = liveViewParams;
//            mainLayout.AddView(liveView);


//            AddView(mainLayout);
//        }

//        protected override void OnLayout(bool changed, int l, int t, int r, int b)
//        {
//            base.OnLayout(changed, l, t, r, b);
//            if (!changed)
//                return;
//            var msw = MeasureSpec.MakeMeasureSpec(r - l, MeasureSpecMode.Exactly);
//            var msh = MeasureSpec.MakeMeasureSpec(b - t, MeasureSpecMode.Exactly);
//            mainLayout.Measure(msw, msh);
//            mainLayout.Layout(0, 0, r - l, b - t);
//        }

//        public void SetupEventHandlers()
//        {
//            liveView.SurfaceTextureListener = this;
//        }

//        public override bool OnKeyDown(Keycode keyCode, KeyEvent e)
//        {
//            if (keyCode == Keycode.Back)
//            {
             
//                return false;
//            }
//            return base.OnKeyDown(keyCode, e);
//        }

//        public async Task<byte[]> TakePhoto()
//        {
//            camera.StopPreview();
//            var ratio = ((decimal)Height) / Width;

//            float degrees = 90;
//            Matrix matrix = new Matrix();
//            matrix.SetRotate(degrees);
//            var image = Bitmap.CreateBitmap(liveView.Bitmap, 0, 0, liveView.Bitmap.Width, (int)(liveView.Bitmap.Width * ratio), matrix,false);
//            byte[] imageBytes = null;
//            using (var imageStream = new System.IO.MemoryStream())
//            {
//                await image.CompressAsync(Bitmap.CompressFormat.Jpeg, 50, imageStream);
//                image.Recycle();
//                imageBytes = imageStream.ToArray();
//            }
//            camera.StartPreview();
//            return imageBytes;
//        }

//        private void StopCamera()
//        {
//            camera.StopPreview();
//            camera.Release();
//        }

//        private void StartCamera()
//        {
//            camera.SetDisplayOrientation(90);
//            camera.SetPreviewCallback(new mPreviewCallback());
//            camera.StartPreview();
//        }


//        #region TextureView.ISurfaceTextureListener implementations

//        public void OnSurfaceTextureAvailable(SurfaceTexture surface, int width, int height)
//        {
//                {

//                {
//                    int cameraCount = Android.Hardware.Camera.NumberOfCameras;
//                    int cameraId = 0;
//                    Android.Hardware.Camera.CameraInfo cameraInfo = new Android.Hardware.Camera.CameraInfo();
//                    for (int camIdx = 0; camIdx < cameraCount; camIdx++)
//                    {
//                        Android.Hardware.Camera.GetCameraInfo(camIdx, cameraInfo);
//                        if (cameraInfo.Facing == Android.Hardware.CameraFacing.Front)
//                        {
//                            camera = Android.Hardware.Camera.Open(camIdx);

//                            cameraId = camIdx;
//                            break;
//                        }
//                    }

//                    if (camera == null)
//                        camera = Android.Hardware.Camera.Open();
//                }


//                var parameters = camera.GetParameters();
//                var aspect = ((decimal)height) / ((decimal)width);

//                // Find the preview aspect ratio that is closest to the surface aspect
//                //var previewSize = parameters.SupportedPreviewSizes
//                //                            .OrderBy(s => Math.Abs(s.Width / (decimal)s.Height - aspect))
//                //                            .First();

//                var previewSize = parameters.SupportedPreviewSizes[0];

//                System.Diagnostics.Debug.WriteLine($"Preview sizes: {parameters.SupportedPreviewSizes.Count}");

//                mainLayout.LayoutParameters.Height = previewSize.Height;
//                mainLayout.LayoutParameters.Width = previewSize.Width;

//                parameters.SetPreviewSize(previewSize.Width, previewSize.Height);
//                camera.SetParameters(parameters);

//                camera.SetPreviewTexture(surface);
//                StartCamera();
//            }
//        }

//        public bool OnSurfaceTextureDestroyed(Android.Graphics.SurfaceTexture surface)
//        {
//            StopCamera();
//            return true;
//        }

//        public void OnSurfaceTextureSizeChanged(Android.Graphics.SurfaceTexture surface, int width, int height)
//        {
//        }

//        public void OnSurfaceTextureUpdated(Android.Graphics.SurfaceTexture surface)
//        {
//        }
//        #endregion
//    }

//}