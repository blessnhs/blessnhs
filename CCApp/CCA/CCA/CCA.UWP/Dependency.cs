using System;

using DependencyHelper.UWP;
using Plugin.InAppBilling;

[assembly: Xamarin.Forms.Dependency(typeof(Toast_UWP))]

namespace DependencyHelper.UWP
{
    public class Toast_UWP : MethodExt
    {
        public void ShowToast(string message)
        {

        }

        public void Flash()
        {

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

        public void Logout()
        {
            
        }

        public void RestartApp()
        {

        }

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
