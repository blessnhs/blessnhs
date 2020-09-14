using OMOK.Control;
using OMOK.CustomAdMobView;
using OMOK.Popup;
using Rg.Plugins.Popup.Extensions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ToastMessage;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{
    class BoardLayout : StackLayout
    {
        public string IdField;
    }

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


        void InitGame(int mode)
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

            if(User.Color == eTeam.Black)
                UpdateTurnBackground(eTeam.Black);
            else
                UpdateTurnBackground(eTeam.White);
        }

        // 흑과 백의 둘 차례를 바꾼다. 
        void ChangeStone(int mode)
        {
            // 흑백 둘이니 더해서 2로 나누면 교환이 된다. 
            if (mode == 4) curStone = (int)V1.BLACK_STONE;
            else if (mode == 5) curStone = (int)V1.WHITE_STONE;
            else curStone = (curStone + 1) % 2;

            if (mode == 1 || mode == 2) isComputer = !isComputer;
            // 현재 둘 차례를 알려준다. 
            if (passTriggerOn) passCount++;
        }

        int CheckKey(int mode)
        {
            if (isComputer)
            {
                var ret = pOmok[curStone].placement(x, y, curStone);
                pOmok[curStone].getXY(ref x, ref y);

                if(ret != 11) //삼삼   //내부 체크는 1,1부터 시작 하나 그래픽 ui는 0,0부터 시작
                    UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White);
 
                return ret;

            }
            else if (aix != -1 && aiy != -1)
            {
                //내부 좌표는 1,1 시작 ui이는 0,0 시작이라 변환
                x = aix + 1;
                y = aiy + 1;

                aix = aiy = -1;

                var ret = pOmok[curStone].placement(x, y, curStone);
                if (ret != 11 && ret != 10) //삼삼 11 이미 놓여졌음 10
                    UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White);
                else
                {

                }

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

        public bool PlaygameLoop(int mode)
        {

            int result = CheckKey(mode);
            if (isComputer) pOmok[curStone].getXY(ref x, ref y);
            switch (result)
            {
                //// 키보드로부터 ESC키가 눌리면 게임을 종료한다. 
                case 15:
                    {
                        bool isWin = false;
                        if(curStone == 0) //흑
                        {
                            if(User.Color == eTeam.Black)
                            {
                                isWin = true;
                            }
                        }
                        else
                        {
                            if (User.Color == eTeam.White)
                            {
                                isWin = true;
                            }
                        }

                        Navigation.PushPopupAsync(new AIGameResult(isWin));
                        return false;
                    }

                case /*SASA*/11:
                    {

                    }
                    break;
                //// 오목이 되었으면 승자를 알리고, 계속할건지 물어본다. 
                //case FIVEMOK: return pDraw().endMsg(curStone);
                //// 착수가 불가능한 곳은 그 이유를 알린다. 
                //case OCCUPIED:
                //case SAMSAM:

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

                        ChangeStone(mode);
                    }
                    break;
                case 16:    //CHANGE
                    ChangeStone(mode);

                    UpdateTurnBackground(curStone == 0 ? eTeam.Black : eTeam.White);
                    break;
                default: break;
            }

            return true;
        }

        // main함수에서 이 함수를 호출하면 게임이 시작된다. 
        void PlayGame(int type, int mode)
        {
            pOmok[0] = factory.GetInstance(type * 10 + 0, mode);
            pOmok[1] = factory.GetInstance(type * 10 + 1, mode);


            InitGame(mode);
        }

        public void UpdateBattleInfo()
        {
            if (User.Color == eTeam.Black)
            {
                blackLabel.Text = User.myInfo.NickName;
                whiteLabel.Text = "알파목";

                bottom2picture.Source = null;

                if (User.myInfo.PhotoPath != null)
                    bottom1picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));

                

                return;
            }

            blackLabel.Text = "알파목";
            whiteLabel.Text = User.myInfo.NickName;

            bottom1picture.Source = null;

            if (User.myInfo.PhotoPath != null)
                bottom2picture.Source = ImageSource.FromUri(new Uri(User.myInfo.PhotoPath));
        }


        public bool isbegin = false;

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

                    absoluteLayout.Children.Add(background, new Rectangle(0, 0, 1, 1), AbsoluteLayoutFlags.All);

                    DrawLine();

                    TouchEffect touchEffect = new TouchEffect();
                    touchEffect.TouchAction += OnTouchEffectAction;
                    absoluteLayout.Effects.Add(touchEffect);
                });
                return false; // runs again, or false to stop
            });

        }

        iAd_IterstitialView iIterstitia;


        double screenx;
        double screeny;

        //square
        double boxwidth;
        double boxheight;

        double size = 19;  //default 19

        public SingleMatch()
        {
            InitializeComponent();
            ClearBoardState();

            InitBoardGrid();

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

            Device.StartTimer(TimeSpan.FromSeconds(1), () =>
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    try 
                    {
                        lock (this)
                        {
                            if (User.myInfo.ai_set_flag == true)
                            {
                                ClearBoardState();

                                PlayGame(User.myInfo.ai_rule, User.myInfo.ai_mode);
                                isbegin = true;
                                User.myInfo.ai_set_flag = false;

                                UpdateBattleInfo();
                            }

                            if (isbegin == true)
                            {
                                isbegin = PlaygameLoop(User.myInfo.ai_mode);

                                if (isbegin == false) //종료
                                {
                                    isbegin = false;

                                }
                            }
                        }
                    }
                    catch(Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                });

                return true;
            });

            iIterstitia = DependencyService.Get<iAd_IterstitialView>();

            Clicked.IsEnabled = false;


            MessagingCenter.Subscribe<SingleMatch>(this, "close", (sender) =>
            {
                Navigation.PopModalAsync();
            });

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

            absoluteLayout.Children.Add(aimMark, new Rectangle(xx, yy, xx_width, yy_height));
        }

        BoxView aimMark = null;

        public void UpdateAim(int x, int y)
        {
            DrawLayout(x, y, Color.AliceBlue);
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
                }, new Rectangle(x + (yy * boxwidth), y, 1, screeny - boxwidth));

            }


            for (int xx = 0; xx < size; xx++) //draw horizontal
            {
                absoluteLayout.Children.Add(new BoxView
                {
                    Color = Color.Black,
                }, new Rectangle(x, y + (xx * boxwidth), screenx - boxwidth, 1));

            }
            //화면 사이즈 조절을 위해 제일 바닥 라인을 그린다.

            absoluteLayout.Children.Add(new BoxView
            {
                Color = Color.Black,
            }, new Rectangle(0, y + (size * boxwidth), screenx, 1));

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

            UpdateAim(aimx,aimy);

        }
        public void RefreshAim()
        {
            UpdateAim(aimx, aimy);
        }


        protected override void OnAppearing()
        {
            base.OnAppearing();
        }

        protected override void OnDisappearing()
        {
           
        }
      
        public void ShowLeaveAd()
        {

            iIterstitia.ShowAd();
        }


        async void OnSettingClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushPopupAsync(new AISelect(this));
        }

        int retrycount = 0;

        void OnRetryClicked(object sender, System.EventArgs e)
        {
            if (retrycount != 0)
                iIterstitia.ShowAd();

            retrycount++;

            Clicked.IsEnabled = true;

            User.myInfo.ai_set_flag = true;
        }
        async void OnLeaveClicked(object sender, System.EventArgs e)
        {
            await Navigation.PushPopupAsync(new Confirm(this));

       //     var page = await Navigation.PopModalAsync();
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
                }, new Rectangle(xx, yy, xx_width, yy_height));
            }
            else
            {
                absoluteLayout.Children.Add(new ImageButton
                {
                    Source = ImageSource.FromResource("OMOK.Image.White.png"),
                    CornerRadius = (int)(xx_width / 2)
                }, new Rectangle(xx, yy, xx_width, yy_height));
            }

            UpdateAim(x, y);
        }

        void OnPutStone(object sender, System.EventArgs e)
        {
            if (isbegin == false)
                return;

            aix = aimx;
            aiy = aimy;
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



        //ai에 넘길 사용자가 선택한 좌표
        public int aix = -1, aiy = -1;

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

            absoluteLayout.Children.Add(LastStoneMark, new Rectangle(xx, yy, xx_width, yy_height));
        }


        BoxView LastStoneMark = null;

        public bool UpdateStone(int x, int y, eTeam status)
        {
            //내부 체크는 1,1부터 시작 하나 그래픽 ui는 0,0부터 시작

            x -= 1;
            y -= 1;

            DrawStone(x, y, status == eTeam.Black ? Color.Black : Color.White);

            DrawLastLayout(x, y, Color.Aqua);

            return true;
        }

      
    }
}