using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK.Network;
using OMOK.CustomAdMobView;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
        const string timeFormat = @"%m\:ss";
        bool isGameInProgress;
        DateTime gameStartTime;

        iAd_IterstitialView iIterstitia;

        public Room()
        {
            InitializeComponent();

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();


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

                    int remaintime = (30 - (int)(DateTime.Now - gameStartTime).TotalSeconds);

                    if (remaintime < 0)
                        remaintime = 0;

                    timeLabel.Text = remaintime.ToString(timeFormat);
                    return isGameInProgress;
                });
            };

            board.GameEnded += (sender, hasWon) =>
            {
                isGameInProgress = false;

                if (hasWon)
                {
               //     DisplayWonAnimation();
                }
                else
                {
                  //  DisplayLostAnimation();
                }
            };

            PrepareForNewGame();

            board.UpdateAim();
        }

        protected override void OnDisappearing()
        {
            NetProcess.SendLeaveRoom(0);
        }

        public void UpdateBattleInfo()
        {
            if(User.Color == eTeam.Black)
            {
                blackLabel.Text = User.myInfo.NickName;
                whiteLabel.Text = User.OppInfo.NickName;

                bottom1picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
                bottom2picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));
            }
            else
            {
                whiteLabel.Text = User.myInfo.NickName;
                blackLabel.Text = User.OppInfo.NickName;

                bottom1picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));
                bottom2picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
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

     

        void OnplayAgainButtonClicked(object sender, object EventArgs)
        {
            PrepareForNewGame();
        }

        async void OnLeaveClicked(object sender, System.EventArgs e)
        {
            NetProcess.SendLeaveRoom(0);

            iIterstitia.ShowAd();
        }

        async void OnClickedLeft(object sender, System.EventArgs e)
        {
            board.UpdateAim(-1, 0);
        }

        async void OnClickedUp(object sender, System.EventArgs e)
        {
            board.UpdateAim(0, -1);
        }

        async void OnPutStone(object sender, System.EventArgs e)
        {
            if (User.IsMyTurn == false)
            {
                return ; 
            }


            var status = board.GetTile(board.x, board.y).Status;

            if (status != eTeam.None && status != eTeam.Aim)
            {
                return;
            }


            board.prevsTATE = User.Color;
            string Message = board.x + ":" + board.y + ":" + User.Color.ToString();
            NetProcess.SendRoomMessage(board.x, board.y, User.Color, Message);
        }

        async void OnClickedDown(object sender, System.EventArgs e)
        {
            board.UpdateAim(0, 1);
        }

        async void OnClickedRight(object sender, System.EventArgs e)
        {
            board.UpdateAim(1, 0);
        }

        public bool UpdateTurnBackground(eTeam status)
        {
            if(status == eTeam.Black)
            {

                blackLabel.BackgroundColor = Color.Beige;

                whiteLabel.BackgroundColor = Color.White;
            }
            else
            {
                blackLabel.BackgroundColor = Color.White;

                whiteLabel.BackgroundColor = Color.Beige;
            }

            return true;
        }

        public void ClearBoard()
        {
            board.ClearBoardState();
        }

        public bool UpdateStone(int x,int y, eTeam status)
        {
            //if (User.IsMyTurn == false || board.GetTile(x,y).Status != eTeam.None)
            //{
            //    return false; 
            //}

            string Message = x + ":" + y + ":" + User.Color.ToString();

            //NetProcess.SendRoomMessage(x, y, User.Color, Message);

            User.IsMyTurn = false;

            board.UpdateStone(x, y, status);

            return true;
        }

        public bool GameResult(GAME_RESULT_NTY nty)
        {
            try
            {
                string Message = "";

                if(nty.VarIndex1 == User.Id)
                {
                    if(User.myInfo.level < nty.VarLevel1)
                    {
                        Message="축하합니다. 승급하셨습니다. " +  User.myInfo.level + " => " + nty.VarLevel1;
                    }
                    else if(User.myInfo.level > nty.VarLevel1)
                    {
                        Message = "강등되셨습니다. " + User.myInfo.level + " => " + nty.VarLevel1;
                    }

                    Message += "\n 포인트 " + User.myInfo.score + " => " + nty.VarLevelPoint1;

                    User.myInfo.level = nty.VarLevel1;
                    User.myInfo.score = nty.VarLevelPoint1;
                }
                else if (nty.VarIndex2 == User.Id)
                {
                    if (User.myInfo.level < nty.VarLevel2)
                    {
                        Message = "축하합니다. 승급하셨습니다. " + User.myInfo.level + " => " + nty.VarLevel2;
                    }
                    else if (User.myInfo.level > nty.VarLevel2)
                    {
                        Message = "강등되셨습니다. " + User.myInfo.level + " => " + nty.VarLevel2;
                    }

                    Message += "\n 포인트 " + User.myInfo.score + " => " + nty.VarLevelPoint2;

                    User.myInfo.level = nty.VarLevel2;
                    User.myInfo.score = nty.VarLevelPoint2;
                }

              

                if (nty.VarIndex == User.Id)
                    DisplayAlert("", "승리 하셨습니다..\n" + Message, "OK");
                else
                    DisplayAlert("", "패배 하셨습니다.\n" + Message, "OK");


                if (User.Id == nty.VarIndex)
                {
                    User.myInfo.win += 1;
                }
               else
                {
                    User.myInfo.lose += 1;
                }

 //            board.ClearBoardState();

               //이번엔 서로 진형을 변경한다. 
               //eTeam newColor = User.Color == eTeam.Black ? eTeam.White : eTeam.Black;

               //User.Color = newColor;

               //UpdateBattleInfo();

               //if (User.Color == eTeam.Black)
               //    User.IsMyTurn = true;
               //else
               //    User.IsMyTurn = false;

               //User.MytrunStartTime = DateTime.Now;

               return true;
            }
            catch (Exception e)
            {

                return false;
            }
        }
    }
}