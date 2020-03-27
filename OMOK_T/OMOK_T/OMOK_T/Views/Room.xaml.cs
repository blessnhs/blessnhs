using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK_T
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
        const string timeFormat = @"%m\:ss";
        bool isGameInProgress;
        DateTime gameStartTime;
     

        public Room()
        {
            InitializeComponent();

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                if(User.IsMyTurn == true)
                {
                    timeLabel.Text = (DateTime.Now - User.MytrunStartTime).ToString(timeFormat);
                }

                return true;
            });

            board.GameStarted += (sender, args) =>
            {
                isGameInProgress = true;
                gameStartTime = DateTime.Now;

                Device.StartTimer(TimeSpan.FromSeconds(1), () =>
                {
                    timeLabel.Text = (DateTime.Now - gameStartTime).ToString(timeFormat);
                    return isGameInProgress;
                });
            };

            board.GameEnded += (sender, hasWon) =>
            {
                isGameInProgress = false;

                if (hasWon)
                {
                    DisplayWonAnimation();
                }
                else
                {
                    DisplayLostAnimation();
                }
            };

            PrepareForNewGame();
        }

        public void UpdateBattleInfo()
        {
            if(User.Color == TileStatus.Black)
            {
                blackLabel.Text = User.MyNickName;
                whiteLabel.Text = User.OppNickName;
            }
            else
            {
                whiteLabel.Text = User.MyNickName;
                blackLabel.Text = User.OppNickName;
            }
        }

        void PrepareForNewGame()
        {
            board.NewGameInitialize();

            timeLabel.Text = new TimeSpan().ToString(timeFormat);
            isGameInProgress = false;
        }

        void OnMainContentViewSizeChanged(object sender, EventArgs args)
        {
            ContentView contentView = (ContentView)sender;
            double width = contentView.Width;
            double height = contentView.Height;

            bool isLandscape = width > height;

            if (isLandscape)
            {
                mainGrid.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                mainGrid.RowDefinitions[1].Height = new GridLength(8, GridUnitType.Star);
                mainGrid.RowDefinitions[2].Height = new GridLength(1, GridUnitType.Star);


                mainGrid.ColumnDefinitions[0].Width = new GridLength(5, GridUnitType.Star);
                mainGrid.ColumnDefinitions[1].Width = new GridLength(5, GridUnitType.Star);

                Grid.SetRow(textStackTop, 1);
                Grid.SetColumn(textStackTop, 0);
            }
            else // portrait
            {
                mainGrid.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
                mainGrid.RowDefinitions[1].Height = new GridLength(8, GridUnitType.Star);
                mainGrid.RowDefinitions[2].Height = new GridLength(1, GridUnitType.Star);

                mainGrid.ColumnDefinitions[0].Width = new GridLength(5, GridUnitType.Star);
                mainGrid.ColumnDefinitions[1].Width = new GridLength(5, GridUnitType.Star);
            }
        }

        // Maintains a square aspect ratio for the board.
        void OnBoardContentViewSizeChanged(object sender, EventArgs args)
        {
            ContentView contentView = (ContentView)sender;
            double width = contentView.Width;
            double height = contentView.Height;
            double dimension = Math.Min(width, height);
            double horzPadding = (width - dimension) / 2;
            double vertPadding = (height - dimension) / 2;
            contentView.Padding = new Thickness(horzPadding, vertPadding);
        }

        async void DisplayWonAnimation()
        {

        }

        async void DisplayLostAnimation()
        {
        }

        async Task DisplayPlayAgainButton()
        {
        }

        void OnplayAgainButtonClicked(object sender, object EventArgs)
        {
            PrepareForNewGame();
        }

        public bool UpdateStone(int x,int y, TileStatus status)
        {
            board.UpdateStone(x, y, status);

            try
            {
                if (board.CheckPointer(y, x, status) == true)
                {
                    if (status == TileStatus.Black)
                        DisplayAlert("", "흑 승리하셨습니다..", "OK");
                    else
                        DisplayAlert("", "백 승리하셨습니다.", "OK");

                    board.ClearBoardState();

                    //이번엔 서로 진형을 변경한다. 
                    TileStatus newColor = User.Color == TileStatus.Black ? TileStatus.White : TileStatus.Black;

                    User.Color = newColor;

                    UpdateBattleInfo();

                    if (User.Color == TileStatus.Black)
                        User.IsMyTurn = true;
                    else
                        User.IsMyTurn = false;

                    User.MytrunStartTime = DateTime.Now;

                    return true;
                }

                return false;
            }
            catch(Exception e)
            {

                return false;
            }
        }
    }
}