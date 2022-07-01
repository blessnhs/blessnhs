using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;
using Xamarin.Forms.PlatformConfiguration;

namespace ToastMessage
{
    public interface Toast
    {
        void Show(string message);

        void StartRecord(object view);

    }
}