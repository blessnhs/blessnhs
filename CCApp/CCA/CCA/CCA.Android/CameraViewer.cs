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

        public TextView textViewMain;

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
            AddImageView(metrics.WidthPixels, half_height * 2);

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
            RelativeLayout.LayoutParams ButtonParams = new RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.WrapContent, RelativeLayout.LayoutParams.WrapContent);
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


            textViewMain.SetX(half_width);
            textViewMain.SetY(metrics.HeightPixels - 70);


            exitButton.SetX(half_width);
            exitButton.SetY(metrics.HeightPixels - 200);
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
                            }
                        });

                    }
                    catch (Exception e)
                    {

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


