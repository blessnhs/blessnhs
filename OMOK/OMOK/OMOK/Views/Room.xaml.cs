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
using OMOK.Control;
using Xamarin.Essentials;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
 
        iAd_IterstitialView iIterstitia;

        public Room()
        {
            InitializeComponent();

            ClearBoardState();

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };

            ProgressRoom.Progress = 0.0f;

            timeLabel.IsEnabled = true;
            timeLabel.Text = "00";

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    try
                    {
                        if (User.IsMyTurn == true)
                        {
                            var current = ((DateTime.Now - User.MytrunStartTime).TotalSeconds * 0.033);
                            ProgressRoom.Progress = current;

                            int remainseconds = 30 - (int)((DateTime.Now - User.MytrunStartTime).TotalSeconds);
                            timeLabel.Text = remainseconds.ToString();
                            DependencyService.Get<Toast>().Show(timeLabel.Text);

                            //   ToastNotification.TostMessage(timeLabel.Text);

                            if ((DateTime.Now - User.MytrunStartTime).TotalSeconds > 30)
                            {
                                NetProcess.SendPassThroughMessage(-1, -1, User.Color);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                });

                return true;
            });
        }

        public void InitTimer()
        {
            User.MytrunStartTime = DateTime.Now;
            ProgressRoom.Progress = 0.0f;

            timeLabel.Text = "00";

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

                //순서는 반대로 흑이 했다면 다음은 백이 할차례
                UpdateTurnBackground(icolor == 0 ? eTeam.Black : eTeam.White);

            }
        }

        int aimx = ConstValue.SIZE / 2, aimy = ConstValue.SIZE / 2;
        BoardLayout prevLayout = null;

        public void UpdateAim(int x, int y)
        {
            var view = grid.Children[y * ConstValue.SIZE + x];

            BoardLayout lo = view as BoardLayout;

            lo.BackgroundColor = Color.DarkGray;

            if (prevLayout != null)
            {
                prevLayout.BackgroundColor = Color.FromHex("#F7E48B");

            }

            prevLayout = lo;
        }

        public bool CheckValid(int _x, int _y)
        {
            if (_x < 0 || _y < 0 || ConstValue.SIZE <= _x || ConstValue.SIZE <= _y)
                return false;

            return true;
        }

        public void RefreshAim()
        {
            UpdateAim(aimx, aimy);
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

        public void ClearBoardState()
        {
            grid.Children.Clear();
            for (int y = 0; y < ConstValue.SIZE; y++)
            {
                for (int x = 0; x < ConstValue.SIZE; x++)
                {
                    BoardLayout slo = new BoardLayout();

                    slo.Margin = new Thickness(0, 0, 0, 0);
                    slo.Padding = new Thickness(0, 0, 0, 0);
                    slo.Orientation = StackOrientation.Vertical;
                    slo.BackgroundColor = Color.FromHex("#F7E48B");
                    slo.IdField = x + ":" + y;
                    slo.GestureRecognizers.Add(
                          new TapGestureRecognizer()
                          {
                              Command = new Command(() => {

                                  if (prevLayout != null)
                                      prevLayout.BackgroundColor = Color.FromHex("#F7E48B");

                                  slo.BackgroundColor = Color.FloralWhite;

                                  var words = slo.IdField.Split(':');

                                  aimx = Convert.ToInt32(words[0]);
                                  aimy = Convert.ToInt32(words[1]);


                                  UpdateAim(aimx, aimy);

                                  prevLayout = slo;

                              })
                          });
                    grid.Children.Add(slo, x, y);
                }
            }


            aimx = ConstValue.SIZE / 2;
            aimy = ConstValue.SIZE / 2;

            prevLayout = null;

            UpdateAim(aimx, aimy);

        }

        public void ShowLeaveAd()
        {
            iIterstitia.ShowAd();
        }

        void OnLeaveClicked(object sender, System.EventArgs e)
        {
            if (User.state == PlayerState.Room)
                NetProcess.SendLeaveRoom(0);
            else
            {
                MainPage parentpage = (MainPage)this.Parent;
                parentpage.CurrentPage = parentpage.Children[0];
            }
        }

        void OnClickedLeft(object sender, System.EventArgs e)
        {
            if (0 > aimx)
                return;

            aimx -= 1;

            UpdateAim(aimx, aimy);
        }

        void OnClickedUp(object sender, System.EventArgs e)
        {
            if (0 > aimy)
                return;

            aimy -= 1;
            UpdateAim(aimx, aimy);
        }

        void OnPutStone(object sender, System.EventArgs e)
        {
            if (User.IsMyTurn == false)
            {
                return;
            }

            var view = grid.Children[aimy * ConstValue.SIZE + aimx];

            BoardLayout lo = view as BoardLayout;

            int cnt = lo.Children.Count();

            if (cnt > 0)
                return;

            NetProcess.SendPassThroughMessage(aimx, aimy, User.Color);
        }

        void OnClickedDown(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= aimy)
                return;

            aimy += 1;
            UpdateAim(aimx, aimy);
        }

        void OnClickedRight(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= aimx)
                return;

            aimx += 1;
            UpdateAim(aimx, aimy);
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
            ClearBoardState();
        }

        public bool UpdateStone(int x, int y, eTeam status)
        {
            User.IsMyTurn = false;

            var view = grid.Children[y * ConstValue.SIZE + x];

            BoardLayout slo = view as BoardLayout;

            if (status == eTeam.White)
            {
                slo.Children.Add(new Button()
                {
                    BackgroundColor = Color.White,
                    CornerRadius = (int)Bounds.Width / 2,
                    HeightRequest = slo.Bounds.Height,
                    BorderWidth = 1,
                    BorderColor = Color.Black
                }); ; ;
            }
            else
            {
                slo.Children.Add(new Button()
                {
                    BackgroundColor = Color.Black,
                    CornerRadius = (int)Bounds.Width / 2,
                    HeightRequest = slo.Bounds.Height,
                });
            }

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