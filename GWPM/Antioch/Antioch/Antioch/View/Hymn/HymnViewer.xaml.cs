using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class HymnViewer : ContentPage
    {
        private int pageindex = 1;

        public HymnViewer(int index)
        {
            InitializeComponent();

            pageindex = index;


            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(HymnViewer)).Assembly;

            string page = string.Format("{0:000}", pageindex);

            string path = "Antioch.Resource.Hymn." + page + ".gif";

            var stream = assembly.GetManifestResourceStream(path);

            if (stream != null)
            {
                if (Device.RuntimePlatform == Device.UWP)
                {
                    Image1.Source = ImageSource.FromResource(path, Assembly.GetExecutingAssembly());

                    Image2.Source = "";
                }
                else 
                {
                    Image1.Source = ImageSource.FromResource(path, Assembly.GetExecutingAssembly());

                    Image2.Source = "";
                }
            }
            else
            {
                string path1 = "Antioch.Resource.Hymn." + page + "-1" + ".gif";

                Image1.Source = ImageSource.FromResource(path1, Assembly.GetExecutingAssembly());

                string path2 = "Antioch.Resource.Hymn." + page + "-2" + ".gif";

                Image2.Source = ImageSource.FromResource(path2, Assembly.GetExecutingAssembly());

            }


        }
    }
}