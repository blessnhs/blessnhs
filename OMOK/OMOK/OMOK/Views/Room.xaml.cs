using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OMOK.Network;
using OMOK.CustomAdMobView;
using OMOK.Views;
using Rg.Plugins.Popup.Extensions;
using ToastMessage;



namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
        const string timeFormat = @"%m\:ss";
        bool isGameInProgress;

        iAd_IterstitialView iIterstitia;


        public Room()
        {
            InitializeComponent();

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };

            ProgressRoom.Progress = 0.0f;

            timeLabel.IsEnabled = true;
            timeLabel.IsRunning = true;
            timeLabel.Text = (DateTime.Now - User.MytrunStartTime).ToString(timeFormat);

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                if (User.IsMyTurn == true)
                {
                    var current = ((DateTime.Now - User.MytrunStartTime).TotalSeconds * 0.033);
                    ProgressRoom.Progress = current;
                    timeLabel.Text = (DateTime.Now - User.MytrunStartTime).ToString(timeFormat);
                    DependencyService.Get<Toast>().Show(timeLabel.Text);

                    //   ToastNotification.TostMessage(timeLabel.Text);

                    if ((DateTime.Now - User.MytrunStartTime).TotalSeconds > 30)
                    {
                        NetProcess.SendPassThroughMessage(-1, -1, User.Color);
                    }
                }

                return true;
            });

            PrepareForNewGame();
        }

        public void InitTimer()
        {
            User.MytrunStartTime = DateTime.Now;
            ProgressRoom.Progress = 0.0f;

            timeLabel.Text = (DateTime.Now - User.MytrunStartTime).ToString(timeFormat);

        }

        public void ProcReceivePutStoneMessage(ROOM_PASS_THROUGH_RES res)
        {

            sbyte x = 0, y = 0;
            byte icolor = 0;

            Helper.Get_X_Y_COLOR(ref x, ref y, ref icolor, res.VarMessageInt);

            eTeam color = icolor == 0 ? eTeam.White : eTeam.Black;

            if (CheckValid(x, y) == true) //시간만료면 false
            {
                UpdateStone(x, y, color);
            }

            //check turn
            {
                if (User.Color == color)
                {
                    User.IsMyTurn = false;
                }
                else
                {
                    User.IsMyTurn = true;
                    User.MytrunStartTime = DateTime.Now;
                }

                UpdateTurnBackground(color);

            }
        }

        public void RefreshAim()
        {
            board.UpdateAim();
        }

        public bool CheckValid(int _x, int _y)
        {
            return board.CheckValid(_x, _y);
        }

        protected override void OnDisappearing()
        {
         //   NetProcess.SendLeaveRoom(0);
        }

        public void UpdateBattleInfo()
        {
            if (User.Color == eTeam.Black)
            {
                blackLabel.Text = User.myInfo.NickName;
                whiteLabel.Text = User.OppInfo.NickName;

                if (User.myInfo.PhotoPath != null)
                    bottom1picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

                if (User.OppInfo.PhotoPath != null)
                    bottom2picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));
            }
            else
            {
                whiteLabel.Text = User.myInfo.NickName;
                blackLabel.Text = User.OppInfo.NickName;

                if (User.OppInfo.PhotoPath != null)
                    bottom1picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));

                if (User.myInfo.PhotoPath != null)
                    bottom2picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
            }
        }

        void PrepareForNewGame()
        {
            board.NewGameInitialize();

       //     timeLabel.Text = new TimeSpan().ToString(timeFormat);
            isGameInProgress = false;
        }

        void OnMainContentViewSizeChanged(object sender, EventArgs args)
        {
            ContentView contentView = (ContentView)sender;
            double width = contentView.Width;
            double height = contentView.Height;

            bool isLandscape = width > height;

           // if (isLandscape)
           // {
           //     mainGrid.RowDefinitions[0].Height = 0;
           //     mainGrid.RowDefinitions[1].Height = new GridLength(1, GridUnitType.Star);

           //     mainGrid.ColumnDefinitions[0].Width = new GridLength(1, GridUnitType.Star);
           //     mainGrid.ColumnDefinitions[1].Width = new GridLength(1, GridUnitType.Star);

           // }
           // else // portrait
           // {
           //     mainGrid.RowDefinitions[0].Height = new GridLength(3, GridUnitType.Star);
           //     mainGrid.RowDefinitions[1].Height = new GridLength(5, GridUnitType.Star);

           //     mainGrid.ColumnDefinitions[0].Width = 0;
           //     mainGrid.ColumnDefinitions[1].Width = new GridLength(1, GridUnitType.Star);

           //}
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
         //   contentView.Padding = new Thickness(horzPadding, vertPadding);
        }

        public void ShowLeaveAd()
        {
            iIterstitia.ShowAd();
        }

        async void OnLeaveClicked(object sender, System.EventArgs e)
        {
            if (User.state == PlayerState.Room)
                NetProcess.SendLeaveRoom(0);
            else
            {
                MainPage parentpage = (MainPage)this.Parent;
                parentpage.CurrentPage = parentpage.Children[0];
            }
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
                return;
            }

            var status = board.GetTile(board.x, board.y).Status;

            if (status == eTeam.White || status == eTeam.Awhite || status == eTeam.Black || status == eTeam.Ablack)
            {
                return;
            }


            board.prevsTATE = User.Color;
            NetProcess.SendPassThroughMessage(board.x, board.y, User.Color);
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
            if (status == eTeam.Black)
            {

                blackLabel.BackgroundColor = Color.YellowGreen;
                whiteLabel.BackgroundColor = Color.White;

                bottom1picture.BackgroundColor = Color.YellowGreen;
                bottom2picture.BackgroundColor = Color.White;
            }
            else
            {
                blackLabel.BackgroundColor = Color.White;
                whiteLabel.BackgroundColor = Color.YellowGreen;

                bottom2picture.BackgroundColor = Color.YellowGreen;
                bottom1picture.BackgroundColor = Color.White;
            }

            return true;
        }

        public void ClearBoard()
        {
            board.ClearBoardState();
        }

        public bool UpdateStone(int x, int y, eTeam status)
        {
            User.IsMyTurn = false;

            board.UpdateStone(x, y, status);

            board.UpdateAimSet(x, y);

            return true;
        }

        public bool GameResult(GAME_RESULT_NTY nty)
        {
            try
            {
                Navigation.PushPopupAsync(new GameResultPage(nty));

                User.IsMyTurn = false;

                if (User.Id == nty.VarIndex)
                {
                    User.myInfo.win += 1;

                    SQLite.InsertResultLog(DateTime.Now, User.OppInfo.NickName, 1);

                }
                else
                {
                    User.myInfo.lose += 1;

                    SQLite.InsertResultLog(DateTime.Now, User.OppInfo.NickName, 0);

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