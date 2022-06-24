using System;

using DependencyHelper.UWP;

[assembly: Xamarin.Forms.Dependency(typeof(Toast_UWP))]

namespace DependencyHelper.UWP
{
    public class Toast_UWP : MethodExt
    {
        public string MachineId()
        {
            try
            {
                return "";
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
              
            }
            catch (Exception e)
            {

            }

        }
    }
}
