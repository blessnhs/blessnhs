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
	public partial class Hymn : ContentPage
	{
        public static int StartNumber = 1, EndNumber = 588;
        public static Dictionary<string, int> DivideSong = new Dictionary<string, int>();
        public static bool LoadList()
        {
            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(Hymn)).Assembly;
            var list = assembly.GetManifestResourceNames().Where(r => r.StartsWith("WBA.Resource.Hymn") /*&& r.EndsWith(".txt")*/).ToArray();

            foreach (var song in list)
            {
                if(song.IndexOf('-') != -1)
                {
                    string[] Header = song.Split('-');

                    if(Header.Length > 0)
                    {
                        string[] Index = Header[0].Split('.');
                        if (Index.Length > 0)
                        {
                            string Number = Index[Index.Length - 1];
                            DivideSong[Number] = Index.Length;
                        }
                    }
                 
                }
            }

            return true;
        }

        async void OnSearchButtonClicked(object sender, EventArgs e)
        {

            if (Helper.IsNumber(IndexEntry.Text) == false)
            {
                IndexEntry.Text = "";
            }
            else
            {
                int idx = Convert.ToInt16(IndexEntry.Text);
                string number = string.Format("{0:000}", idx);

                if(DivideSong.ContainsKey(number) == false)
                {
                    string path = "WBA.Resource.Hymn." + number + ".gif";

                    Image1.Source = ImageSource.FromResource(path);

                    IndexEntry.Text = "";

                    Image2.Source = "";
                }
                else
                {
                    string path1 = "WBA.Resource.Hymn." + number +"-1"+ ".gif";

                    Image1.Source = ImageSource.FromResource(path1);

                    IndexEntry.Text = "";

                    string path2 = "WBA.Resource.Hymn." + number + "-2" + ".gif";

                    Image2.Source = ImageSource.FromResource(path2);
                    
                }

         
            }
        }

        public Hymn()
        {
            InitializeComponent();

        }
	}
}