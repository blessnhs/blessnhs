using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{
    public class OxyExData
    {
        public enum LogType
        {
            YEAR_LOG = 1000,
            MONTH_LOG = 30,
            WEEK_LOG = 7,
            DAY_LOG = 1
        }

        public Dictionary<int, Log> LogData = new Dictionary<int, Log>();

        public PlotModel PieModel { get; set; }
        public PlotModel AreaModel { get; set; }
        public PlotModel BarModel { get; set; }
        public PlotModel StackedBarModel { get; set; }

        public OxyExData(Dictionary<int, Log> _LogData)
        {
            LogData = _LogData;
     

            PieModel = CreatePieChart();
            AreaModel = CreateAreaChart();
            StackedBarModel = CreateBarChart(true, "");
            BarModel = CreateBarChart(false, "Win Defeat Graph");
        }
        private PlotModel CreatePieChart()
        {
            var model = new PlotModel { Title = "Pie Chart" };

            var ps = new PieSeries
            {
                StrokeThickness = .25,
                InsideLabelPosition = .25,
                AngleSpan = 360,
                StartAngle = 0
            };

            // http://www.nationsonline.org/oneworld/world_population.htm
            // http://en.wikipedia.org/wiki/Continent
            ps.Slices.Add(new PieSlice("Africa", 1030) { IsExploded = false });
            ps.Slices.Add(new PieSlice("Americas", 929) { IsExploded = false });
            ps.Slices.Add(new PieSlice("Asia", 4157));
            ps.Slices.Add(new PieSlice("Europe", 739) { IsExploded = false });
            ps.Slices.Add(new PieSlice("Oceania", 35) { IsExploded = false });
            model.Series.Add(ps);
            return model;
        }

        public PlotModel CreateAreaChart()
        {
            var plotModel1 = new PlotModel { Title = "Area Series with crossing lines" };
            var areaSeries1 = new AreaSeries();
            areaSeries1.Points.Add(new DataPoint(0, 50));
            areaSeries1.Points.Add(new DataPoint(10, 140));
            areaSeries1.Points.Add(new DataPoint(20, 60));
            areaSeries1.Points2.Add(new DataPoint(0, 60));
            areaSeries1.Points2.Add(new DataPoint(5, 80));
            areaSeries1.Points2.Add(new DataPoint(20, 70));
            plotModel1.Series.Add(areaSeries1);
            return plotModel1;
        }

        private PlotModel CreateBarChart(bool stacked, string title)
        {
            var model = new PlotModel
            {
                Title = title,
                LegendPlacement = LegendPlacement.Outside,
                LegendPosition = LegendPosition.BottomCenter,
                LegendOrientation = LegendOrientation.Horizontal,
                LegendBorderThickness = 0
            };

            var s1 = new BarSeries { Title = "Win", IsStacked = stacked, StrokeColor = OxyColors.Black, StrokeThickness = 1 };
            s1.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.YEAR_LOG].win });
            s1.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.MONTH_LOG].win });
            s1.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.WEEK_LOG].win });
            s1.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.DAY_LOG].win });

            var s2 = new BarSeries { Title = "Defeat", IsStacked = stacked, StrokeColor = OxyColors.Black, StrokeThickness = 1 };
            s2.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.YEAR_LOG].defeat });
            s2.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.MONTH_LOG].defeat });
            s2.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.WEEK_LOG].defeat });
            s2.Items.Add(new BarItem { Value = LogData[(int)OxyExData.LogType.DAY_LOG].defeat });

           

            var categoryAxis = new CategoryAxis { Position = CategoryAxisPosition() };
            categoryAxis.Labels.Add("Year " + DateTime.Now.Year);
            categoryAxis.Labels.Add("Monthly " + +DateTime.Now.Month);
            categoryAxis.Labels.Add("Weekly");
            categoryAxis.Labels.Add("Daily");
            var valueAxis = new LinearAxis { Position = ValueAxisPosition(), MinimumPadding = 0, MaximumPadding = 1, AbsoluteMinimum = 0 };
            model.Series.Add(s1);
            model.Series.Add(s2);
            model.Axes.Add(categoryAxis);
            model.Axes.Add(valueAxis);
            return model;
        }

        private AxisPosition CategoryAxisPosition()
        {
            if (typeof(BarSeries) == typeof(ColumnSeries))
            {
                return AxisPosition.Bottom;
            }

            return AxisPosition.Left;
        }

        private AxisPosition ValueAxisPosition()
        {
            if (typeof(BarSeries) == typeof(ColumnSeries))
            {
                return AxisPosition.Left;
            }

            return AxisPosition.Bottom;
        }

    }
    public class Record
    {
        public string Text { get; set; }
    }

    public class Log
    {
        public int win;
        public int defeat;
    }

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class RecordHistory : ContentPage
    {
        public RecordHistory()
        {
            InitializeComponent();

             Dictionary<int, Log> LogData = new Dictionary<int, Log>();
            LogData[(int)OxyExData.LogType.YEAR_LOG] = new Log();
            LogData[(int)OxyExData.LogType.MONTH_LOG] = new Log();
            LogData[(int)OxyExData.LogType.WEEK_LOG] = new Log();
            LogData[(int)OxyExData.LogType.DAY_LOG] = new Log();


            leaveButton.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };

            var monday = DateTime.Today.AddDays(-(int)DateTime.Today.DayOfWeek + (int)DayOfWeek.Monday);
            var satday = DateTime.Today.AddDays(-(int)DateTime.Today.DayOfWeek + (int)DayOfWeek.Sunday);


            var list = SQLite.ReadResultLog();

            foreach (var r in list)
            {

                //year
                if (DateTime.Now.Year == r.Time.Year)
                {
                    if(r.Result == 1)
                        LogData[(int)OxyExData.LogType.YEAR_LOG].win += 1;
                    else
                        LogData[(int)OxyExData.LogType.YEAR_LOG].defeat += 1;
                }

                //week
                if (DateTime.Now >= monday && satday <= DateTime.Now)
                {
                    if (r.Result == 1)
                        LogData[(int)OxyExData.LogType.WEEK_LOG].win += 1;
                    else
                        LogData[(int)OxyExData.LogType.WEEK_LOG].defeat += 1;
                }

                //month
                if (DateTime.Now.Month == r.Time.Month)
                {
                    if (r.Result == 1)
                        LogData[(int)OxyExData.LogType.MONTH_LOG].win += 1;
                    else
                        LogData[(int)OxyExData.LogType.MONTH_LOG].defeat += 1;
                }
                //day
                if (DateTime.Now.Day == r.Time.Day)
                {
                    if (r.Result == 1)
                        LogData[(int)OxyExData.LogType.DAY_LOG].win += 1;
                    else
                        LogData[(int)OxyExData.LogType.DAY_LOG].defeat += 1;
                }
            }

            var vData = new OxyExData(LogData);

            BindingContext = vData;
        }
    }
}