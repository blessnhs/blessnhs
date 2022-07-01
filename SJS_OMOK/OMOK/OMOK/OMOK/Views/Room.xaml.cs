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
using System.Threading;

namespace OMOK
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Room : ContentPage
    {
        public BoardRenderer _renderer = new BoardRenderer();

        AI _ai = new AI();
        public bool isPlaying = false;

        iAd_RewardVideoView rewardVideo;

        int size = 15;

        //3,3 체크를 위해 렌주룰 흑을 생성 백은 어차피 제한이 없음
        public CBLACKSTONE renjuRuleChecker = new CBLACKSTONE();

        public void InitBoardGrid()
        {
            Device.StartTimer(new TimeSpan(0, 0, 1), () =>
            {
                // do something every 60 seconds
                Device.BeginInvokeOnMainThread(() =>
                {
                    _renderer.Init(size/* //오목 15x15*/, Width, ref absoluteLayout, blackLabel, whiteLabel, bottom1picture, bottom2picture);
                });
                return false; // runs again, or false to stop
            });

        }

        public void RefreshTurnInfo(eTeam team)
        {
            _renderer.UpdateTurnBackground(team);
            _renderer.UpdateAim();
        }

        /// <summary>
        /// /////////////////////////////////////////////////////////////////////

        /// </summary>
        int debug;
        public Room()
        {
            InitializeComponent();

            if (User.IsEnableScreenChat == true)
                Chatting.IsEnabled = true;

            InitTimer();

            _ai._renderer = _renderer;
    
            renjuRuleChecker.initBoard();

            InitBoardGrid();

            if (User.Auto == true)
            {
                User.myInfo.ai_reset_flag = true;

                if (User.Color == eTeam.Black)
                    Thread.Sleep(1000);
            }

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
               
                 try
                 {
                     if (User.IsMyTurn == true)
                     {
                         var current = ((DateTime.Now - User.MytrunStartTime).TotalSeconds * 0.033);
                         ProgressRoom.Progress = current;

                         int remainseconds = 30 - (int)((DateTime.Now - User.MytrunStartTime).TotalSeconds);
                         if (remainseconds < 0)
                             remainseconds = 0;

                        Device.BeginInvokeOnMainThread(() =>
                        {
                            timeLabel.Text = remainseconds.ToString();
                        });

                      //   DependencyService.Get<Toast>().Show(timeLabel.Text);

                         //   ToastNotification.TostMessage(timeLabel.Text);

                         if ((DateTime.Now - User.MytrunStartTime).TotalSeconds > 30)
                         {
                             if (User.Auto == true)
                                 NetProcess.SendLeaveRoom(0);
                             else
                                 NetProcess.SendPassThroughMessage(-1, -1, User.Color);
                         }
                     }

                     if(debug > 90)
                         NetProcess.SendLeaveRoom(0);


                     ///////////////////////////////////////////////////////////////////
                     if (User.myInfo.ai_reset_flag == true)
                     {
                         //흑ai
                         if (User.Color == eTeam.Black)
                         {
                             User.myInfo.ai_rule = 3;
                             User.myInfo.ai_mode = 1;
                         }
                         else//백ai
                         {
                             User.myInfo.ai_rule = 3;
                             User.myInfo.ai_mode = 2;
                         }

                        Device.BeginInvokeOnMainThread(() =>
                        {
                            _ai.PlayGame(User.myInfo.ai_rule, User.myInfo.ai_mode, true);
                            isPlaying = true;
                            User.myInfo.ai_reset_flag = false;

                            UpdateBattleInfo();
                        });

                     }

                     if (isPlaying == true)
                     {
                        Device.BeginInvokeOnMainThread(() =>
                        {
                            isPlaying = _ai.PlaygameLoop(this, User.myInfo.ai_mode,true);
                        });

                        if (isPlaying == false) //종료
                         {
                             isPlaying = false;

                         }
                     }

                 }
                 catch (Exception e)
                 {
                     Console.WriteLine(e.ToString());
                 }
              

                return !isExit;
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

        public void SendLeaveRoom()
        {
            NetProcess.SendLeaveRoom(0);
        }

        public void ProcReceivePutStoneMessage(ROOM_PASS_THROUGH_RES res)
        {

            sbyte x = 0, y = 0;
            byte icolor = 0;

            Helper.Get_X_Y_COLOR(ref x, ref y, ref icolor, res.VarMessageInt);

            eTeam color = icolor == 0 ? eTeam.White : eTeam.Black;

            if (CheckValid(x, y) == true) //시간만료면 false
            {
               
                _renderer.UpdateStone(x, y, color, false);
            }
            debug++;

            //check turn
            {
                if (User.Color == color)
                {
                    User.IsMyTurn = false;
                }
                else
                {
                    _ai.aix = x;
                    _ai.aiy = y;


                    User.IsMyTurn = true;
                    User.MytrunStartTime = DateTime.Now;
                }

                //순서는 반대로 흑이 했다면 다음은 백이 할차례
                _renderer.UpdateTurnBackground(icolor == 0 ? eTeam.Black : eTeam.White);

            }
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

        public bool CheckValid(int _x, int _y)
        {
            if (_x < 0 || _y < 0 || ConstValue.SIZE <= _x || ConstValue.SIZE <= _y)
                return false;

            return true;
        }
        bool isExit = false;

        protected override void OnDisappearing()
        {
            isExit = true;
            NetProcess.SendLeaveRoom(0);
        }
        public void ShowLeaveAd()
        {
        //    rewardVideo.ShowAd();
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
            if (0 > _renderer.aimx)
                return;

            _renderer.aimx -= 1;

            _renderer.UpdateAim();
        }

        void OnClickedUp(object sender, System.EventArgs e)
        {
            if (0 > _renderer.aimy)
                return;

            _renderer.aimy -= 1;
            _renderer.UpdateAim();
        }

        public Dictionary<int, Dictionary<int,int>> board = new Dictionary<int, Dictionary<int, int>>();

        void OnPutStone(object sender, System.EventArgs e)
        {
            if (User.IsMyTurn == false)
            {
                return;
            }

            if (board.ContainsKey(_renderer.aimy) == false)
                board[_renderer.aimy] = new Dictionary<int, int>();

            if (board[_renderer.aimy].ContainsKey(_renderer.aimx) == false)
            {
                board[_renderer.aimy][_renderer.aimx] = 0;
            }
            else if(board[_renderer.aimy][_renderer.aimx] != 0)
            {
                return;
            }

            if (User.Color == eTeam.Black)
            {
                //내부 좌표는 1,1 시작 ui이는 0,0 시작이라 변환
                int x = _renderer.aimx + 1;
                int y = _renderer.aimy + 1;

                var ret = renjuRuleChecker.placement(x, y, 0);
                if (ret == 11 || ret == 10) //삼삼 11 이미 놓여졌음 10
                {
                    DependencyService.Get<Toast>().Show("착수금지 구역입니다.");
                    return;
                }
            }

            NetProcess.SendPassThroughMessage(_renderer.aimx, _renderer.aimy, User.Color);
        }

        CameraPage cameraPage;

        void OnChatting(object sender, System.EventArgs e)
        {
            cameraPage = null;
            cameraPage = new CameraPage();
            //Navigation.PushAsync((new CameraPage()/*_MachPage*/));
            Navigation.PushPopupAsync(cameraPage);
        }
            

        void OnClickedDown(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= _renderer.aimy)
                return;

            _renderer.aimy += 1;
            _renderer.UpdateAim();
        }

        void OnClickedRight(object sender, System.EventArgs e)
        {
            if (ConstValue.SIZE - 1 <= _renderer.aimx)
                return;

            _renderer.aimx += 1;
            _renderer.UpdateAim();
        }
     
        public void ClearBoard()
        {
            _renderer.ClearBoardState();
        }

        public bool GameResult(GAME_RESULT_NTY nty)
        {
            try
            {
                if (User.Auto != true)
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