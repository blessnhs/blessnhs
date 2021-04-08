using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace WBA.MainTabbedPage
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Planxaml : ContentPage
    {
        private readonly ObservableCollection<string> _rangeCollection = new ObservableCollection<string>();
        private bool _shouldShowRange;

        public Planxaml()
        {
            InitializeComponent();

            UpdateCalendar(ViewModel.CurrentDateTime);

            RangeFrom.MinimumDate = DateTime.Today;
            RangeFrom.Date = DateTime.Today;

            RangeUntil.MinimumDate = RangeFrom.Date;
            RangeUntil.Date = RangeFrom.Date.AddDays(7);

            timePicker1.Time = new TimeSpan(12, 0, 0);
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();
            ViewModel.PropertyChanged += OnPropertyChanged;
            ViewModel.DidSelectRangeClicked += OnSelectRangeClicked;

            RangeFrom.DateSelected += OnFromRangeSelected;
            RangeUntil.DateSelected += OnUntilRangeSelected;
        }

        protected override void OnDisappearing()
        {
            base.OnDisappearing();
            ViewModel.PropertyChanged -= OnPropertyChanged;
            ViewModel.DidSelectRangeClicked -= OnSelectRangeClicked;

            RangeFrom.DateSelected -= OnFromRangeSelected;
            RangeUntil.DateSelected -= OnUntilRangeSelected;

        //    RangeList.ItemTapped -= OnRangeListItemTapped;
        }

        private void OnRangeListItemTapped(object sender, ItemTappedEventArgs e)
        {
        //    // Deselect item
         //   RangeList.ItemsSource = null;
        //    RangeList.ItemsSource = _rangeCollection;
        }

        private void OnFromRangeSelected(object sender, DateChangedEventArgs e)
        {
            RangeUntil.MinimumDate = RangeFrom.Date;
        }

        private void OnUntilRangeSelected(object sender, DateChangedEventArgs e)
        {
            RangeFrom.MaximumDate = RangeUntil.Date;
        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs args)
        {
            if (args?.PropertyName == nameof(ViewModel.CurrentDateTime))
                UpdateCalendar(ViewModel.CurrentDateTime);
        }

        private void OnSelectRangeClicked(object sender, EventArgs e)
        {
            _shouldShowRange = true;
            UpdateCalendar(ViewModel.CurrentDateTime);

            _rangeCollection.Add($"{RangeFrom.Date:d} - {RangeUntil.Date:d}");

            FocusButton = null;
        }

        private void UpdateCalendar(DateTime dateTime)
        {
            if (dateTime.Day != 1)
                throw new ArgumentException($"{nameof(dateTime)}.Day is {dateTime.Day} but should be 1!");

            // Remove days
            CalendarGrid.Children.Clear();

            // Update month and year label
            MonthLabel.Text = dateTime.ToString("MMMM");
            YearLabel.Text = dateTime.Year.ToString();

            // Add days
            var dayOfWeek = (int)dateTime.DayOfWeek - 1; // -1 to let the week start on Monday instead of Sunday
            if (dayOfWeek < 0)
                dayOfWeek = 6;

            var daysInMonth = DateTime.DaysInMonth(dateTime.Year, dateTime.Month) + 1;
            var row = 0;

            for (var day = 1; day < daysInMonth; day++)
            {
                var currentDay = dateTime.AddDays(day - 1);
                var isInRange = currentDay >= RangeFrom.Date && currentDay <= RangeUntil.Date;

                var dayButton = new Button
                {
                    Text = day.ToString(),
                    HorizontalOptions = LayoutOptions.CenterAndExpand,
                    VerticalOptions = LayoutOptions.CenterAndExpand,
                    BackgroundColor = Color.White,

                 };

                //오늘 버튼 표시 
                if(DateTime.Now.Year == currentDay.Year && DateTime.Now.Month == currentDay.Month && DateTime.Now.Day == currentDay.Day)
                {
                    FocusButton = dayButton;
                    FocusButton.BackgroundColor = Color.Red;
                }

                dayButton.Clicked += delegate {
                    
                    if (FocusButton != null)
                        FocusButton.BackgroundColor = Color.White;

                    FocusButton = dayButton;
                    FocusButton.BackgroundColor = Color.Red;

                    ScheduleGrid.IsVisible = true;

                    ScheduleView.Clear();
                    SchedueListView.ItemsSource = scheduleView;

                    foreach (var data in SQLLiteDB.ReadUserScheduleData())
                    {
                        if (dateTime.Year == data.Time.Year && dateTime.Month == data.Time.Month && Convert.ToInt16(dayButton.Text) == data.Time.Day)
                            scheduleView.Add(new ScheduleViewItem { Display = Helper.DateTimeToShortTime(data.Time) + " " + data.Message,Id = data.Id });
                    }
                };

                if (_shouldShowRange && isInRange)
                    dayButton.BackgroundColor = Color.Red;

                CalendarGrid.Children.Add(dayButton, dayOfWeek, row);

                dayOfWeek += 1;

                // Begin new row
                if (dayOfWeek > 6)
                {
                    dayOfWeek = 0;
                    row++;
                }
            }
        }


        private void ClearCalendarButtonColor(Color color)
        { 
            foreach(var elem in CalendarGrid.Children)
            {
                if(elem.GetType() == typeof(Button))
                {
                    elem.BackgroundColor = color;
                }
            }

            if (FocusButton != null)
                FocusButton.BackgroundColor = Color.Red;
        }

        private Button FocusButton;

        ObservableCollection<ScheduleViewItem> scheduleView = new ObservableCollection<ScheduleViewItem>();
        public ObservableCollection<ScheduleViewItem> ScheduleView { get { return scheduleView; } }

        async void OnScheduleDelButtonClicked(object sender, EventArgs args)
        {
            Button button = sender as Button;

            long id = Convert.ToInt32(button.CommandParameter.ToString());

            SQLLiteDB.DelItem(id);

            List<ScheduleViewItem> saveList = new List<ScheduleViewItem>();

            foreach (var data in scheduleView)
            {
                if (data.Id == id)
                    continue;

                saveList.Add((ScheduleViewItem)data);
            }

            ScheduleView.Clear();

            foreach (var data in saveList)
            {
                scheduleView.Add((ScheduleViewItem)data);
            }

            SchedueListView.ItemsSource = scheduleView;
        }

        async void OnScheduleSaveButtonClicked(object sender, EventArgs args)
        {
            //하나씩 저장
            if (FocusButton != null)
            {
                DateTime saveDateTime = new DateTime(ViewModel.CurrentDateTime.Year, ViewModel.CurrentDateTime.Month, Convert.ToInt32(FocusButton.Text), timePicker1.Time.Hours, timePicker1.Time.Minutes, timePicker1.Time.Seconds);
  
                UserScheduleData saveShedulData = new UserScheduleData();
                saveShedulData.Time = saveDateTime;
                saveShedulData.Message = ScheduleEdit.Text;
                SQLLiteDB.Upsert(saveShedulData);

                scheduleView.Add(new ScheduleViewItem { Id = saveShedulData.Id,Display = Helper.DateTimeToShortTime(saveDateTime) + " " + ScheduleEdit.Text });

                FocusButton.BackgroundColor = Color.White;
                FocusButton = null;
                ScheduleEdit.Text = string.Empty;
                timePicker1.Time = new TimeSpan(12, 0, 0);
             }
            else //범위로 저장
            {
                DateTime current = RangeFrom.Date;
                while (true)
                {
                    DateTime saveDateTime = new DateTime(current.Year, current.Month, current.Day, timePicker1.Time.Hours, timePicker1.Time.Minutes, timePicker1.Time.Seconds);

                    UserScheduleData saveShedulData = new UserScheduleData();
                    saveShedulData.Time = saveDateTime;
                    saveShedulData.Message = ScheduleEdit.Text;
                    SQLLiteDB.Upsert(saveShedulData);

                    current = current.AddDays(1);

                    if (current > RangeUntil.Date)
                        break;
                }

                ClearCalendarButtonColor(Color.White);

                RangeStackLayout.IsVisible = false;
                ScheduleGrid.IsVisible = false;
            }
        }
        async void OnLogSearchButtonClicked(object sender, EventArgs args)
        {
            ClearCalendarButtonColor(Color.White);
            ScheduleView.Clear();
            foreach (var schedule in SQLLiteDB.ReadScheduleLog())
            {
                scheduleView.Add(new ScheduleViewItem { Display = schedule.Time.ToString() });
            }

            SchedueListView.ItemsSource = scheduleView;

        }
        async void OneDayRegButtonClicked(object sender, EventArgs args)
        {
            RangeStackLayout.IsVisible = false;
            ScheduleGrid.IsVisible = true;

            ClearCalendarButtonColor(Color.White);

        }
        async void ManyDayRegButtonClicked(object sender, EventArgs args)
        {
            RangeStackLayout.IsVisible = true;
            ScheduleGrid.IsVisible = false;
        }
        async void SelectButtonClicked(object sender, EventArgs args)
        {
            ScheduleGrid.IsVisible = true;
        }
        

        public class ScheduleViewItem
        {
            public long Id { get; set; }
            public DateTime Time { get; set; }
            public string Message { get; set; }
            public string Display { get; set; }
        }

    }
}