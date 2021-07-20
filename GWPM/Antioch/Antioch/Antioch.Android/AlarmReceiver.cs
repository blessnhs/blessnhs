using System;
using Android.App;
using Android.Content;
using Android.OS;
using Antioch.Droid;


namespace Antioch.Droid
{
    [BroadcastReceiver(Enabled = true)]
 //   [IntentFilter(new string[] { "android.intent.action.BOOT_COMPLETED" }, Priority = (int)IntentFilterPriority.LowPriority)]
    class AlarmReceiver : BroadcastReceiver
    {
        public override void OnReceive(Context context, Intent intent_)
        {
            Intent startService = new Intent(context, typeof(BackEndService));
            startService.SetAction(Constants.ACTION_START_SERVICE);

            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {
                context.StartForegroundService(startService);
            }
            else
            {
                context.StartService(startService);
            }
        }

        static private int AlarmId = 0;
        static public void AddAlarmEvent(int seconds = 60)
        {
            DateTime alarmtime = DateTime.Now.AddSeconds(seconds);

            Intent alarmIntent = new Intent(Android.App.Application.Context, typeof(AlarmReceiver));

            PendingIntent pendingIntent = PendingIntent.GetBroadcast(Android.App.Application.Context, AlarmId++, alarmIntent, PendingIntentFlags.UpdateCurrent);
            AlarmManager alarmManager = (AlarmManager)Android.App.Application.Context.GetSystemService(Context.AlarmService);

            Java.Util.Calendar calendar = Java.Util.Calendar.Instance;
            calendar.TimeInMillis = Java.Lang.JavaSystem.CurrentTimeMillis();
            calendar.Set(alarmtime.Year, alarmtime.Month - 1, alarmtime.Day, alarmtime.Hour, alarmtime.Minute, alarmtime.Second);

            if (Build.VERSION.SdkInt < Android.OS.BuildVersionCodes.M)
            {
                if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.Kitkat)
                {
                    //API 19 이상 API 23미만
                    alarmManager.SetExact(AlarmType.RtcWakeup, calendar.TimeInMillis, pendingIntent);
                }
                else
                {
                    //API 19미만
                    alarmManager.Set(AlarmType.RtcWakeup, calendar.TimeInMillis, pendingIntent);
                }
            }
            else
            {
                //API 23 이상
                alarmManager.SetExactAndAllowWhileIdle(AlarmType.RtcWakeup, calendar.TimeInMillis, pendingIntent);
            }

        }
    }
}