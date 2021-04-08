using WBA.MainTabbedPage;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Internals;
using Xamarin.Forms.Xaml;

namespace WBA.TestamentPage
{
    public enum BibleSplite
    {
        OldTestament,
        NewTestament
    }


    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class BibleList : ContentPage
    {
       
        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as BibleTableInfo;

            User.CacheData.BibleName = contact.Name;
       
            var parentPage = this.Parent as TabbedPage;

            int MaxChapterSize = BibleInfo.GetChapterSize(contact.Name);

            parentPage.Children[1] = new Chapter(MaxChapterSize);

            parentPage.CurrentPage = parentPage.Children[1];
        }
        public BibleList(BibleSplite _type)
        {
            InitializeComponent();

            var assembly = IntrospectionExtensions.GetTypeInfo(typeof(BibleList)).Assembly;

            List<BibleTableInfo> LoadInfo = new List<BibleTableInfo>();
            int idInc = 0;
            if (_type == BibleSplite.NewTestament)
            {
                foreach(var data in BibleInfo.ListNewTestament)
                {
                    BibleTableInfo info = new BibleTableInfo();

                    info.Id = idInc++;
                    info.Name = data;
                    info.MaxChapterSize = BibleInfo.GetChapterSize(data);

                    LoadInfo.Add(info);
                }
            }
            else
            {
                foreach (var data in BibleInfo.ListOldTestament)
                {
                    BibleTableInfo info = new BibleTableInfo();

                    info.Id = idInc++;
                    info.Name = data;
                    info.MaxChapterSize = BibleInfo.GetChapterSize(data);

                    LoadInfo.Add(info);
                }
            }

            listView.ItemsSource = LoadInfo;
        }
    }
}
