using System;
using System.IO;
using Android.App;
using Android.Media;
using Android.OS;
using Android.Views;
using Android.Widget;
using DependencyHelper.Droid;
using Xamarin.Forms.Platform.Android;
using Android.Graphics;
using Android.Content;
using Android.Support.V4.App;
using AndroidApp = Android.App.Application;
using Android.Runtime;

[assembly: Xamarin.Forms.Dependency(typeof(Method_Android))]

namespace DependencyHelper.Droid
{
    public class Method_Android : MethodExt
    {
        public string MachineId()
        {
            try
            {
                return Android.Provider.Settings.Secure.GetString(Android.App.Application.Context.ContentResolver, Android.Provider.Settings.Secure.AndroidId);
            }
            catch(Exception e)
            {

            }

            return "";
        }
        public string MachineName()
        {
           

            return "";
        }

        public void Notification(string message)
        {
            try
            {
                var manager = (NotificationManager)AndroidApp.Context.GetSystemService(AndroidApp.NotificationService);

                if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
                {

                    var notification = new Notification.Builder(AndroidApp.Context, "DE")
                        .SetContentTitle(DateTime.Now.ToString() + " Notify!")
                        .SetContentText(message)
                        .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
                        .SetLargeIcon(BitmapFactory.DecodeResource(AndroidApp.Context.Resources, CCA.Droid.Resource.Drawable.xamagonBlue))
                        .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
                        .Build();

                    manager.Notify(1, notification);
                }
                else
                {
                    var notification = new Notification.Builder(Android.App.Application.Context)
                                                 .SetContentTitle(DateTime.Now.ToString() + " Notify!")
                                                 .SetContentText(message)
                                                 .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
                                                 .SetLargeIcon(BitmapFactory.DecodeResource(AndroidApp.Context.Resources, CCA.Droid.Resource.Drawable.xamagonBlue))
                                                 .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
                                                 .Build();

                    manager.Notify(1, notification);
                }
            }
            catch (Exception e)
            {

            }

        }
    }
}
