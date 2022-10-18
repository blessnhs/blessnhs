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
using CCA.Droid;
using Plugin.InAppBilling;
using Xamarin.Essentials;
using CCA;

[assembly: Xamarin.Forms.Dependency(typeof(Method_Android))]

namespace DependencyHelper.Droid
{
    public class Method_Android : MethodExt
    {
        async public void Flash()
        {
            try
            {
                // Turn On
                await Flashlight.TurnOnAsync();

                // Turn Off
                await Flashlight.TurnOffAsync();
            }
            catch (FeatureNotSupportedException fnsEx)
            {
                // Handle not supported on device exception
            }
            catch (PermissionException pEx)
            {
                // Handle permission exception
            }
            catch (Exception ex)
            {
                // Unable to turn on/off flashlight
            }
        }


        public void ShowToast(string message)
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                var toast = Android.Widget.Toast.MakeText(Android.App.Application.Context, message, ToastLength.Short);
                toast.SetGravity(GravityFlags.Center | GravityFlags.Center, 0, 5);
                toast.Show();
            });
        }

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

        public void Logout()
        {
            MainActivity.activity.GoolgeLogout();
        }

        public async void Purchase(string purchaseid)
        {
            try
            {
                await CrossInAppBilling.Current.ConnectAsync();

                var purchase = await CrossInAppBilling.Current.PurchaseAsync(purchaseid, ItemType.InAppPurchase);
                if (purchase == null)
                {
                    //Not purchased, alert the user
                }
                else
                {
                    //Purchased, save this information
                    var id = purchase.Id;
                    var token = purchase.PurchaseToken;
                    var state = purchase.State;
                }

            }
            catch (Exception ex)
            {
                //Something bad has occurred, alert user
            }
            finally
            {
                //Disconnect, it is okay if we never connected
                CrossInAppBilling.Current.DisconnectAsync();
            }
        }

        public void RestartApp()
        {
            var intent = new Intent(MainActivity.activity, typeof(MainActivity));
            intent.PutExtra("crash", true);
            intent.AddFlags(ActivityFlags.ClearTop | ActivityFlags.ClearTask | ActivityFlags.NewTask);

            var pendingIntent = PendingIntent.GetActivity(MainActivity.context, 32514, intent, PendingIntentFlags.OneShot);

            MainActivity.activity.Finish();
            MainActivity.context.StartActivity(intent);
            
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


        static public void NotificationException(string message)
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
            finally
            {
                NetProcess.SendQNS(message);
            }

        }
    }
}
