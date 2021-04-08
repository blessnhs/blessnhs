using System;
using Android.OS;
using Android.App;
using Android.Content;
using Android.Graphics;
using WBA.MainTabbedPage.Droid;
//using Plugin.LocalNotifications;

namespace WBA.Droid
{
    public static class Constants
    {
        public const int SERVICE_RUNNING_NOTIFICATION_ID = 10000;
        public const string ACTION_START_SERVICE = "WBA.action.START_SERVICE";
    }

    [Service]
    public class BackEndService : Service
    {
        static readonly string TAG = typeof(BackEndService).FullName;

        bool isStarted;
   
        public override void OnCreate()
        {
            base.OnCreate();
        }

        public override StartCommandResult OnStartCommand(Intent intent, StartCommandFlags flags, int startId)
        {
            if (isStarted)
            {
                foreach (var data in SQLLiteDB.ReadUserScheduleData())
                {
                    if (data.IsNotify == false && data.Time < DateTime.Now.AddMinutes(15))
                    {
                     //   CrossLocalNotifications.Current.Show(data.Message, DateTime.Now.ToString(), data.Id, DateTime.Now);

                        var manager = (NotificationManager)GetSystemService(NotificationService);

                        if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
                        {

                            var notification = new Notification.Builder(this, "DE")
                                .SetContentTitle(DateTime.Now.ToString() + "알림!")
                                .SetContentText(data.Message)
                                .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                .SetLargeIcon(BitmapFactory.DecodeResource(Resources, Resource.Drawable.xamagonBlue))
                                .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                .Build();

                            manager.Notify(data.Id, notification);
                        }
                        else
                        {
                            var notification = new Notification.Builder(this)
                                                         .SetContentTitle(DateTime.Now.ToString() + "알림!")
                                                         .SetContentText(data.Message)
                                                         .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                                         .SetLargeIcon(BitmapFactory.DecodeResource(Resources, Resource.Drawable.xamagonBlue))
                                                         .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                                         .Build();

                            manager.Notify(data.Id, notification);
                        }

                            //Notify update
                        data.IsNotify = true;
                        SQLLiteDB.Upsert(data, false);
                    }
                }

            }
            else
            {
                RegisterForegroundService();
                isStarted = true;
            }

            SQLLiteDB.InsertScheduleLog(DateTime.Now);

            AlarmReceiver.AddAlarmEvent(10);
            
            return StartCommandResult.Sticky;
        }


        public override IBinder OnBind(Intent intent)
        {
            // Return null because this is a pure started service. A hybrid service would return a binder that would
            // allow access to the GetFormattedStamp() method.
            return null;
        }
        public override void OnTaskRemoved(Intent rootIntent)
        {
          //  AlarmReceiver.AddAlarmEvent(10);

            base.OnTaskRemoved(rootIntent);
        }

        public override void OnDestroy()
        {
            // Remove the notification from the status bar.
            var notificationManager = (NotificationManager)GetSystemService(NotificationService);
            notificationManager.Cancel(Constants.SERVICE_RUNNING_NOTIFICATION_ID);

            isStarted = false;

          //  AlarmReceiver.AddAlarmEvent(10);

            base.OnDestroy();
        }

        void RegisterForegroundService()
        {
            NotificationManager Manager = (NotificationManager)GetSystemService(NotificationService);

            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {

                var chan1 = new NotificationChannel("DE",
                    "DE", NotificationImportance.Default);
                chan1.LockscreenVisibility = NotificationVisibility.Private;
                Manager.CreateNotificationChannel(chan1);

                var notification = new Notification.Builder(this, "DE")
                .SetContentTitle("성경읽기 실행중")
                .SetContentText("성경읽기")
                .SetSmallIcon(Resource.Drawable.xamagonBlue)
                .Build();


                StartForeground(Constants.SERVICE_RUNNING_NOTIFICATION_ID, notification);
            }
            else
            {
                var notification = new Notification.Builder(this)
                           .SetContentTitle("성경읽기 실행중")
                           .SetContentText("성경읽기")
                           .SetSmallIcon(Resource.Drawable.xamagonBlue)
                           .Build();


                StartForeground(Constants.SERVICE_RUNNING_NOTIFICATION_ID, notification);
            }
        }
    }
}

