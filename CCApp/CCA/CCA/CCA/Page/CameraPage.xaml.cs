using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Rg.Plugins.Popup.Pages;
using Xamarin.Essentials;
using System.Threading;

namespace CCA.Page
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class CameraPage : PopupPage
    {

        public delegate void DoSomeDelegate(string parameter, Action<string> callback);
        public DoSomeDelegate OnControl;

        public CameraPage()
        {
            InitializeComponent();
        }

     

        public void ControlCamera(string command)
        {
            // Call the method registered in the custom renderer
            OnControl?.Invoke(command, (args) =>
            {
                // Acquire the value from custom renderer
            });
        }
    }
}