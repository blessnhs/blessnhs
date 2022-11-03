﻿using System;
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
            NetProcess.AudioStream.Clear();
        }

        RelativeLayout mainLayout;

        List<Button> alignList = new List<Button>();

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
            try
            {                var metrics = Resources.DisplayMetrics;

                half_width = metrics.WidthPixels / 2;
                half_height = metrics.HeightPixels / 4;


                mainLayout = new RelativeLayout(Context);
                mainLayout.SetBackgroundColor(Color.Black);

                /////////////////////////////////////////////////////////////////////////////////
                //중앙 스크린
                AddImageView(metrics.WidthPixels, metrics.HeightPixels);

                ///////////////////////////////////////////////////////////////////////////////
                Button batteryGageButton = new Button(Context);
                RelativeLayout.LayoutParams ViewParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WrapContent,
                RelativeLayout.LayoutParams.WrapContent);
                batteryGageButton.LayoutParameters = ViewParams;
                batteryGageButton.Text = "0%";
                batteryGageButton.Click += async (s, e) =>
                {
                };
                mainLayout.AddView(batteryGageButton);
                alignList.Add(batteryGageButton);
                ///////////////////////////////////////////////////////////////////////////////
                ///////////////////////////////////////////////////////////////////////////////
                Button MICButton = new Button(Context);
                MICButton.Text = "MIC";
                MICButton.Click += async (s, e) =>
                {
                    NetProcess.SendCameraControl(page.MachinId, page.PlayerId, CameraControlType.Mic);
                    MICButton.Text = (MICButton.Text == "OFF" ? "ON" : "OFF");
                };
                alignList.Add(MICButton);
                mainLayout.AddView(MICButton);
                ///////////////////////////////////////////////////////////////////////////////            ///
                Button switchButton = new Button(Context); ;
                switchButton.Text = "Switch";
                switchButton.Click += async (s, e) =>
                {
                    NetProcess.SendCameraControl(page.MachinId, page.PlayerId, CameraControlType.SwitchCamera);
                };
                alignList.Add(switchButton);
                mainLayout.AddView(switchButton);
                ///////////////////////////////////////////////////////////////////////////////
                Button flashButton = new Button(Context);
                flashButton.Text = "Flash";
                flashButton.Click += async (s, e) =>
                {
                    NetProcess.SendCameraControl(page.MachinId, page.PlayerId, CameraControlType.Flash);
                };

                alignList.Add(flashButton);
                mainLayout.AddView(flashButton);
                ///////////////////////////////////////////////////////////////////////////////
                Button exitButton = new Button(Context);
                exitButton.LayoutParameters = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WrapContent,
                RelativeLayout.LayoutParams.WrapContent);
                exitButton.Text = "EXIT";
                exitButton.Click += async (s, e) =>
                {
                    PopupNavigation.Instance.PopAsync();
                };

                alignList.Add(exitButton);
                mainLayout.AddView(exitButton);
                ////////////////////////////////////////////////////////////DrawLayout///////////////////
                AddView(mainLayout);
            }
            catch(Exception e)
            {

            }
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

            int screen_width = half_width + half_width;

            //한줄에 몇개 보여줄지
            int displayObjectCount = alignList.Count;

            //스크린을 나눈 크기
            int divide_screen_by_button_count = (int)screen_width / displayObjectCount;
            int button_width = divide_screen_by_button_count;

            int ypoisition = metrics.HeightPixels - 100;

            int i = 0;
            foreach (var button in alignList)
            {
                int current_position = (divide_screen_by_button_count * i);
                button.SetX(current_position);
                button.SetY(ypoisition);
                button.SetWidth(divide_screen_by_button_count);

                i++;
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

                        MainThread.BeginInvokeOnMainThread(() =>
                        {
                            StreamWrapper ms;
                            while (NetProcess.JpegStream.TryDequeue(out ms) == true)
                            {
                                if (ms == null)
                                    continue;

                                var bitmap = BitmapFactory.DecodeByteArray(ms?.stream.ToArray(), 0, ms.stream.ToArray().Length);

                                imageView?.SetImageBitmap(bitmap);


                                alignList[0].Text = page.TargetBatteryLevel + "%";
                            }
                        });

                    }
                    catch (Exception e)
                    {
                        Method_Android.NotificationException(e.Message);
                    }

                    Thread.Sleep(1);
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

                    Thread.Sleep(1);
                }

            });

        }

    }

}


