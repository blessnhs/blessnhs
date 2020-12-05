using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Threading;

using Android;
using Android.App;
using Android.Content.PM;
using Android.Media;
using Android.OS;
using Android.Runtime;
using Android.Support.Design.Widget;
using Android.Support.V4.Content;
using Android.Support.V7.App;
using Android.Views;
using Android.Widget;
using OMOK.Network;

namespace OMOK.Droid
{
    static public class AudioManagerM
    {
        static int sampleRate = 8000;
        static ChannelOut channelOut = ChannelOut.Mono;
        static ChannelIn channelIn = ChannelIn.Mono;
        static Encoding encoding = Encoding.Pcm16bit;


        static AudioRecord recorder;
        static AudioTrack audioTrack;

        static public void Clear()
        {
            recorder?.Stop();
            recorder?.Release();
            recorder?.Dispose();
            recorder = null;

            audioTrack?.Stop();
            audioTrack?.Release();
            audioTrack?.Dispose();
            audioTrack = null;
        }

        static bool completefile = false;

        static public void record()
        {
            try
            {
                InitAudioTrack();

                recorder?.Stop();
                recorder?.Release();
                recorder?.Dispose();
                recorder = null;


                int minBufSize = AudioTrack.GetMinBufferSize(sampleRate, channelOut, encoding);
                //       DatagramSocket socket = new DatagramSocket();
                byte[] buffer = new byte[minBufSize];
                //     DatagramPacket packet;
                //     InetAddress destination = InetAddress.GetByName(serverAddress);
                recorder = new AudioRecord(AudioSource.Mic, sampleRate, channelIn, encoding, minBufSize*4);

                if (recorder.State != Android.Media.State.Initialized)
                {
                    return;
                }

                string Path = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "temp.pcm");

                System.IO.File.Delete(Path);

                recorder.StartRecording();

                DateTime currentTime = DateTime.Now.AddMilliseconds(500);

                FileStream fs = new FileStream(Path, FileMode.Append, FileAccess.Write);

                //while (recorder.RecordingState == RecordState.Recording)
                //{
                //    minBufSize = recorder.Read(buffer, 0, buffer.Length);

                //    if (completefile == true)
                //        continue;

                //    fs.Write(buffer, 0, buffer.Length);

                //    if (currentTime < DateTime.Now)
                //    {
                //        fs.Close();

                //        play(File.ReadAllBytes(Path));
                //    //    NetProcess.SendAudioMessage(File.ReadAllBytes(Path));

                //        File.Delete(Path);
                //        var r = File.Exists(Path);

                //        fs = new FileStream(Path, FileMode.Append, FileAccess.Write);

                //        currentTime = DateTime.Now.AddMilliseconds(500);

                //    }
                //}

                while (recorder.RecordingState == RecordState.Recording)
                {
                    minBufSize = recorder.Read(buffer, 0, buffer.Length);

                    NetProcess.SendAudioMessage(buffer);
                }



            }
            catch (Exception e)
            {
            }
            finally
            {

                recorder.Stop();
                recorder.Release();
                recorder.Dispose();
                recorder = null;
            }
        }

        static public void InitAudioTrack()
        {
            int minBufSize = AudioTrack.GetMinBufferSize(sampleRate, channelOut, encoding);
            audioTrack = new AudioTrack(
                                 Android.Media.Stream.Music,
                                  // Frequency
                                  sampleRate,
                                  // Mono or stereo
                                  channelOut,
                                  // Audio encoding
                                  encoding,
                                   // Length of the audio clip.
                                   minBufSize * 4,
                                  // Mode. Stream or static.
                                  AudioTrackMode.Stream);
        }

        static int overallBytes = 0;

        static public async void play(byte[] data)
        {
  
            if (audioTrack.State != AudioTrackState.Initialized)
                return;

            int minBufSize = AudioTrack.GetMinBufferSize(sampleRate, channelOut, encoding);


            int bytes = audioTrack.Write(data, 0, data.Length);

            overallBytes += bytes;

            if(overallBytes >= minBufSize*4)
            {
                overallBytes = 0;
                audioTrack.Play();
            }

          

        }
    }
}