using System;
using System.Collections.Generic;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class PlanDetail : ContentPage
    {
        public PlanDetail()
        {
            InitializeComponent();
        }

        protected void OnItemSelected(object sender, SelectedItemChangedEventArgs e)
        {
            listView.ScrollTo(e.SelectedItem, ScrollToPosition.MakeVisible, true);
        }


        private List<BibleTableInfo> LoadInfo = new List<BibleTableInfo>();
        public void RefreshData()
        {
            int idInc = 0;

            if(LoadInfo.Count == 0)
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
            if(plan != null)
            {
                ReadChapterCount.Text = plan.Count.ToString();

                var search = LoadInfo.Find(e => e.Name == plan.BibleName);
                if (search != null)
                {
                    listView.SelectedItem = search;
                    listView.TabIndex = search.Id;
                }

                StartTime.Date = plan.StartTime;


                ShowInfomation(StartTime.Date,plan.Count);
            }
        }

        protected override void OnAppearing()
        {
            RefreshData();
        }

        private void ShowInfomation(DateTime StartTime,int Count)
        {
            //읽기 정보 출력
            int totalchapter = 0;
            var search = BibleInfo.List.Find(e => e.Name == SelBibleName);
            if (search == null)
                return ;

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

            Info.Text = "읽어야할 총 장수 " + totalchapter.ToString() + "\n 종료 날짜 " + endTime.ToLocalTime();
        }

        private string SelBibleName="";

        async void OnButtonClicked(object sender, EventArgs args)
        {
            if (SelBibleName == "")
            {
                await DisplayAlert("", "시작할 권을 먼저 선택해야 합니다.", "OK");
            }
            else
            {
                var CountText = ReadChapterCount.Text;

                if (CountText == null)
                    CountText = "3";

                if (Helper.IsNumber(CountText) == false)
                {
                    await DisplayAlert("", "장수는 숫자로만 입력하세요 ", "OK");
                    return;
                }

                int Count = Convert.ToInt16(CountText);

                if (Count > 100 || 0 >= Count)
                {
                    await DisplayAlert("", "권장 장수가 아닙니다.", "OK");
                    return;
                }

                bool answer = await DisplayAlert("안내", SelBibleName + "부터 " + "하루 " + CountText + "장씩 시작 하시겠 습니까?", "예", "아니요");
                if (answer == true)
                {
                    SQLLiteDB.InsertBibleReadPlan(StartTime.Date, SelBibleName, Count);

                    ShowInfomation(StartTime.Date,Count);
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