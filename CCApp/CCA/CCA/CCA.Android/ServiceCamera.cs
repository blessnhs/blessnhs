using Android.App;
using Android.Content;
using Android.Graphics;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using DependencyHelper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xamarin.Forms;
using AndroidApp = Android.App.Application;


namespace CCA.Droid
{
	public static class Constants
	{
		public const int DELAY_BETWEEN_LOG_MESSAGES = 5000; // milliseconds
		public const int SERVICE_RUNNING_NOTIFICATION_ID = 10000;
		public const string SERVICE_STARTED_KEY = "has_service_been_started";
		public const string BROADCAST_MESSAGE_KEY = "broadcast_message";
		public const string NOTIFICATION_BROADCAST_ACTION = "ServicesDemo3.Notification.Action";

		public const string ACTION_START_SERVICE = "ServicesDemo3.action.START_SERVICE";
		public const string ACTION_STOP_SERVICE = "ServicesDemo3.action.STOP_SERVICE";
		public const string ACTION_RESTART_TIMER = "ServicesDemo3.action.RESTART_TIMER";
		public const string ACTION_MAIN_ACTIVITY = "ServicesDemo3.action.MAIN_ACTIVITY";
	}

	[Service]
	public class ServiceCamera : Service
	{
		static readonly string TAG = typeof(ServiceCamera).FullName;

		bool isStarted;
		Handler handler;
		Action runnable;

		private void NetworkProcess()
		{
			//network
			{

				//network thread
				Task.Run(() =>
				{
					while (true)
					{
						try
						{

							NetProcess.start();
							NetProcess.client.PacketRecvSync();
							NetProcess.Loop();
						}
						catch (Exception e)
						{
							DependencyService.Get<MethodExt>().Notification(e.Message);
						}

						Thread.Sleep(10);
					}
				});
			}
		}

		public override void OnCreate()
		{
			base.OnCreate();
			handler = new Handler();

			// This Action is only for demonstration purposes.
			runnable = new Action(() =>
			{
			});
		}

		public override StartCommandResult OnStartCommand(Intent intent, StartCommandFlags flags, int startId)
		{
			if (intent.Action.Equals(Constants.ACTION_START_SERVICE))
			{
				if (isStarted)
				{
					//Log.Info(TAG, "OnStartCommand: The service is already running.");
				}
				else
				{
					NetworkProcess();

					RegisterForegroundService();
			
					isStarted = true;
				}
			}
			else if (intent.Action.Equals(Constants.ACTION_STOP_SERVICE))
			{
			
				StopForeground(true);
				StopSelf();
				isStarted = false;

			}
			else if (intent.Action.Equals(Constants.ACTION_RESTART_TIMER))
			{
			}

			// This tells Android not to restart the service if it is killed to reclaim resources.
			return StartCommandResult.Sticky;
		}


		public override IBinder OnBind(Intent intent)
		{
			// Return null because this is a pure started service. A hybrid service would return a binder that would
			// allow access to the GetFormattedStamp() method.
			return null;
		}


		public override void OnDestroy()
		{
			isStarted = false;
			base.OnDestroy();
		}

		/// <summary>
		/// This method will return a formatted timestamp to the client.
		/// </summary>
		/// <returns>A string that details what time the service started and how long it has been running.</returns>
		

		void RegisterForegroundService()
		{
			var manager = (NotificationManager)AndroidApp.Context.GetSystemService(AndroidApp.NotificationService);

			Notification notification;

			if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
			{
				var chan1 = new NotificationChannel("DE",
				 "DE", NotificationImportance.Default);
				chan1.LockscreenVisibility = NotificationVisibility.Private;
				manager.CreateNotificationChannel(chan1);

				notification = new Notification.Builder(AndroidApp.Context, "DE")
					.SetContentTitle(DateTime.Now.ToString() + " Notify!")
					.SetContentText("message")
					.SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
					.SetLargeIcon(BitmapFactory.DecodeResource(AndroidApp.Context.Resources, CCA.Droid.Resource.Drawable.xamagonBlue))
					.SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
					.Build();

				manager.Notify(1, notification);
			}
			else
			{
				notification = new Notification.Builder(Android.App.Application.Context)
											 .SetContentTitle(DateTime.Now.ToString() + " Notify!")
											 .SetContentText("message")
											 .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
											 .SetLargeIcon(BitmapFactory.DecodeResource(AndroidApp.Context.Resources, CCA.Droid.Resource.Drawable.xamagonBlue))
											 .SetSmallIcon(CCA.Droid.Resource.Drawable.xamagonBlue)
											 .Build();

				manager.Notify(1, notification);
			}

			// Enlist this instance of the service as a foreground service
			StartForeground(Constants.SERVICE_RUNNING_NOTIFICATION_ID, notification);
		}

	}
}