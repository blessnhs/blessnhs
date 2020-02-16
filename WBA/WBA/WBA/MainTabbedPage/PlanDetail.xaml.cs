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

            List<BibleTableInfo> LoadInfo = new List<BibleTableInfo>();
            int idInc = 0;

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

            listView.ItemsSource = LoadInfo;
        }

        public bool IsNumber(string me)
        {
            foreach (char ch in me)
            {
                if (!Char.IsDigit(ch))
                    return false;
            }

            return true;
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

                if (IsNumber(CountText) == false)
                {
                    await DisplayAlert("", "장수는 숫자로만 입력하세요 ", "OK");
                    return;
                }

                int Count = Convert.ToInt16(CountText);

                if(Count > 100)
                {
                    await DisplayAlert("", "권장 장수가 아닙니다.", "OK");
                    return;
                }
              
                bool answer = await DisplayAlert("안내", SelBibleName + "부터 " + "하루 " + CountText + "씩 시작하겠습니까?", "예", "아니요");
                if(answer == true)
                {
                    SQLLiteDB.InsertBibleReadPlan(StartTime.Date, SelBibleName, Count);
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