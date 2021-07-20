//using System;
//using Android.App;
//using Android.Content;
//using Android.Views;
//using Android.Widget;
//using Antioch.Droid;
//using Xamarin.Forms.Platform.Android;
//using Android.Media;
//using System.IO;
//using System.Threading.Tasks;
//using System.Threading;

//[assembly: Xamarin.Forms.ExportRenderer(typeof(CameraPage), typeof(CameraPageRenderer))]
//namespace Antioch.Droid
//{
//    public class CameraPageRenderer : PageRenderer
//    {
//        bool isPlay = false;
//        Activity Activity => this.Context as Activity;

//        public CameraPageRenderer(Context context) : base(context)
//        {
//            Activity.SetContentView(Resource.Layout.camera);

//            InitCamera();

//            System.Timers.Timer recordtimer = new System.Timers.Timer();
//            recordtimer.Interval = 3000;
//            recordtimer.Enabled = true;
//            recordtimer.Elapsed += OnTimedEvent;

//            void OnTimedEvent(object sender, System.Timers.ElapsedEventArgs e)
//            {
//                Task.Run(() =>
//                {
//                    DateTime CheckTime = DateTime.Now;

//                    Activity.RunOnUiThread(() =>
//                    {
//                         if (DateTime.Now >= CheckTime)
//                         {
//                             try
//                             {
//                                 if (isPlay == false)
//                                 {
//                                     isPlay = true;

//                                  //   StartRecord();
//                                 }
//                                 else
//                                 {

//                                     isPlay = false;

//                                  //   recorder.Stop();

//                                     recorder?.Release();
//                                     recorder = null;

//                                     var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
//                                     var filename = System.IO.Path.Combine(documents, write_sequence.ToString() + ".MP4");

//                                     if (File.Exists(filename) == false)
//                                         return;

//                                     NetProcess.SendRoomMessage(File.ReadAllBytes(filename));

//                                     write_sequence++;

//                                     CheckTime = DateTime.Now;



//                                 }
//                             }
//                             catch (Exception ex)
//                             {
//                                 Console.WriteLine(ex.Message);
//                             }

//                             CheckTime = DateTime.Now.AddSeconds(3);
//                         }
                        
//                    });

//                });


//                //System.Timers.Timer recordtimer = new System.Timers.Timer();
//                //recordtimer.Interval = 3000;
//                //recordtimer.Enabled = true;
//                //recordtimer.Elapsed += OnTimedEvent;

//                //void OnTimedEvent(object sender, System.Timers.ElapsedEventArgs e)
//                //{
//                //    try
//                //    {
//                //        if (isPlay == false)
//                //        {
//                //            isPlay = true;

//                //            StartRecord();
//                //        }
//                //        else
//                //        {

//                //            isPlay = false;

//                //            recorder.Stop();

//                //            recorder?.Release();
//                //            recorder = null;

//                //        }
//                //    }
//                //    catch (Exception ex)
//                //    {
//                //        Console.WriteLine(ex.Message);
//                //    }

//                //    if (isPlay == false)
//                //    {
//                //        var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
//                //        var filename = System.IO.Path.Combine(documents, write_sequence.ToString() + ".MP4");

//                //        if (File.Exists(filename) == false)
//                //            return;

//                //        NetProcess.SendRoomMessage(File.ReadAllBytes(filename));

//                //        write_sequence++;
//                //    }
//                //}

//            }

//            System.Timers.Timer timerRead = new System.Timers.Timer();
//            timerRead.Interval = 1000;
//            timerRead.Enabled = true;
//          //  timerRead.Elapsed += OntimerReadTimedEvent;



//            void OntimerReadTimedEvent(object sender, System.Timers.ElapsedEventArgs e)
//            {
//                try
//                {
//                    Activity.RunOnUiThread(() =>
//                    {
//                        if (once == true)
//                            return;

//                        if ((NetProcess.MP4Index) < (read_sequence+1))
//                        {
//                            once = false;
//                            return;
//                        }

//                        var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
//                        var filename = System.IO.Path.Combine(documents, "r" + read_sequence.ToString() + ".MP4");

//                        if (File.Exists(filename) == false)
//                        {
//                            return;
//                        }

//                        try
//                        {
//                            using (System.IO.Stream stream = new FileStream(filename, FileMode.Open))
//                            {
//                                // File/Stream manipulating code here
//                            }
//                        }
//                        catch
//                        {
//                            once = false;
//                            return;
//                        }

//                        once = true;

//                        var surfaceView = (SurfaceView)Activity.FindViewById(Resource.Id.surfaceView1);

//                        play?.Release();
//                        play = null;

//                        play = new MediaPlayer();
//                        play.Reset();
//                        play.SetDataSource(filename);
//                        play.SetDisplay(surfaceView.Holder);
//                        play.Prepare();

//                        play.Start();
//                        play.Completion += OnPlayer_Completion;
//                    });

//                }
//                catch (Exception ex)
//                {
//                    Console.WriteLine(ex.Message);
//                }
//            }

//        }

//        void OnPlayer_Completion(object sender, EventArgs e)
//        {
//            try
//            {
//                Activity.RunOnUiThread(() =>
//                {
                     
                    
//                      var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
//                      var filename = System.IO.Path.Combine(documents, "r" + read_sequence.ToString() + ".MP4");

//                      var surfaceView = (SurfaceView)Activity.FindViewById(Resource.Id.surfaceView1);

//                      if (File.Exists(filename) == false)
//                      {
//                          once = false;
//                          return;
//                      }

//                    if ((NetProcess.MP4Index) < (read_sequence+1))
//                    {
//                        once = false;
//                        return;
//                    }

//                      try
//                      {
//                          using (System.IO.Stream stream = new FileStream(filename, FileMode.Open))
//                          {
//                              // File/Stream manipulating code here
//                          }
//                      }
//                      catch
//                      {
//                        once = false;
//                        return;
//                      }

//                      play?.Stop();
//                      play?.Release();
//                      play?.Dispose();
//                      play = null;

//                      play = new MediaPlayer();
//                      play.Reset();

//                      var fis = new Java.IO.FileInputStream(new Java.IO.File(filename));

//                      play.SetDataSource(fis.FD);
//                      play.SetDisplay(surfaceView.Holder);
//                      play.Prepare(); 
//                        play.Prepared += (object snd, EventArgs evt) => {
//                            (snd as MediaPlayer).Start();
//                        };

//                    play.Completion += OnPlayer_Completion;

//                     read_sequence++;
                    
//                });

//            }
//            catch (Exception ex)
//            {
//                Console.WriteLine(ex.Message);
//            }
//        }

//        int read_sequence = 1;
//        int write_sequence = 1;
//        bool once = false;
//        MediaPlayer play;
//        MediaRecorder recorder;
//        public void StartRecord()
//        {
         

//            var video = Activity.FindViewById<VideoView>(Resource.Id.videoView1);

//            video.StopPlayback();

//            var documents = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
//            var filename = System.IO.Path.Combine(documents, write_sequence.ToString()+".MP4");

//            File.Delete(filename);

//            var cameraParameters = camera.GetParameters();
//            var supportedSizes = cameraParameters.SupportedVideoSizes;
//            var supportedPreviewSizes = cameraParameters.SupportedPreviewSizes;
//    //        cameraParameters.Zoom = 30;

//            camera.Unlock();

//            recorder?.Release();
//            recorder = null;

//            recorder = new MediaRecorder();
//            recorder.SetCamera(camera);

//            recorder.SetVideoSource(VideoSource.Camera);
//            recorder.SetAudioSource(AudioSource.Camcorder);
//            recorder.SetOutputFormat(OutputFormat.Mpeg4);

//            recorder.SetVideoSize(supportedSizes[0].Width, supportedSizes[0].Height);
        
//            recorder.SetVideoEncoder(VideoEncoder.Mpeg4Sp);
//            recorder.SetAudioEncoder(AudioEncoder.AmrNb);


//            // Change the stream to your stream of choice. 
          
         

//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_ALARM, true);
//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_DTMF, true);
//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_MUSIC, true);
//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_RING, true);
//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_SYSTEM, true);
//            //((AudioManager)activity.getApplicationContext().getSystemService(Context.AUDIO_SERVICE)).setStreamMute(AudioManager.STREAM_VOICE_CALL, true);

//            recorder.SetPreviewDisplay(video.Holder.Surface);

//            recorder.SetOutputFile(filename);
//            recorder.Prepare();
//            recorder.Start();
//        }


//        int cameraId;
//        Android.Hardware.Camera camera;
             
       
//        private bool OpenCamera(int id)
//        {
//            //Optns a camera and return true/false if it worked.
//            try
//            {
               
//                camera = Android.Hardware.Camera.Open(id);

//                cameraId = id;
//                camera.StartPreview();
      
//                return true;
//            }
//            catch (Exception ex)
//            {
//                return false;
//            }
//        }

//        byte[] ss = new byte[55555];

//        private void InitCamera()
//        {
//            //bool success = false;
//            //success = OpenCamera(0); //Try the face camera

//            //if (success == false)
//            //{
//            //    success = OpenCamera(1); //Try the rear camera
//            //}
//            //variable to get the number of cameras in the device
//            int cameraCount = Android.Hardware.Camera.NumberOfCameras;

//            Android.Hardware.Camera.CameraInfo cameraInfo = new Android.Hardware.Camera.CameraInfo();
//            for (int camIdx = 0; camIdx < cameraCount; camIdx++)
//            {
//                Android.Hardware.Camera.GetCameraInfo(camIdx, cameraInfo);
//                if (cameraInfo.Facing == Android.Hardware.CameraFacing.Front)
//                {
//                    camera = Android.Hardware.Camera.Open(camIdx);

//                    cameraId = camIdx;
//                    camera.SetDisplayOrientation(90);
//                }
//            }
//            //var video = Activity.FindViewById<VideoView>(Resource.Id.videoView1);
//            //camera.SetPreviewDisplay(video.Holder);
//            camera.AddCallbackBuffer(ss);
//            camera.SetPreviewCallbackWithBuffer(new mPreviewCallback());
//            camera.StartPreview();


//        }
//    }

//    public class mPreviewCallback : Java.Lang.Object, Android.Hardware.Camera.IPreviewCallback
//    {
//        public void OnPreviewFrame(byte[] data, Android.Hardware.Camera camera)
//        {
//            var paras = camera.GetParameters();
//            var imageformat = paras.PreviewFormat;
//            if (imageformat == Android.Graphics.ImageFormatType.Nv21)
//            {
//                Android.Graphics.YuvImage img = new Android.Graphics.YuvImage(data,
//                    imageformat, paras.PreviewSize.Width, paras.PreviewSize.Height, null);
//            }
//        }
//    }

//}