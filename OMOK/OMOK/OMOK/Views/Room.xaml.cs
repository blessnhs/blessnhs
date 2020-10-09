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
using Xamarin.Forms.Shapes;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
        double screenx;
        double screeny;

        //square
        double boxwidth;
        double boxheight;

        double size = 19;  //default 19

        iAd_RewardVideoView rewardVideo;

        //3,3 체크를 위해 렌주룰 흑을 생성 백은 어차피 제한이 없음
        public CBLACKSTONE renjuRuleChecker = new CBLACKSTONE();

        public void InitBoardGrid()
        {
            size = 15; //오목 15x15

            Device.StartTimer(new TimeSpan(0, 0, 1), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    screeny = screenx = Width;

                    boxwidth = screenx / size;
                    boxheight = screeny / size;

                    var background = new Image();
                    background.Source = ImageSource.FromResource("OMOK.Image.Board_1.png");

                    background.Aspect = Aspect.AspectFill;
                    background.VerticalOptions = LayoutOptions.FillAndExpand;
                    background.HorizontalOptions = LayoutOptions.FillAndExpand;

                    absoluteLayout.Children.Add(background, new Xamarin.Forms.Rectangle(0, 0, 1, 1), AbsoluteLayoutFlags.All);

                    DrawLine();

                    TouchEffect touchEffect = new TouchEffect();
                    touchEffect.TouchAction += OnTouchEffectAction;
                    absoluteLayout.Effects.Add(touchEffect);
                });
                return false; // runs again, or false to stop
            });

        }

        /// <summary>
        /// /////////////////////////////////////////////////////////////////////
      
/// </summary>

        public Room()
        {
            InitializeComponent();

            renjuRuleChecker.initBoard();

            InitBoardGrid();

            ClearBoardState();

            Button PrevBtn = new Button { Text = "◁", HorizontalOptions = LayoutOptions.Start };
            PrevBtn.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };

            ProgressRoom.Progress = 0.0f;

            timeLabel.IsEnabled = true;
            timeLabel.Text = "00";

            rewardVideo = DependencyService.Get<iAd_RewardVideoView>();

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
                            if (remainseconds < 0)
                                remainseconds = 0;

                            timeLabel.Text = remainseconds.ToString();
                         //   DependencyService.Get<Toast>().Show(timeLabel.Text);

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


            MessagingCenter.Subscribe<Room>(this, "close", (sender) =>
            {
                NetProcess.SendLeaveRoom(0);

            });

            //언어 수동 변환
            if (User.Locale != "ko")
            {
                LeaveRoom.Text = "Exit";

                LeftButton.Text = "Left";
                UPButton.Text = "Up";
                Clicked.Text = "Stone";
                DownButton.Text = "Down";
                RightButton.Text = "Right";

                black.Text = "Black";
                white.Text = "White";
            }

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
        void DrawLayout(int x, int y, Color color)
        {
            if (aimMark != null)
                absoluteLayout.Children.Remove(aimMark);

            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 4;
            double yy = beginy + (boxwidth / 2) - boxwidth / 4;

            double xx_width = (boxwidth / 4) * 2;
            double yy_height = ((boxwidth / 4) * 2);

            aimMark = new BoxView
            {
                BackgroundColor = color
            };

            absoluteLayout.Children.Add(aimMark, new Xamarin.Forms.Rectangle(xx, yy, xx_width, yy_height));
        }

        BoxView aimMark = null;

        public void UpdateAim(int x, int y)
        {
            DrawLayout(x, y, Color.AliceBlue);
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
            NetProcess.SendLeaveRoom(0);
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
        void DrawLine()
        {
            double x, y;
            x = boxwidth / 2;
            y = boxwidth / 2;

            for (int yy = 0; yy < size; yy++) //draw vertical line
            {
                absoluteLayout.Children.Add(new BoxView
                {
                    Color = Color.Black,
                }, new Xamarin.Forms.Rectangle(x + (yy * boxwidth), y, 1, screeny - boxwidth));

            }


            for (int xx = 0; xx < size; xx++) //draw horizontal
            {
                absoluteLayout.Children.Add(new BoxView
                {
                    Color = Color.Black,
                }, new Xamarin.Forms.Rectangle(x, y + (xx * boxwidth), screenx - boxwidth, 1));

            }

            absoluteLayout.Children.Add(new BoxView
            {
                Color = Color.Black,
            }, new Xamarin.Forms.Rectangle(0, y + (size * boxwidth), screenx, 1));
        }

        void FindXY(int x, int y, out int out_x, out int out_y)
        {
            out_y = out_x = -1;

            for (int by = 0; by < size; by++)
            {
                for (int bx = 0; bx < size; bx++)
                {
                    double beginx = boxwidth * bx;
                    double beginy = boxwidth * by;

                    double xx = beginx + (boxwidth / 2) - boxwidth / 3;
                    double yy = beginy + (boxwidth / 2) - boxwidth / 3;

                    double xxwidth = xx + (boxwidth / 3) * 2;
                    double yywidth = yy + (boxwidth / 3) * 2;


                    if (xx <= x && xxwidth >= x)
                    {
                        if (yy <= y && yywidth >= y)
                        {
                            out_x = bx;
                            out_y = by;
                            return;
                        }
                    }
                }
            }
        }

        void OnTouchEffectAction(object sender, TouchActionEventArgs args)
        {
            if (args.Type != TouchActionType.Pressed)
                return;

            var x = args.Location.X;
            var y = args.Location.Y;

            int outx;
            int outy;
            FindXY((int)x, (int)y, out outx, out outy);

            aimx = outx;
            aimy = outy;

            UpdateAim(aimx, aimy);


        }

        public void ClearBoardState()
        {
            if (absoluteLayout.Children.Count > 1)
            {
                List<View> chiles = new List<View>();
                foreach (var child in absoluteLayout.Children)
                {
                    var type = child.GetType();
                    if (type.Name == "Image")
                        continue;

                    chiles.Add(child);
                }

                foreach (var child in chiles)
                {
                    absoluteLayout.Children.Remove(child);
                }
            }
            DrawLine();


            aimx = ConstValue.SIZE / 2;
            aimy = ConstValue.SIZE / 2;

            aimMark = null;

            UpdateAim(aimx, aimy);

        }

        public void ShowLeaveAd()
        {
            rewardVideo.ShowAd();
        }

        async void OnLeaveClicked(object sender, System.EventArgs e)
        {
            User.IsMyTurn = false;

            if (User.state == PlayerState.Room)
            {
                await Navigation.PushPopupAsync(new Confirm(this));
            }
            else
            {
                await Navigation.PopModalAsync();
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

            if(User.Color == eTeam.Black)
            {
                //내부 좌표는 1,1 시작 ui이는 0,0 시작이라 변환
                int x = aimx + 1;
                int y = aimy + 1;

                var ret = renjuRuleChecker.placement(x, y, 0);
                if (ret == 11 || ret == 10) //삼삼 11 이미 놓여졌음 10
                {
                    DependencyService.Get<Toast>().Show("착수금지 구역입니다.");
                    return;
                }
            }

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

        void DrawLastLayout(int x, int y, Color color)
        {
            if (LastStoneMark != null)
                absoluteLayout.Children.Remove(LastStoneMark);

            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 4;
            double yy = beginy + (boxwidth / 2) - boxwidth / 4;

            double xx_width = (boxwidth / 4) * 2;
            double yy_height = ((boxwidth / 4) * 2);

            LastStoneMark = new BoxView
            {
                BackgroundColor = color
            };

            absoluteLayout.Children.Add(LastStoneMark, new Xamarin.Forms.Rectangle(xx, yy, xx_width, yy_height));
        }

        void DrawRestrictLayout(int x, int y, Color color)
        {

            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 4;
            double yy = beginy + (boxwidth / 2) - boxwidth / 4;

            double xx_width = (boxwidth / 4) * 2;
            double yy_height = ((boxwidth / 4) * 2);

            Line line = new Line();
            line.X1 = xx;
            line.Y1 = yy;

            line.X2 = xx + xx_width;
            line.Y2 = yy + yy_height;
            line.BackgroundColor = color;

            absoluteLayout.Children.Add(line);
        }


        void DrawStone(int x, int y, Color color)
        {
            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 2;
            double yy = beginy + (boxwidth / 2) - boxwidth / 2;

            double xx_width = (boxwidth / 2) * 2;
            double yy_height = ((boxwidth / 2) * 2);


            if (color == Color.Black)
            {
                absoluteLayout.Children.Add(new ImageButton
                {
                    Source = ImageSource.FromResource("OMOK.Image.Black.png"),
                    CornerRadius = (int)(xx_width / 2)
                }, new Xamarin.Forms.Rectangle(xx, yy, xx_width, yy_height));
            }
            else
            {
                absoluteLayout.Children.Add(new ImageButton
                {
                    Source = ImageSource.FromResource("OMOK.Image.White.png"),
                    CornerRadius = (int)(xx_width / 2)
                }, new Xamarin.Forms.Rectangle(xx, yy, xx_width, yy_height));
            }

            UpdateAim(x, y);
        }


        BoxView LastStoneMark = null;

        public bool UpdateStone(int x, int y, eTeam status)
        {
            User.IsMyTurn = false;

            DrawStone(x, y, status == eTeam.Black ? Color.Black : Color.White);

            DrawLastLayout(x, y, Color.Aqua);


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

                    SQLite.InsertResultLog(DateTime.Now, User.OppInfo.NickName + " " + Helper.LevelConverter(User.OppInfo.level), 1);

                }
                else
                {
                    User.myInfo.lose += 1;

                    SQLite.InsertResultLog(DateTime.Now, User.OppInfo.NickName + " " + Helper.LevelConverter(User.OppInfo.level), 0);

                }

                return true;
            }
            catch (Exception e)
            {

                return false;
            }
        }
    }
}