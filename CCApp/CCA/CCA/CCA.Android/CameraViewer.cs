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
using DependencyHelper.Droid;

[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraViewer), typeof(CameraViewerRenderer))]
namespace FullCameraApp.Droid
{


    public class CameraViewerRenderer : PageRenderer
    {
        public CameraViewerRenderer(Context context) : base(context)
        {
            NetProcess.JpegStream.Clear();
        }

        RelativeLayout mainLayout;

        //퇴장버튼
        Button exitButton;

        //카메라 변환
        Button switchButton;

        //카메라 변환
        Button flashButton;

        //배터리 게이지
        Button textViewBattery;

        AudioManagerM audiomgr = new AudioManagerM();
        ImageView imageView;

        Activity Activity => this.Context as Activity;

        CameraViewer page;

        bool isDestroy = false;

        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Page> e)
        {
            base.OnElementChanged(e);

            page = (CameraViewer)e.NewElement;


            SetupUserInterface();
            SetUpPlayTask();

        }

        int half_width;
        int half_height;


        void AddImageView(int pos)
        {
            ///////////////////////////////////////////////////////////////////////////////
            imageView = new ImageView(Context);


            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            imageViewParams.Height = half_height;
            imageViewParams.Width = half_width;

            imageView.Rotation = 0;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);
            ///////////////////////////////////////////////////////////////////////////////
        }


        void AddImageView(int width, int height)
        {
            ///////////////////////////////////////////////////////////////////////////////
            imageView = new ImageView(Context);

            RelativeLayout.LayoutParams imageViewParams = new RelativeLayout.LayoutParams(
             RelativeLayout.LayoutParams.WrapContent,
             RelativeLayout.LayoutParams.WrapContent);
            imageViewParams.Height = height;
            imageViewParams.Width = width;

            imageView.Rotation = 0;
            imageView.LayoutParameters = imageViewParams;
            imageView.SetScaleType(ImageView.ScaleType.FitXy);
            mainLayout.AddView(imageView);
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
            //중앙 스크린
            AddImageView(metrics.WidthPixels, metrics.HeightPixels);

            ///////////////////////////////////////////////////////////////////////////////
            textViewBattery = new Button(Context);
            RelativeLayout.LayoutParams TextViewParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent,
            RelativeLayout.LayoutParams.WrapContent);
            textViewBattery.LayoutParameters = TextViewParams;
            textViewBattery.Text = "0%";
            textViewBattery.Click += async (s, e) =>
            {
            };
            mainLayout.AddView(textViewBattery);
            ///////////////////////////////////////////////////////////////////////////////
            ///

            ///////////////////////////////////////////////////////////////////////////////
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent, RelativeLayout.LayoutParams.WrapContent);
            ///////////////////////////////////////////////////////////////////////////////
            exitButton = new Button(Context);
            exitButton.LayoutParameters = ButtonParams;
            exitButton.Text = "EXIT";
            exitButton.Click += async (s, e) =>
            {
                PopupNavigation.Instance.PopAsync();
            };
            mainLayout.AddView(exitButton);
            ////////////////////////////////////////////////////////////DrawLayout///////////////////
            ///////////////////////////////////////////////////////////////////////////////
            switchButton = new Button(Context);
            switchButton.LayoutParameters = ButtonParams;
            switchButton.Text = "Switch";
            switchButton.Click += async (s, e) =>
            {
                NetProcess.SendCameraControl(page.MachinId, page.PlayerId, CameraControlType.SwitchCamera);
            };
            mainLayout.AddView(switchButton);
            ////////////////////////////////////////////////////////////DrawLayout///////////////////           
            ///////////////////////////////////////////////////////////////////////////////
            flashButton = new Button(Context);
            flashButton.LayoutParameters = ButtonParams;
            flashButton.Text = "Flash";
            flashButton.Click += async (s, e) =>
            {
                NetProcess.SendCameraControl(page.MachinId, page.PlayerId, CameraControlType.Flash);
            };
            mainLayout.AddView(flashButton);
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



            int width = half_width + half_width;

            int button_width = exitButton.Width;

            int displayObjectCount = 4;

            int pos3 = (int)width / displayObjectCount;
            double remain = Math.Abs(pos3 - button_width) * displayObjectCount;


            int xaddspace = (int)(remain / displayObjectCount);
            int ypoisition = metrics.HeightPixels - 100;

     
            {
                int i = 0;
                int current_position = (pos3 * i) + xaddspace;

                exitButton.SetX(current_position);
                exitButton.SetY(ypoisition);

                i = 1;
                current_position = (pos3 * i) + xaddspace;

                switchButton.SetX(current_position);
                switchButton.SetY(ypoisition);

                i = 2;
                current_position = (pos3 * i) + xaddspace;

                flashButton.SetX(current_position);
                flashButton.SetY(ypoisition);


                i = 3;
                current_position = (pos3 * i) + xaddspace;


                textViewBattery.SetX(current_position);
                textViewBattery.SetY(ypoisition);

            }


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

        public void SetUpPlayTask()
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
                        if (NetProcess.JpegStream.Count == 0)
                            continue;

                        if (chk < DateTime.Now)
                        {
                            exitButton.Text = "exit";

                            chk = DateTime.Now.AddSeconds(3);
                        }


                        MainThread.BeginInvokeOnMainThread(() =>
                        {
                            StreamWrapper ms;
                            while (NetProcess.JpegStream.TryDequeue(out ms) == true)
                            {
                                if (ms == null)
                                    continue;

                                var bitmap = BitmapFactory.DecodeByteArray(ms?.stream.ToArray(), 0, ms.stream.ToArray().Length);

                                imageView?.SetImageBitmap(bitmap);


                                textViewBattery.Text = page.TargetBatteryLevel + "%";
                            }
                        });

                    }
                    catch (Exception e)
                    {
                        Method_Android.NotificationException(e.Message);
                    }
                }
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

                        audiomgr?.play(ms.stream.ToArray());

                    }
                }

            });

        }

    }

}


