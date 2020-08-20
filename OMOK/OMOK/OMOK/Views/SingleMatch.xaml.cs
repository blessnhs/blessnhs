using OMOK.CustomAdMobView;
using OMOK.Popup;
using Rg.Plugins.Popup.Extensions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using ToastMessage;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{

    public class COMOKFACTORY
    {
        public COMOK gomoku_Black = new COMOK();
        public COMOK gomoku_White = new COMOK();
        public CNORMAL general_Black = new CNORMAL();
        public CNORMAL general_White = new CNORMAL();
        public CBLACKSTONE renju_Black = new CBLACKSTONE();

        public CGOMOKUAI gomokuAI_Black = new CGOMOKUAI();
        public CGOMOKUAI gomokuAI_White = new CGOMOKUAI();
        public CJENERALAI normalAI_Black = new CJENERALAI();
        public CJENERALAI normalAI_White = new CJENERALAI();
        public CRENJUBLACKAI renjuAI_Black = new CRENJUBLACKAI();
        public CRENJUWHITEAI renjuAI_White = new CRENJUWHITEAI();

        public COMOK GetInstance(int type, int mode)
        {
            type = mode * 100 + type;
            switch (type)
            {
                case 110: return gomokuAI_Black;
                case 131:
                case 111: return gomoku_White;
                case 120: return normalAI_Black;
                case 121: return general_White;
                case 130: return renjuAI_Black;

                case 210: return gomoku_Black;
                case 211: return gomokuAI_White;
                case 220: return general_Black;
                case 221: return normalAI_White;
                case 230: return renju_Black;
                case 231: return renjuAI_White;

                case 410:
                case 510:
                case 310: return gomoku_Black;
                case 431:
                case 531:
                case 331:
                case 411:
                case 511:
                case 311: return gomoku_White;
                case 420:
                case 520:
                case 320: return general_Black;
                case 421:
                case 521:
                case 321: return general_White;
                case 430:
                case 530:
                case 330: return renju_Black;
            }

            return null;
        }
    };

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SingleMatch : ContentPage
    {
        const string timeFormat = @"%m\:ss";

        iAd_IterstitialView iIterstitia;

        //현재 착점을 할 돌
        int curStone;
        // omok의 x, y와 같은 좌표를 유지한다. 
        int x, y;

        // 무승부를 알기 위해서 변수가 이렇게 많이 필요하다.
        // 누군가 pass를  하면 passTrigger가 On상태가 된다.
        // 그럼 passCounter가 시작되고
        // 연속으로 백돌과 흑돌이 연속으로 눌렀는지 
        // 아니면 한쪽 돌만 연속으로 pass한 것인지 
        // 판단하기 위하여 stoneState는 배열을 사용함. 
        // 무승부임을 알리는게 의외로 복잡하다. 
        public bool isComputer;
        bool passTriggerOn;
        int passCount;
        int[] stoneState = new int[2];

        COMOKFACTORY factory = new COMOKFACTORY();
        COMOK []pOmok = new COMOK[2];


        void initGame(int mode)
        {
            x = y = ConstValue.CENTER;
            passTriggerOn = false;
            passCount = 0;
            stoneState[0] = 0;
            stoneState[1] = 0;
            if (mode == 2) isComputer = true;
            else isComputer = false;
            if (mode == 4) curStone = (int)V1.BLACK_STONE;
            else curStone = (int)V1.WHITE_STONE;

            if (mode == 5) pOmok[curStone].initOmok((int)V1.WHITE_STONE);
            else pOmok[curStone].initOmok((int)V1.BLACK_STONE);

            if (mode == 5) UpdateStone(x, y, eTeam.White);
            else UpdateStone(x, y, eTeam.Black);

          
            if(mode == 1)
                User.Color = eTeam.White;
            else
                User.Color = eTeam.Black;
        }

        // 흑과 백의 둘 차례를 바꾼다. 
        void changeStone(int mode)
        {
            // 흑백 둘이니 더해서 2로 나누면 교환이 된다. 
            if (mode == 4) curStone = (int)V1.BLACK_STONE;
            else if (mode == 5) curStone = (int)V1.WHITE_STONE;
            else curStone = (curStone + 1) % 2;

            if (mode == 1 || mode == 2) isComputer = !isComputer;
            // 현재 둘 차례를 알려준다. 
            if (passTriggerOn) passCount++;
        }

        int checkKey(int mode)
        {
            int ch, dx, dy; ;

            if (isComputer)
            {
                var ret = pOmok[curStone].placement(x, y, curStone);
                pOmok[curStone].getXY(ref x, ref y);
                UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White);
 
                return ret;

            }
            else if (aix != 0 || aiy != 0)
            {
                x = aix;
                y = aiy;

                aix = aiy = 0;

                UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White);

                var ret = pOmok[curStone].placement(x, y, curStone);
                return ret;
            }
            else return 0;

            //dx = dy = 0;
            //switch (ch)
            //{
            //    case UP: --dy; break;
            //    case DOWN: ++dy; break;
            //    case LEFT: --dx; break;
            //    case RIGHT: ++dx; break;

            //    case DEL: return PASS;
            //    case ESC: return FINISH;
            //    case ' ': return pOmok[curStone].placement(x, y, curStone);
            //    case 'r':
            //    case 'R':
            //        if (mode == 1 || mode == 2)
            //        {
            //            pOmok[curStone].redo();
            //            pOmok[(curStone + 1) % 2].redo();
            //        }
            //        else return pOmok[curStone].redo();
            //    case 'u':
            //    case 'U':
            //        if (mode == 1 || mode == 2)
            //        {
            //            pOmok[curStone].undo();
            //            pOmok[(curStone + 1) % 2].undo();
            //        }
            //        else return pOmok[curStone].undo();
            //    default: break;
            //}
            //pOmok[curStone].setBoard(x, y);
            //y += dy;
            //x += dx;


            //return 0;
        }

        public bool CheckAILoop()
        {
            isbegin = playgameloop(User.myInfo.ai_mode);
            isbegin = playgameloop(User.myInfo.ai_mode);
            if (isbegin == false) //종료
            {
                board.ClearBoardState();
                User.myInfo.ai_set_flag = true;
                isbegin = true;

                playGame(User.myInfo.ai_rule, User.myInfo.ai_mode);
                isbegin = true;
                User.myInfo.ai_set_flag = false;

                UpdateBattleInfo();

                isbegin = playgameloop(User.myInfo.ai_mode);
            }
            return true;
        }

        public bool playgameloop(int mode)
        {

            int result = checkKey(mode);
            if (isComputer) pOmok[curStone].getXY(ref x, ref y);
            switch (result)
            {
                //// 키보드로부터 ESC키가 눌리면 게임을 종료한다. 
                case 15:
                    {
                        Navigation.PushPopupAsync(new AIGameResult(curStone));
                        return false;
                    }
                   
                //// 오목이 되었으면 승자를 알리고, 계속할건지 물어본다. 
                //case FIVEMOK: return pDraw().endMsg(curStone);
                //// 착수가 불가능한 곳은 그 이유를 알린다. 
                //case OCCUPIED:
                //case SAMSAM:
                //case SASA:
                //case SIXMOK:
                //case NOTUNDO:
                //    pDraw().errMsg(result);
                //    pOmok[curStone].setBoard(x, y);
                //    break;
                ////착수가 됐거나, 한수 물렸을 때 돌을 바꾼다.  
                case 17:// (int)V2.PASS:
                    {
                        if (!passTriggerOn)
                            passTriggerOn = true;

                        stoneState[curStone]++;

                        if (mode == 1 || mode == 2)
                            stoneState[(curStone + 1) % 2]++;

                        changeStone(mode);
                    }
                    break;
                case 16:    //CHANGE
                    changeStone(mode);
                    break;
                default: break;
            }

            return true;
        }

        // main함수에서 이 함수를 호출하면 게임이 시작된다. 
        void playGame(int type, int mode)
        {
            pOmok[0] = factory.GetInstance(type * 10 + 0, mode);
            pOmok[1] = factory.GetInstance(type * 10 + 1, mode);


            initGame(mode);
        }

        public void UpdateBattleInfo()
        {
            if (User.Color == eTeam.Black)
            {
                blackLabel.Text = User.myInfo.NickName;
                whiteLabel.Text = "알파목";

                if (User.myInfo.PhotoPath != null)
                    bottom1picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

                if (User.OppInfo.PhotoPath != null)
                    bottom2picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));
            }
            else
            {
                 if (User.OppInfo.PhotoPath != null)
                    bottom1picture.Source = ImageSource.FromUri(new Uri(User.OppInfo.PhotoPath));

                if (User.myInfo.PhotoPath != null)
                    bottom2picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
            }
        }


        public bool isbegin = false;
    
        public SingleMatch()
        {
            InitializeComponent();

            // Subscribe to a message (which the ViewModel has also subscribed to) to display an alert
            MessagingCenter.Subscribe<SingleMatch, string>(this, "Start", async (sender, arg) =>
            {
                PrepareForNewGame();

                playGame(User.myInfo.ai_rule, User.myInfo.ai_mode);
                isbegin = true;
                User.myInfo.ai_set_flag = false;

                UpdateBattleInfo();

                isbegin = playgameloop(User.myInfo.ai_mode);
            });

            Navigation.PushPopupAsync(new AISelect(this));

            board.parent = this; 

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

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

        void PrepareForNewGame()
        {
            board.NewGameInitialize();

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
            var page = await Navigation.PopModalAsync();
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
            var status = board.GetTile(board.x, board.y).Status;

            if (status == eTeam.White || status == eTeam.Awhite || status == eTeam.Black || status == eTeam.Ablack)
            {
                return;
            }

            aix = board.x;
            aiy = board.y;

            board.UpdateStone(board.x, board.y, eTeam.Black);

            CheckAILoop();
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

        public int aix = 0, aiy = 0;

        public bool UpdateStone(int x, int y, eTeam status)
        {
            board.UpdateStone(x, y, status);
            board.UpdateAimSet(x, y);

            return true;
        }

      
    }
}