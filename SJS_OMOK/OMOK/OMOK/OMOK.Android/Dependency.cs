using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

using Android.App;
using Android.Content;
using Android.Gms.Auth.Api;
using Android.Gms.Auth.Api.SignIn;
using Android.Gms.Common.Apis;
using Android.Media;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Firebase.Auth;
using OMOK.Droid;
using ToastMessage.Droid;
using Xamarin.Forms;
using Xamarin.Forms.Platform.Android;

[assembly: Xamarin.Forms.Dependency(typeof(Toast_Android))]

namespace ToastMessage.Droid
{
    public class Toast_Android : Toast
    {
        public void Show(string message)
        {
            var toast = Android.Widget.Toast.MakeText(Android.App.Application.Context, message, ToastLength.Short);
            toast.SetGravity(GravityFlags.Center | GravityFlags.Center, 0, 5);
            toast.Show();

        }

        static MediaRecorder recorder;

        public void StopRecord()
        {
            try
            {
                recorder?.Stop();
            }
            catch (Exception)
            {

            }
        }

        public void StartRecord(object video_obj)
        {
            VideoView video = video_obj as VideoView; 
            InitCamera();

            video.StopPlayback();

            var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
            var filename = System.IO.Path.Combine(documents, "Write.MP4");

            File.Delete(filename);

            var cameraParameters = camera.GetParameters();
            var supportedSizes = cameraParameters.SupportedVideoSizes;
            var supportedPreviewSizes = cameraParameters.SupportedPreviewSizes;

            camera.Unlock();

            recorder?.Release();
            recorder = null;

            recorder = new MediaRecorder();
            recorder.SetCamera(camera);

            recorder.SetVideoSource(VideoSource.Camera);
            recorder.SetAudioSource(AudioSource.Camcorder);
            recorder.SetOutputFormat(OutputFormat.Mpeg4);

            recorder.SetVideoSize(supportedSizes[0].Width, supportedSizes[0].Height);

            recorder.SetVideoEncoder(VideoEncoder.Mpeg4Sp);
            recorder.SetAudioEncoder(AudioEncoder.AmrNb);

            recorder.SetPreviewDisplay(video.Holder.Surface);

            recorder.SetOutputFile(filename);
            recorder.Prepare();
            recorder.Start();
        }


        int cameraId;
        static Android.Hardware.Camera camera;

        private bool OpenCamera(int id)
        {
            //Optns a camera and return true/false if it worked.
            try
            {
                camera = Android.Hardware.Camera.Open(id);
                cameraId = id;
                camera.StartPreview();

                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        private void InitCamera()
        {
            bool success = false;
            success = OpenCamera(1); //Try the face camera

            if (success == false)
            {
                success = OpenCamera(0); //Try the rear camera
            }

        }
        public void Record()
        {


        }
    }
}
