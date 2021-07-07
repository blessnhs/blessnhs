using System;
using Android.OS;
using Android.App;
using Android.Content;
using Android.Graphics;
using System.Threading.Tasks;
using System.Threading;

namespace Antioch.Droid
{
    public static class Constants
    {
        public const int SERVICE_RUNNING_NOTIFICATION_ID = 10000;
        public const string ACTION_START_SERVICE = "Antioch.action.START_SERVICE";
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

        private void NetworkProcess()
        {
            //network
            {

                //network thread
                Task.Run(() =>
                {
                    while (true)
                    {
                        NetProcess.start();
                        NetProcess.client.PacketRecvSync();
                        Thread.Sleep(1);
                    }
                });

                //network thread
                Task.Run(() =>
                {
                    DateTime checktime = DateTime.Now;

                    while (true)
                    {
                        NetProcess.Loop();
                        Thread.Sleep(1);
                    }
                });

            }
        }

        public override StartCommandResult OnStartCommand(Intent intent, StartCommandFlags flags, int startId)
        {
            if (isStarted)
            {
                NetworkProcess();
                {
                    var manager = (NotificationManager)GetSystemService(NotificationService);

                    if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
                    {

                        var notification = new Notification.Builder(this, "DE")
                            .SetContentTitle(DateTime.Now.ToString() + "알림!")
                            .SetContentText("새로운 공지가 등록되었습니다.")
                            .SetSmallIcon(Resource.Drawable.xamagonBlue)
                            .SetLargeIcon(BitmapFactory.DecodeResource(Resources, Resource.Drawable.xamagonBlue))
                            .SetSmallIcon(Resource.Drawable.xamagonBlue)
                            .Build();

                        manager.Notify(1, notification);
                    }
                    else
                    {
                        var notification = new Notification.Builder(this)
                                                     .SetContentTitle(DateTime.Now.ToString() + "알림!")
                                                     .SetContentText("새로운 공지가 등록되었습니다.")
                                                     .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                                     .SetLargeIcon(BitmapFactory.DecodeResource(Resources, Resource.Drawable.xamagonBlue))
                                                     .SetSmallIcon(Resource.Drawable.xamagonBlue)
                                                     .Build();

                        manager.Notify(1, notification);
                    }
                }
            }
            else
            {
                RegisterForegroundService();
                isStarted = true;
            }

            AlarmReceiver.AddAlarmEvent(5);

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

