using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace SJS_BADUK
{
    enum _GameStatus { NoLogin = 0, Login, Wait, Match, Play, Select, Calc, End, Demo, Obsever };
    enum _BW { None = -1, Black, White };


    struct _LastCapterOne
    {
        public Point m_pt;
        public int m_nCount;
    };


    public class BadukControl
    {
        double[,] dol_pos = new double[20, 20];
        double dol_count = 0;

        int del_White = 0;
        int del_Black = 0;
        _GameStatus GameStatus = _GameStatus.Play;

        double[,] del_pos = new double[20, 20];             /// 사석 지정 리스트 
                                                            /// 

        // 한 돌 잡았을 경우 정보. x,y,Count
        _LastCapterOne _LCO = new _LastCapterOne();

        public BoardRenderer _render;

        public void Init(BoardRenderer render)
        {
            _render = render;

            for (int y = 0; y < 20; y++)
            {
                for (int x = 0; x < 20; x++)
                {
                    dol_pos[(int)x, (int)y] = -1;            // 게임 돌 위치.

                    del_pos[(int)x, (int)y] = -1;           // 사석 체크 위치.
                }
            }
        }


        //좌표 변환 내부는 1,1 부터 시작이며
        //draw 관련함수는 0,0 부터 시작한다.
        public void Board_MouseLeftButtonDown(double x,double y)
        {
            int nBW = (int)dol_count % 2;

            Point pt = new Point(x, y);

            List<Point> delDol = new List<Point>();
            //List<Point> delDol_2 = new List<Point>();         자살수

            if (GameStatus == _GameStatus.Play)
            {
                 //timerSetBlack = timerSetWhite = 3;                // bug fix. 2009.7.23.


                if (dol_pos[(int)pt.X, (int)pt.Y] == -1)
                {
                    dol_pos[(int)pt.X, (int)pt.Y] = nBW;

                    System.Diagnostics.Debug.WriteLine("돌 놓음 : {0}, {1}, ({2})", pt.X.ToString(), pt.Y.ToString(), nBW.ToString());


                    //
                    //  돌 두는 자리의 주변 빈 공간 여부.
                    //
                    int nSpace = Common.Func.CheckAround(nBW, dol_pos, pt);
                    System.Diagnostics.Debug.WriteLine("CheckAround : {0})", nSpace);


                    //
                    //  Check Killed DOL.
                    //
                    int nTotalSpace = 0;
                    delDol = Common.Func.CheckAndKillBoard(nBW, ref dol_pos, pt, ref nTotalSpace);

                    if (delDol.Count == 0 && nSpace == 0)
                    {
                        dol_pos[(int)pt.X, (int)pt.Y] = -1;
                        return;
                    }


                    //
                    //      한점 잡은 후 바로 돌을 놓으려 할 경우를 막는다.
                    //
                    if (delDol.Count == 1)
                    {
                        if ((_LCO.m_nCount == (int)dol_count - 1) && pt == _LCO.m_pt)
                        {
                            dol_pos[(int)pt.X, (int)pt.Y] = -1;
                            return;
                        }

                        _LCO.m_nCount = (int)dol_count;
                        _LCO.m_pt = delDol[0];
                    }


                    System.Diagnostics.Debug.WriteLine("돌 삭제 리스트  : {0}", delDol.Count);



                    //
                    //          자살 수 체크 
                    //

                    nSpace = Common.Func.CheckSpace(nBW, dol_pos, pt);
                    System.Diagnostics.Debug.WriteLine("CheckSpace : {0})", nSpace);


                    if (delDol.Count == 0)
                    {
                        int nChk = 0;
                        if (nBW == 0) nChk = 1;     // Black 인경우 White
                        if (nBW == 1) nChk = 0;     // White 인 경우 Black 으로 검색

                        nTotalSpace = 0;

                        delDol = Common.Func.CheckAndKillBoard(nChk, ref dol_pos, pt, ref nTotalSpace);
                        if (delDol.Count > 0 && nTotalSpace == 0 && nSpace == 0)
                        {
                            Point ptSave = new Point(pt.X, pt.Y);
                            delDol.Add(ptSave);

                            System.Diagnostics.Debug.WriteLine("자살수 삭제 리스트  : {0}", delDol.Count);
                        }
                        else
                        {
                            // 빈공간이 있으므로,
                            // 삭제 리스트를 초기화 한다.

                            while (delDol.Count > 0)
                            {
                                delDol.RemoveAt(0);
                            }

                        }

                    }



                    dol_count++;
                }
                else // 이미 돌이 놓여있을 경우
                {
                    System.Diagnostics.Debug.WriteLine("이미 돌이 있음");
                    return;
                }

                if (nBW == 0)
                {
                    _render.DrawStone((int)pt.X-1, (int)pt.Y-1, Color.Black);


                    System.Diagnostics.Debug.WriteLine("Black put {0},{1} ", pt.X, pt.Y);

                }
                else
                {
                    _render.DrawStone((int)pt.X-1, (int)pt.Y-1, Color.White);

                    System.Diagnostics.Debug.WriteLine("White put {0},{1} ", pt.X, pt.Y);
                }


                // 죽은 돌 제거.        delDol
                if (delDol.Count > 0)
                {

                    if (nBW == 0)
                    {   // Black 을 둔 후 돌이 죽었으므로, 죽은 돌은 White
                        del_White += (int)delDol.Count;

                       

                    }
                    else
                    {
                        del_Black += (int)delDol.Count;
                      
                    }


                    System.Diagnostics.Debug.WriteLine("삭제 루틴 시작, 총 삭제 돌 {0}", delDol.Count);

                    for (int n = 0; n < delDol.Count; n++)
                    {
                        Point ptDol = delDol[n];
                        System.Diagnostics.Debug.WriteLine("삭제 위치 : {0}, {1}", ptDol.X, ptDol.Y);

                        dol_pos[(int)ptDol.X, (int)ptDol.Y] = -1;

                        _render.RemoveStone((int)ptDol.X-1, (int)ptDol.Y-1);
                    }

                    System.Diagnostics.Debug.WriteLine("삭제 루틴 완료, 총 삭제 돌 {0}", delDol.Count);


                }

            } // end if (GameStatus == _GameStatus.Play)





            if (GameStatus == _GameStatus.Select)
            {
               
            }


        }

    }
}
