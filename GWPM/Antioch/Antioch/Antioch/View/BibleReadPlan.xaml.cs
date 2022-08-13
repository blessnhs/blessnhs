using Antioch.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
     public partial class BibleReadPlan : ContentView
    {
        public BibleReadPlan()
        {
            InitializeComponent();

            RefreshData();
        }

        protected void OnItemSelected(object sender, SelectedItemChangedEventArgs e)
        {
            listView.ScrollTo(e.SelectedItem, ScrollToPosition.MakeVisible, true);
        }


        private List<BibleTableInfo> LoadInfo = new List<BibleTableInfo>();
        public void RefreshData()
        {
            int idInc = 0;

            if (LoadInfo.Count == 0)
            {
                foreach (var data in BibleInfo.ListOldTestament)
                {
                    BibleTableInfo info = new BibleTableInfo();

                    info.Id = idInc++;
                    info.Name = data;
                    info.MaxChapterSize = BibleInfo.GetChapterSize(data);

                    LoadInfo.Add(info);
                }

                foreach (var data in BibleInfo.ListNewTestament)
                {
                    BibleTableInfo info = new BibleTableInfo();

                    info.Id = idInc++;
                    info.Name = data;
                    info.MaxChapterSize = BibleInfo.GetChapterSize(data);

                    LoadInfo.Add(info);
                }
            }

            listView.ItemsSource = LoadInfo;

            listView.ItemSelected += OnItemSelected;

            var plan = SQLLiteDB.ReadBibleReadPlan();
            if (plan != null)
            {
                ReadChapterCount.Text = plan.Count.ToString();

                BeginChapter.Text = plan.BeginChapter.ToString();

                var search = LoadInfo.Find(e => e.Name == plan.BibleName);
                if (search != null)
                {
                    listView.SelectedItem = search;
                    listView.TabIndex = search.Id;
                }

                StartTime.Date = plan.StartTime;


                ShowInfomation(StartTime.Date, plan.Count);
            }
        }

        private void ShowInfomation(DateTime StartTime, int Count)
        {
            //읽기 정보 출력
            int totalchapter = 0;
            var search = BibleInfo.List.Find(e => e.Name == SelBibleName);
            if (search == null)
                return;

            int accChapterSize = 0;
            foreach (var bible in BibleInfo.List)
            {
                if (search.Id > bible.Id)
                    continue;

                totalchapter += bible.MaxChapterSize;
            }

            int enddays = totalchapter / Count;

            int remainTime = totalchapter % Count;

            if (remainTime > 0)
                enddays += 1;

            DateTime endTime = StartTime.AddDays(enddays);

            Info.Text = "Read Chapter Count " + totalchapter.ToString() + "\n End Date " + endTime.ToLocalTime();
        }

        private string SelBibleName = "";

        async void OnButtonClicked(object sender, EventArgs args)
        {
            if (SelBibleName == "")
            {
                await App.Current.MainPage.DisplayAlert("", "You must choose a testament.", "OK");
            }
            else
            {
                var searchBible = BibleInfo.List.Find(e => e.Name == SelBibleName);
                if (searchBible == null)
                    return;

                var CountText = ReadChapterCount.Text;

                if (CountText == null)
                    CountText = "3";

                if (Helper.IsNumber(CountText) == false)
                {
                    await App.Current.MainPage.DisplayAlert("", "Please write number ", "OK");
                    return;
                }

                int Count = Convert.ToInt16(CountText);

                if (Count > 100 || 0 >= Count)
                {
                    await App.Current.MainPage.DisplayAlert("", "Please write number.", "OK");
                    return;
                }


                var ChapterStart = BeginChapter.Text;
                if (Helper.IsNumber(ChapterStart) == false)
                {
                    await App.Current.MainPage.DisplayAlert("", "Please write begin chapter number ", "OK");
                    return;
                }

                int Chapter_Start = Convert.ToInt16(ChapterStart);

                if (searchBible.MaxChapterSize < Chapter_Start || 0 >= Chapter_Start)
                {
                    await App.Current.MainPage.DisplayAlert("", "Please write chapter number Range.", "OK");
                    return;
                }

                int Chapter  = Convert.ToInt16(CountText);

                bool answer = await App.Current.MainPage.DisplayAlert("info", SelBibleName + " from " + "each day " + CountText  , "yes", "no");
                if (answer == true)
                {
                    SQLLiteDB.InsertBibleReadPlan(StartTime.Date, SelBibleName, Count, Chapter_Start);

                    ShowInfomation(StartTime.Date, Count);
                }

            }
        }
        void Handle_ItemSelected(object sender, Xamarin.Forms.SelectedItemChangedEventArgs e)
        {
            //선택된 아이템을 Contact 타입으로 변환
            var contact = e.SelectedItem as BibleTableInfo;

            SelBibleName = contact.Name;

        }
    }

}