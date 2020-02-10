using System;
using System.Threading.Tasks;
using Android.OS;
using Android.App;
using Android.Content;
using Android.Util;
using Android.Widget;
using Android.Support.V4.App;
using Android.Graphics;
using WBA.Droid;
using System.Threading;
using Android.App.Job;
using Android.Content.PM;
using SQLite;
using System.Collections.Generic;
using WBA.MainTabbedPage;
using Plugin.LocalNotifications;
using WBA.MainTabbedPage.Droid;
//using Plugin.LocalNotifications;

namespace WBA.Droid
{
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
                        var notification = new Notification.Builder(this, "DE")
                        .SetContentTitle(DateTime.Now.ToString() + "알림!")
                        .SetContentText(data.Message)
                        .SetSmallIcon(Resource.Drawable.xamagonBlue)
                        .SetLargeIcon(BitmapFactory.DecodeResource(Resources, Resource.Drawable.xamagonBlue))
                        .SetSmallIcon(Resource.Drawable.xamagonBlue)
                        .Build();

                        manager.Notify(data.Id, notification);


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
    }

    public static class Constants
    {
        public const int DELAY_BETWEEN_LOG_MESSAGES = 5000; // milliseconds
        public const int SERVICE_RUNNING_NOTIFICATION_ID = 10000;
        public const string SERVICE_STARTED_KEY = "has_service_been_started";
        public const string BROADCAST_MESSAGE_KEY = "broadcast_message";
        public const string NOTIFICATION_BROADCAST_ACTION = "WBA.Notification.Action";

        public const string ACTION_START_SERVICE = "WBA.action.START_SERVICE";
        public const string ACTION_STOP_SERVICE =  "WBA.action.STOP_SERVICE";
        public const string ACTION_RESTART_TIMER = "WBA.action.RESTART_TIMER";
        public const string ACTION_MAIN_ACTIVITY = "WBA.action.MAIN_ACTIVITY";
    }

}

