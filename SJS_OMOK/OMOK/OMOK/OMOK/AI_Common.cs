using OMOK.Network;
using OMOK.Popup;
using OMOK.Views;
using Rg.Plugins.Popup.Extensions;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Threading;
using Xamarin.Forms;

namespace OMOK
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

    public class AI
    {
        public BoardRenderer _renderer;

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
        COMOK[] pOmok = new COMOK[2];


        //ai에 넘길 사용자가 선택한 좌표
        public int aix = -1, aiy = -1;

        public void Init(int mode, BoardRenderer renderer)
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



            //기본 좌표계는 AI 는 1,1 부터 시작이나 UI는 0,0부터 시작
            //AI좌표는 -1,-1 해준다.
            if (mode == 5) _renderer.UpdateStone(x, y, eTeam.White, true);
            else _renderer.UpdateStone(x, y, eTeam.Black, true);
        }

        void InitGame(int mode, bool ispvp = false)
        {
            Init(mode, _renderer);

            if (ispvp == false)
            {
                if (mode == 1)
                    User.Color = eTeam.White;
                else
                {
                    User.Color = eTeam.Black;
                }
            }

            if (User.Color == eTeam.Black)
            {
                _renderer.UpdateTurnBackground(eTeam.Black);

                if(ispvp == true)
                NetProcess.SendPassThroughMessage(x-1, y-1, User.Color);
            }
            else
                _renderer.UpdateTurnBackground(eTeam.White);
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

        int CheckKey(int mode, bool isSend = false)
        {
            if (isComputer)
            {
                var ret = pOmok[curStone].placement(x, y, curStone);
                pOmok[curStone].getXY(ref x, ref y);

                if (ret != 11 && isSend  == false) //삼삼   //내부 체크는 1,1부터 시작 하나 그래픽 ui는 0,0부터 시작
                    _renderer.UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White);

                if (isSend == true) //네트워크는 기본 0,0 기준이라 변환해준다.
                    NetProcess.SendPassThroughMessage(x-1, y-1, User.Color);

                return ret;

            }
            else if (aix != -1 && aiy != -1)
            {
                if (isSend == false)
                {
                    //내부 좌표는 1,1 시작 ui이는 0,0 시작이라 변환
                    x = aix + 1;
                    y = aiy + 1;
                }
                else
                {
                    x = aix + 1;
                    y = aiy + 1;
                }

                aix = aiy = -1;

                var ret = pOmok[curStone].placement(x, y, curStone);
                if (ret != 11 && ret != 10 && isSend == false) //삼삼 11 이미 놓여졌음 10
                    _renderer.UpdateStone(x, y, curStone == 0 ? eTeam.Black : eTeam.White,true);

                if (isSend == true) //네트워크는 기본 0,0 기준이라 변환해준다.
                    NetProcess.SendPassThroughMessage(x-1, y-1, User.Color == eTeam.White ? eTeam.Black : eTeam.White);

                else
                {

                }

                return ret;
            }
            else return 0;
        }



        public bool PlaygameLoop(Page page, int mode, bool isSend = false)
        {

            int result = CheckKey(mode,isSend);
            if (isComputer) pOmok[curStone].getXY(ref x, ref y);
            switch (result)
            {
                //// 키보드로부터 ESC키가 눌리면 게임을 종료한다. 
                case 15:
                    {
                        bool isWin = false;
                        if (curStone == 0) //흑
                        {
                            if (User.Color == eTeam.Black)
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

                        if(User.Auto == false)
                        page.Navigation.PushPopupAsync(new AIGameResult(isWin));
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

                    _renderer.UpdateTurnBackground( curStone == 0 ? eTeam.Black : eTeam.White);
                    break;
                default: break;
            }

            return true;
        }

        // main함수에서 이 함수를 호출하면 게임이 시작된다. 
        public void PlayGame(int type, int mode,bool ispvp = false)
        {
            pOmok[0] = factory.GetInstance(type * 10 + 0, mode);
            pOmok[1] = factory.GetInstance(type * 10 + 1, mode);


            InitGame(mode, ispvp);
        }

    }
}
