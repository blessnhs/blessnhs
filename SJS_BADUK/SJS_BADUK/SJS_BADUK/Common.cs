using System;
using System.Collections.Generic;
using Xamarin.Forms;
using System.Text;
enum DOL { Black = 0, White };

namespace Common
{
    public enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    }

    public class Func
    {
        public static double GetBoardPos(double n)
        {
            //                int nR = (int)n / 25;


            double dR = (System.Math.Round(n * 10 / 25) / 10);
            int nR = (int)System.Math.Round(dR);

            //System.Diagnostics.Debug.WriteLine("double {0}, int {1} ", dR, nR);


            if (nR == 0) nR = 1;

            return (double)nR * 25;
        }


        public static Point GetXY(Point pt)
        {
            int nX = (int)pt.X / 25;
            int nY = (int)pt.Y / 25;

            Point ptR = new Point();
            ptR.X = nX;
            ptR.Y = nY;

            return ptR;
        }

        public static void GameInit(double[,] dol_pos, ref double[,] del_pos)
        {
            for (double y = 0; y < 20; y++)
            {
                for (double x = 0; x < 20; x++)
                {
                    dol_pos[(int)x, (int)y] = -1;            // 게임 돌 위치.

                    del_pos[(int)x, (int)y] = -1;           // 사석 체크 위치.
                }
            }
        }


        public static int CheckSpace(int nBW, double[,] dol_pos, Point pt)       // 2009.7.31.
        {
            int nSpace = 0;

            // Up, Right, Down, Left 
            if (pt.Y > 1)
            {
                if (dol_pos[(int)pt.X, (int)pt.Y - 1] == -1)
                    //|| dol_pos[(int)pt.X, (int)pt.Y - 1] == nBW)        // UP
                    nSpace++;
            }

            if (pt.X < 19)
            {
                if (dol_pos[(int)pt.X + 1, (int)pt.Y] == -1)
                    //|| dol_pos[(int)pt.X + 1, (int)pt.Y] == nBW)            // Right
                    nSpace++;
            }

            if (pt.Y < 19)
            {
                if (dol_pos[(int)pt.X, (int)pt.Y + 1] == -1)
                    //|| dol_pos[(int)pt.X, (int)pt.Y+1] == nBW)            // Bottom
                    nSpace++;
            }

            if (pt.X > 1)
            {
                if (dol_pos[(int)pt.X - 1, (int)pt.Y] == -1)
                    //|| dol_pos[(int)pt.X - 1, (int)pt.Y] == nBW)            // Left
                    nSpace++;
            }


            return nSpace;

        }

        public static int CheckAround(int nBW, double[,] dol_pos, Point pt)       // 2009.7.31.
        {
            int nSpace = 0;

            // Up, Right, Down, Left 
            if (pt.Y > 1)
            {
                if (dol_pos[(int)pt.X, (int)pt.Y - 1] == -1
                    || dol_pos[(int)pt.X, (int)pt.Y - 1] == nBW)        // UP
                    nSpace++;
            }

            if (pt.X < 19)
            {
                if (dol_pos[(int)pt.X + 1, (int)pt.Y] == -1
                    || dol_pos[(int)pt.X + 1, (int)pt.Y] == nBW)            // Right
                    nSpace++;
            }

            if (pt.Y < 19)
            {
                if (dol_pos[(int)pt.X, (int)pt.Y + 1] == -1
                    || dol_pos[(int)pt.X, (int)pt.Y + 1] == nBW)            // Bottom
                    nSpace++;
            }

            if (pt.X > 1)
            {
                if (dol_pos[(int)pt.X - 1, (int)pt.Y] == -1
                    || dol_pos[(int)pt.X - 1, (int)pt.Y] == nBW)            // Left
                    nSpace++;
            }


            return nSpace;

        }

        public static List<Point> CheckAndKillBoard(int nBW, ref double[,] arrayPos, Point pt, ref int nTotalSpace)
        {
            // nBW == 0 Black, nBW == 1 White
            // Up, Right, Down, Left
            //
            List<Point> DelDol = new List<Point>();

            int X = (int)pt.X;
            int Y = (int)pt.Y;

            int nChk = 0;
            if (nBW == 0) nChk = 1;     // Black 인경우 White
            if (nBW == 1) nChk = 0;     // White 인 경우 Black 으로 검색

            int nSpace = 0;
            int nSpaceRef = 0;
            if (pt.Y > 1)
            {
                if ((arrayPos[(int)pt.X, (int)pt.Y - 1]) == nChk)
                {
                    // 만약 흰돌(1) 이면, 검정돌 기준으로 체크
                    // 만약 검정돌(0)이면, 흰돌 기준으로 빈공간 체크
                    List<Point> arrDol = new List<Point>();

                    nSpaceRef++;
                    nSpace = EraseCheck(nChk, arrayPos, X, Y - 1, 1, ref arrDol);      // UP 

                    nTotalSpace += nSpace;

                    System.Diagnostics.Debug.WriteLine(" UP Space : {0} , Space Ref 유효 {1}, 돌갯수 {2} "
                        , nSpace, nSpaceRef, arrDol.Count);

                    if (nSpace == 0)
                    {
                        DeleteDol(arrDol);

                        for (int n = 0; n < arrDol.Count; n++)  // 검색된 삭제돌을 반환리스트에 추가.
                        {
                            Point ptDel = arrDol[n];
                            Point ptSave = new Point(ptDel.X, ptDel.Y);

                            if (ListExistCheck(DelDol, ptSave))
                                DelDol.Add(ptSave);
                        }
                    }
                }

            }

            if (pt.X < 19)
            {
                if ((arrayPos[(int)pt.X + 1, (int)pt.Y]) == nChk)
                {
                    // 만약 흰돌(1) 이면, 검정돌 기준으로 체크
                    // 만약 검정돌(0)이면, 흰돌 기준으로 빈공간 체크
                    List<Point> arrDol = new List<Point>();

                    nSpaceRef++;
                    nSpace = EraseCheck(nChk, arrayPos, X + 1, Y, 2, ref arrDol);    // Right

                    nTotalSpace += nSpace;

                    System.Diagnostics.Debug.WriteLine(" Right Space : {0} , Space Ref 유효 {1}, 돌갯수 {2} "
                        , nSpace, nSpaceRef, arrDol.Count);

                    if (nSpace == 0)
                    {
                        DeleteDol(arrDol);

                        for (int n = 0; n < arrDol.Count; n++)  // 검색된 삭제돌을 반환리스트에 추가.
                        {
                            Point ptDel = arrDol[n];
                            Point ptSave = new Point(ptDel.X, ptDel.Y);

                            if (ListExistCheck(DelDol, ptSave))
                                DelDol.Add(ptSave);
                        }
                    }
                }

            }

            if (pt.Y < 19)
            {
                if ((arrayPos[(int)pt.X, (int)pt.Y + 1]) == nChk)
                {
                    // 만약 흰돌(1) 이면, 검정돌 기준으로 체크
                    // 만약 검정돌(0)이면, 흰돌 기준으로 빈공간 체크
                    List<Point> arrDol = new List<Point>();

                    nSpaceRef++;
                    nSpace = EraseCheck(nChk, arrayPos, X, Y + 1, 3, ref arrDol);  // Down
                    System.Diagnostics.Debug.WriteLine(" Down Space : {0} , Space Ref 유효 {1}, 돌갯수 {2} "
                        , nSpace, nSpaceRef, arrDol.Count);

                    nTotalSpace += nSpace;


                    if (nSpace == 0)
                    {
                        DeleteDol(arrDol);

                        for (int n = 0; n < arrDol.Count; n++)  // 검색된 삭제돌을 반환리스트에 추가.
                        {
                            Point ptDel = arrDol[n];
                            Point ptSave = new Point(ptDel.X, ptDel.Y);

                            if (ListExistCheck(DelDol, ptSave))
                                DelDol.Add(ptSave);
                        }
                    }
                }

            }

            if (pt.X > 1)
            {
                if ((arrayPos[(int)pt.X - 1, (int)pt.Y]) == nChk)
                {
                    // 만약 흰돌(1) 이면, 검정돌 기준으로 체크
                    // 만약 검정돌(0)이면, 흰돌 기준으로 빈공간 체크
                    List<Point> arrDol = new List<Point>();

                    nSpaceRef++;
                    nSpace = EraseCheck(nChk, arrayPos, X - 1, Y, 4, ref arrDol);   // Left

                    nTotalSpace += nSpace;

                    System.Diagnostics.Debug.WriteLine(" Left Space : {0} , Space Ref 유효 {1}, 돌갯수 {2} "
                        , nSpace, nSpaceRef, arrDol.Count);

                    if (nSpace == 0)
                    {
                        DeleteDol(arrDol);

                        for (int n = 0; n < arrDol.Count; n++)  // 검색된 삭제돌을 반환리스트에 추가.
                        {
                            Point ptDel = arrDol[n];
                            Point ptSave = new Point(ptDel.X, ptDel.Y);

                            if (ListExistCheck(DelDol, ptSave))
                                DelDol.Add(ptSave);
                        }
                    }
                }

            }

            return DelDol;
            //                System.Diagnostics.Debug.WriteLine(" Space : {0} , Space Ref 유효 {1}", nSpace, nSpaceRef);

        }


        public static List<Point> CheckSuicide(int nBW, ref double[,] arrayPos, Point pt)
        {
            List<Point> DelDol = new List<Point>();

            int X = (int)pt.X;
            int Y = (int)pt.Y;

            // 현재 색상의 돌에 Space == 0 이면 모두 삭제리스트에 추가한다.


            return DelDol;

        }

        public static int EraseCheck(int nChk, double[,] _arrayPos, int X, int Y, int nDirect, ref List<Point> listDol)
        {
            int nSpace = 0;

            // space check ==>   1. top --> right --> bottom --> left   ( range : 1 ~ 19 )


            int nCheckedCnt = listDol.Count;

            if (nCheckedCnt > 0)
            {
                Point ptChk = new Point(X, Y);
                for (int nCnt = 0; nCnt < nCheckedCnt; nCnt++)
                {
                    if (ptChk == listDol[nCnt])
                        return nSpace;
                }
            }

            listDol.Add(new Point(X, Y));

            System.Diagnostics.Debug.WriteLine("EraseCheck listDol count : {0}", listDol.Count);


            //
            // TOP
            //
            if (Y > 1 && nDirect != 3)
            {
                if (_arrayPos[X, Y - 1] == -1)
                {
                    nSpace++;
                    //return nSpace;
                }
                else
                {
                    if (_arrayPos[X, Y - 1] == nChk)
                        nSpace += EraseCheck(nChk, _arrayPos, X, Y - 1, 1, ref listDol);

                }
            }

            //
            //  RIGHT
            //
            if (X < 19 && nDirect != 4)
            {
                if (_arrayPos[X + 1, Y] == -1)
                {
                    nSpace++;
                    //return nSpace;
                }
                else
                {
                    if (_arrayPos[X + 1, Y] == nChk)
                        nSpace += EraseCheck(nChk, _arrayPos, X + 1, Y, 2, ref listDol);
                }
            }

            //
            // Bottom
            //
            if (Y < 19 && nDirect != 1)
            {
                if (_arrayPos[X, Y + 1] == -1)
                {
                    nSpace++;
                    //return nSpace;
                }
                else
                {
                    if (_arrayPos[X, Y + 1] == nChk)
                        nSpace += EraseCheck(nChk, _arrayPos, X, Y + 1, 3, ref listDol);
                }
            }

            //
            //  LEFT
            //
            if (X > 1 && nDirect != 2)
            {
                if (_arrayPos[X - 1, Y] == -1)
                {
                    nSpace++;
                    //return nSpace;
                }
                else
                {
                    if (_arrayPos[X - 1, Y] == nChk)
                        nSpace += EraseCheck(nChk, _arrayPos, X - 1, Y, 4, ref listDol);
                }
            }


            return nSpace;
        }

        public static void DeleteDol(List<Point> arrDol)
        {
            int nCnt = arrDol.Count;

            for (int n = 0; n < nCnt; n++)
            {
                Point ptDol = arrDol[n];

                System.Diagnostics.Debug.WriteLine(" 삭제된 돌 : {0} ", ptDol);
            }
        } // end public static void DeleteDol(ref double [,]arrayPos, List<Point> arrDol)

        public static bool ListExistCheck(List<Point> DelDol, Point ptSave)
        {
            bool bAdd = true;
            int nCnt = DelDol.Count;

            for (int nLoop = 0; nLoop < nCnt; nLoop++)
            {
                Point pt = DelDol[nLoop];

                if (pt == ptSave)
                {
                    bAdd = false;
                    break;
                }
            }

            return bAdd;
        }


        //
        //          사석 체크
        //
        static public void CheckCaptured(double[,] dol_pos, ref double[,] del_pos, Point pt)
        {


            int X = (int)pt.X;
            int Y = (int)pt.Y;

            int nBW = (int)dol_pos[X, Y];     // Black 0 , White 1 

            if (nBW == -1)      // 공백이면 리턴.
            {

                return;
            }

            // 삭제 표시 또는 기존 삭제 표시 해제.
            int nSetReset = 0;

            if (del_pos[X, Y] == 1)
            {
                nSetReset = 0;          // 삭제 표시 해제.

                del_pos[X, Y] = 0;
            }
            else
            {
                nSetReset = 1;          // 삭제 표시 설정
                del_pos[X, Y] = 1;
            }



            // up 
            if (Y > 1)
            {
                if (dol_pos[X, Y - 1] == nBW)
                {
                    SearchLinkDOL(dol_pos, X, Y - 1, nBW, ref del_pos, 1, nSetReset);        // up direction
                }
            }

            // right
            if (X < 19)
            {
                if (dol_pos[X + 1, Y] == nBW)
                {
                    SearchLinkDOL(dol_pos, X + 1, Y, nBW, ref del_pos, 2, nSetReset);        // right direction
                }
            }

            // down
            if (Y < 19)
            {
                if (dol_pos[X, Y + 1] == nBW)
                {
                    SearchLinkDOL(dol_pos, X, Y + 1, nBW, ref del_pos, 3, nSetReset);        // down direction
                }
            }


            // left
            if (X > 1)
            {
                if (dol_pos[X - 1, Y] == nBW)
                {
                    SearchLinkDOL(dol_pos, X - 1, Y, nBW, ref del_pos, 4, nSetReset);        // left direction
                }
            }


            System.Diagnostics.Debug.WriteLine("End CheckCaptured");

            //
            // Debug
            //

            int deletedDol = 0;
            for (double y = 1; y < 20; y++)
            {
                for (double x = 1; x < 20; x++)
                {
                    if (del_pos[(int)x, (int)y] == 1)           // 사석 체크 위치.
                    {
                        deletedDol++;

                        System.Diagnostics.Debug.WriteLine("del {0}, {1}", x, y);

                    }
                }
            }

            System.Diagnostics.Debug.WriteLine("deleted dol check {0}", deletedDol);

        }

        static public void SearchLinkDOL(double[,] dol_pos, int X, int Y, int nBW, ref double[,] del_pos, int nDirect, int nSetReset)
        {
            //
            // up, right, down, left 순으로 동일색 돌 검색 
            // nBW = 0 블랙, 1 화이트 
            //

            if (dol_pos[X, Y] == nBW)
            {
                if (nSetReset == 1)
                {

                    if (del_pos[X, Y] == 1)
                        return;

                    del_pos[X, Y] = 1;                  // 삭제 표시 Check.
                }

                if (nSetReset == 0)
                {
                    if (del_pos[X, Y] == 0)
                        return;

                    del_pos[X, Y] = 0;                  // 삭제 표시 Un-Check.
                }

            }
            else
                return;


            // up 검색.
            if (Y > 1 && nDirect != 3)
            {
                //if (dol_pos[X, Y - 1] != nBW)
                //    return;

                SearchLinkDOL(dol_pos, X, Y - 1, nBW, ref del_pos, 1, nSetReset);      // up research
            }

            // right 검색.
            if (X < 19 && nDirect != 4)
            {
                //if (dol_pos[X+1, Y] != nBW)
                //    return;

                SearchLinkDOL(dol_pos, X + 1, Y, nBW, ref del_pos, 2, nSetReset);      // right research
            }

            // down 검색.
            if (Y < 19 && nDirect != 1)
            {
                //if (dol_pos[X, Y+1] != nBW)
                //    return;

                SearchLinkDOL(dol_pos, X, Y + 1, nBW, ref del_pos, 3, nSetReset);      // down research
            }

            // left 검색.
            if (X > 1 && nDirect != 2)
            {
                //if (dol_pos[X - 1, Y] != nBW)
                //    return;

                SearchLinkDOL(dol_pos, X - 1, Y, nBW, ref del_pos, 4, nSetReset);      // right research
            }
        }


        //static public void ShowXmark(double[,] del_pos, ref Canvas board)
        //{
        //    // 1. board 의 Xmark 모두 제거 후, 
        //    // 2. del_pos 의 값으로 Xmark 추가 표시


        //    //  1. 제거
        //    DeleteXmark(ref board);

        //    //  2. 설정
        //    int nDelCount = 0;

        //    for (int nX = 1; nX < 20; nX++)
        //    {
        //        for (int nY = 1; nY < 20; nY++)
        //        {
        //            if (del_pos[nX, nY] == 1)
        //            {
        //                Xmark mark = new Xmark();

        //                Point pt = new Point();
        //                pt.X = nX;
        //                pt.Y = nY;
        //                mark.setPos(pt);



        //                mark.SetValue(Canvas.LeftProperty, (double)nX * 25 - 8);
        //                mark.SetValue(Canvas.TopProperty, (double)nY * 25 - 8);

        //                board.Children.Add(mark);

        //                nDelCount++;
        //            }
        //        }
        //    }

        //    System.Diagnostics.Debug.WriteLine("ShowXmark : {0}", nDelCount);

        //}


        //
        //      삭제 지정된 돌의 정보를 지우고, 화면에서 정리 한다.
        //
        //static public void ResultDisplayBoard(ref Canvas board, ref double[,] dol_pos, ref double[,] del_pos, ref int nWhiteKilled, ref int nBlackKilled)
        //{
        //    //
        //    //      1. X 표시 삭제
        //    //      2. board 에 del_pos 돌들을 삭제 후 killed stone 에 추가
        //    //
        //    DeleteXmark(ref board);



        //    for (int y = 1; y <= 19; y++)
        //    {
        //        for (int x = 1; x <= 19; x++)
        //        {
        //            if (del_pos[x, y] == 1)        // 1 = mark, 0 = unmark
        //            {
        //                if (dol_pos[x, y] == 0)
        //                {
        //                    // 0 ->Black,       검은돌이 잡혔으니, 백선수에게 +
        //                    nBlackKilled++;
        //                }

        //                if (dol_pos[x, y] == 1)
        //                {
        //                    // 1 -> white       흰돌이 잡혔으니, 흑선수에게 +
        //                    nWhiteKilled++;


        //                }



        //                //
        //                // Search Board & delete DOL Object
        //                //

        //                int nItemCnt = board.Children.Count;
        //                for (int nCnt = 0; nCnt < nItemCnt; nCnt++)
        //                {
        //                    String strName = board.Children.ElementAt(nCnt).ToString();
        //                    //System.Diagnostics.Debug.WriteLine( "element name {0}", strName );


        //                    Point dolPos = new Point();
        //                    dolPos.X = dolPos.Y = 0;

        //                    if (strName == "SL_Baduk.B_Dol" || strName == "SL_Baduk.W_Dol")
        //                    {
        //                        if (strName == "SL_Baduk.B_Dol")
        //                        {
        //                            B_Dol dol = (B_Dol)board.Children.ElementAt(nCnt);
        //                            dolPos = dol.getPos();

        //                        }

        //                        if (strName == "SL_Baduk.W_Dol")
        //                        {
        //                            W_Dol dol = (W_Dol)board.Children.ElementAt(nCnt);
        //                            dolPos = dol.getPos();

        //                        }


        //                        if (dolPos.X == x && dolPos.Y == y)
        //                        {
        //                            board.Children.RemoveAt(nCnt);
        //                            nCnt--;
        //                            nItemCnt--;

        //                            System.Diagnostics.Debug.WriteLine("ResultDisplayBoard  --> Delete {0} , {1}"
        //                                , x, y);


        //                            dol_pos[x, y] = -1;         // 빈공간으로 set.
        //                        }
        //                    }
        //                }

        //            }
        //        }
        //    }//for (int y = 1; y <= 19; y++)

        //}// static public void ResultDisplayBoard(ref Canvas board, ref double[,] dol_pos, ref double[,] del_pos)



        //
        //      계가.               2009..7.12
        //
        //static public void ResultDisplayGame(ref Canvas board, double[,] dol_pos, ref int nBlackArea, ref int nWhiteArea)
        //{

        //    //
        //    // 각각의 빈공간을 up, right, down, left 를 비교 하여 
        //    // 같은 색의 돌인 경우 집으로 계산.
        //    int nB = 0;
        //    int nW = 0;
        //    int nSpace = 0;     // 공배
        //    int nDraw = 0;      // 빈집

        //    //dol_pos[1, 1] = 0;
        //    //dol_pos[2, 1] = 1;
        //    //dol_pos[1, 2] = 0;
        //    //dol_pos[2 ,3] = 1;
        //    //dol_pos[1, 3] = 0;
        //    //dol_pos[2, 3] = 1;


        //    for (int y = 1; y <= 19; y++)
        //    {
        //        for (int x = 1; x <= 19; x++)
        //        {
        //            //x = 2;
        //            //y = 2;

        //            if (dol_pos[x, y] == -1)            // 공백 
        //            {
        //                // up, right, down, left 검색 후.
        //                // 최초 돌이 동일색상이면 집으로 계산

        //                int nU = 0, nR = 0, nD = 0, nL = 0;

        //                nU = CheckUP(dol_pos, x, y);
        //                nR = CheckRight(dol_pos, x, y, nU);
        //                nD = CheckDown(dol_pos, x, y, nR);
        //                nL = CheckLeft(dol_pos, x, y, nD);

        //                if (nL != -2)
        //                {

        //                    if (nL == 0)
        //                    {
        //                        XBlack black = new XBlack();
        //                        black.SetValue(Canvas.LeftProperty, (double)(x * 25 - 5));
        //                        black.SetValue(Canvas.TopProperty, (double)(y * 25 - 5));

        //                        board.Children.Add(black);

        //                        nB++;
        //                    }

        //                    if (nL == 1)
        //                    {
        //                        XWhite white = new XWhite();
        //                        white.SetValue(Canvas.LeftProperty, (double)(x * 25 - 5));
        //                        white.SetValue(Canvas.TopProperty, (double)(y * 25 - 5));

        //                        board.Children.Add(white);

        //                        nW++;
        //                    }
        //                }
        //                else
        //                {
        //                    if (nL == -1)
        //                        nDraw++;
        //                    else
        //                    {
        //                        Xmark mark = new Xmark();
        //                        mark.SetValue(Canvas.LeftProperty, (double)(x * 25 - 8));
        //                        mark.SetValue(Canvas.TopProperty, (double)(y * 25 - 8));

        //                        board.Children.Add(mark);

        //                        nSpace++;
        //                    }
        //                }


        //                // -1 (경계), 0 (블랙), 1 (화이트)


        //            }
        //        }
        //    }

        //    nBlackArea = nB;
        //    nWhiteArea = nW;

        //    System.Diagnostics.Debug.WriteLine("집계산 결과 : B {0}, W {1}, Space {2}, Draw {3} ", nB, nW, nSpace, nDraw);


        //}

        static public int CheckUP(double[,] dol_pos, int x, int y)
        {
            int nRet = -1;

            for (int n = y; n >= 1; n--)
            {
                if (dol_pos[x, n] != -1.0)
                {
                    return (int)dol_pos[x, n];
                }
            }

            return nRet;
        }
        static public int CheckRight(double[,] dol_pos, int x, int y, int BW)
        {
            int nRet = BW;

            for (int n = x; n <= 19; n++)
            {
                if (dol_pos[n, y] != -1.0)
                {
                    if (BW == -1)
                        return (int)dol_pos[n, y];
                    else
                    {
                        if (BW == dol_pos[n, y])
                        {
                            return BW;
                        }
                        else
                        {
                            return -2;      //          unmatch
                        }
                    }
                }
            }

            return nRet;
        }
        static public int CheckDown(double[,] dol_pos, int x, int y, int BW)
        {
            int nRet = BW;

            if (nRet == -2) return nRet;

            for (int n = y; n <= 19; n++)
            {
                if (dol_pos[x, n] != -1.0)
                {
                    if (BW == -1)
                        return (int)dol_pos[x, n];
                    else
                    {
                        if (BW == dol_pos[x, n])
                        {
                            return BW;
                        }
                        else
                        {
                            return -2;      //
                        }
                    }
                }
            }

            return nRet;
        }
        static public int CheckLeft(double[,] dol_pos, int x, int y, int BW)
        {
            int nRet = BW;

            if (nRet == -2) return nRet;

            for (int n = x; n >= 1; n--)
            {
                if (dol_pos[n, y] != -1.0)
                {
                    if (BW == -1)
                        return (int)dol_pos[n, y];
                    else
                    {
                        if (BW == dol_pos[n, y])
                        {
                            return BW;
                        }
                        else
                        {
                            return -2;      //
                        }
                    }
                }
            }

            return nRet;
        }

    }
}