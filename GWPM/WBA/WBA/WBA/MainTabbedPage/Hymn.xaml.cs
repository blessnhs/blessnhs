using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{


    //성경의 타이틀 정보
    public class HymnTableInfo
    {
        public int Id { get; set; }
        public string Name { get; set; }

        public string Display
        {
            get => Id + "장 " + Name;
        }
    };

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Hymn : ContentPage
	{
        public static int StartNumber = 1, EndNumber = 588;

        //문자열 인덱스 : "123" 해당 2장존재하는 데이터는 없음 (123장-1,123장-2) 존재하지 않음
        public static Dictionary<string, string> DivideSong = new Dictionary<string, string>();
        public static Dictionary<int, string> DivideSongTitle = new Dictionary<int, string>();

        public static bool LoadList()
        {
            //이미 불러옴
            if (DivideSong.Count > 0)
                return true;

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(Hymn)).Assembly;

            //여기서 부터는 통일 찬송가 목록을 불러옴
            {
                assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleInfo)).Assembly;
                Stream stream = assembly.GetManifestResourceStream("WBA.Resource.HymnList.txt");
                //1Chr.14:5 And Ibhar, and Elishua, and Elpalet,
                using (var reader = new System.IO.StreamReader(stream))
                {
                    string text;
                    while ((text = reader.ReadLine()) != null)
                    {
                        try
                        {
                            if (text == "" || text == null)
                                continue;

                            string[] Header = text.Split(':');

                            if (Header == null || Header.Count() == 0)
                                continue;

                            string Title = Header[0].TrimEnd();
                            int index = Convert.ToInt32(Header[1].TrimEnd());

                            DivideSongTitle[index] = Title;
                            DivideSong[Header[1]] = Title;

                        }
                        catch (Exception e)
                        {

                        }

                    }
                }
            }

            return true;
        }

        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as HymnTableInfo;

            NavigationPage.SetHasNavigationBar(this, true);
            NavigationPage.SetHasBackButton(this, true);

            Navigation.PushModalAsync(new HymnViewer(contact.Id));
        }
        void Entry_TextChanged(object sender, TextChangedEventArgs e)
        {
            var oldText = e.OldTextValue;
            var newText = e.NewTextValue;

            List<HymnTableInfo> hymnList = new List<HymnTableInfo>();
     
            foreach (var page in DivideSong)
            {
                if(page.Key.IndexOf(newText) != -1)
                {
                    HymnTableInfo table = new HymnTableInfo();
                    table.Id = Convert.ToInt16(page.Key);
                    table.Name = page.Value;

                    hymnList.Add(table);
                    continue;
                }

                if (page.Value.IndexOf(newText) != -1)
                {
                    HymnTableInfo table = new HymnTableInfo();
                    table.Id = Convert.ToInt16(page.Key);
                    table.Name = page.Value;

                    hymnList.Add(table);
                    continue;
                }
            }

            listView.ItemsSource = hymnList;
        }

        public Hymn()
        {
            InitializeComponent();

            List<HymnTableInfo> hymnList = new List<HymnTableInfo>();

            foreach (var page in DivideSong)
            {
                HymnTableInfo table = new HymnTableInfo();
                table.Id = Convert.ToInt16(page.Key);
                table.Name = page.Value;

                hymnList.Add(table);
            }

            listView.ItemsSource = hymnList;

        }
	}
}