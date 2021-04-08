using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class HymnViewer : ContentPage
    {
        private int pageindex = 1;

        protected override void OnAppearing()
        {

            base.OnAppearing(); // do the usual stuff OnAppearing does

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(HymnViewer)).Assembly;

            string page = string.Format("{0:000}", pageindex);

            string path = "WBA.Resource.Hymn." + page + ".gif";

            var stream = assembly.GetManifestResourceStream(path);

            if (stream != null)
            {
                Image1.Source = ImageSource.FromResource(path);

                Image2.Source = "";
            }
            else
            {
                string path1 = "WBA.Resource.Hymn." + page + "-1" + ".gif";

                Image1.Source = ImageSource.FromResource(path1);

                string path2 = "WBA.Resource.Hymn." + page + "-2" + ".gif";

                Image2.Source = ImageSource.FromResource(path2);

            }
        }

        public HymnViewer(int index)
        {
            InitializeComponent();

            pageindex = index;

          
        }
    }
}