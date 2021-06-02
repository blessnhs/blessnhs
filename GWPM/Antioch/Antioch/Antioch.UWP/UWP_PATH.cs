using Antioch.UWP;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;

using DependencyHelper.Droid;
using System.IO;

[assembly: Xamarin.Forms.Dependency(typeof(UWP_PATH))]

namespace DependencyHelper.Droid
{
    public class UWP_PATH : FlatformPath
    {
        public string GetPath()
        {
            Windows.Storage.StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
      
            return localFolder.Path;
        }
    }
}
