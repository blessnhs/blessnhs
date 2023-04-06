using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


using FFMediaToolkit;
using FFMediaToolkit.Encoding;
using FFMediaToolkit.Graphics;

namespace CCA.Droid
{
    public class ImageConvert
    {
        private bool ffmpegInit = false;
        private MediaOutput mediaFile = null;

        /// <summary>
        /// Start Record video
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public void StartRecordVideo(string path)
        {
            try
            {
                if (mediaFile == null)
                {
                    if (!ffmpegInit)
                    {
                        FFmpegLoader.FFmpegPath = "./";
                        ffmpegInit = true;
                    }

                    int width = 1024;
                    int heigth = 1024;

                    VideoEncoderSettings settings = new VideoEncoderSettings(width, heigth, 30, VideoCodec.MPEG2);
                    settings.EncoderPreset = EncoderPreset.Medium;

                    mediaFile = MediaBuilder.CreateContainer(path).WithVideo(settings).Create();
                }
            }
            catch(Exception e)
            {

            }
        }

        /// <summary>
        /// Add RecordFrame
        /// </summary>
        /// <param name="bitmap"></param>
        public void AddRecordFrame(System.Drawing.Bitmap bitmap)
        {
            if (bitmap == null)
                return;

            if (mediaFile == null)
                return;

            System.Drawing.Imaging.BitmapData bdata = bitmap.LockBits(new System.Drawing.Rectangle(System.Drawing.Point.Empty, bitmap.Size), System.Drawing.Imaging.ImageLockMode.WriteOnly, bitmap.PixelFormat);
            ImageData imgdata = ImageData.FromPointer(bdata.Scan0, ImagePixelFormat.Bgra32, bitmap.Size);
            mediaFile.Video.AddFrame(imgdata);
            bitmap.UnlockBits(bdata);
        }

        /// <summary>
        /// Stop Record Video
        /// </summary>
        public void StopRecordVideo()
        {
            if (mediaFile != null)
            {
                mediaFile.Video.Dispose();
                mediaFile.Dispose();
                mediaFile = null;
            }
        }
    }
}