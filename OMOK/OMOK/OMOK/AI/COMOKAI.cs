using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public class PointAI
    {
        public int x, y;
    };

    public class COMOKAI
    {
        public static int[,] nBoard = new int[ConstValue.SIZE + 2, ConstValue.SIZE + 2];
        public static int[,] nWeight = new int[ConstValue.SIZE + 2, ConstValue.SIZE + 2];
        public static PointAI[] p = new PointAI[10];
        static int top = -1;

        public bool isO_Four;
        public bool isF_Three;

        public PointAI l = new PointAI();  //left top, right bottom;
        public PointAI r = new PointAI();
        public COMOKRULE pRule = new COMOKRULE();
        public delegate int fp4(int a, int b, int c,int d);
        public delegate bool fp3(int a, int b, int c);

        public COMOKAI()
        {
            for(int i=0;i<10;i++ )
            {
                p[i] = new PointAI();
            }

            for (int i = 0; i < ConstValue.SIZE + 2; i++)
            {
                nBoard[i, 0] = nBoard[i, ConstValue.SIZE + 1] = (int)V1.WALL;
                nBoard[0, i] = nBoard[ConstValue.SIZE + 1, i] = (int)V1.WALL;
            }
        }

        public void SetBoard(int[,] arr, COMOKRULE pr)
        {
            pRule = pr;

            for (int i = 1; i <= ConstValue.SIZE; i++)
                for (int j = 1; j <= ConstValue.SIZE; j++)
                    nBoard[i, j] = arr[i, j];

            pRule.SetBoard(arr);
        }

        public void initWeight()
        {
            for (int i = 1; i <= ConstValue.SIZE; i++)
            {
                for (int j = 0; j <= ConstValue.SIZE; j++)
                {
                    nWeight[i, j] = 0;
                }
            }
        }

       public  bool IsEmpty(int x, int y)
        {
            bool r = (nBoard[y, x] == (int)V1.LINE);
            return r;
        }

        // 현재 바둑알이 놓여 있는 상하좌우의 좌표를 구한다. 
        public void GetRect(ref PointAI p1, ref PointAI p2)
        {
            p1.x = p1.y = ConstValue.SIZE;
            p2.x = p2.y = 0;
            for (int i = 1; i <= ConstValue.SIZE; i++)
            {
                for (int j = 0; j <= ConstValue.SIZE; j++)
                {
                    if (nBoard[i, j] == (int)V1.BLACK_STONE || nBoard[i, j] == (int)V1.WHITE_STONE)
                    {
                        if (p1.x > j) p1.x = j;
                        if (p2.x < j) p2.x = j;
                        if (p1.y > i) p1.y = i;
                        if (p2.y < i) p2.y = i;
                    }
                }
            }
        }

        // 탐색 범위를 위에서 구한 좌표에 +2씩 해서 정한다.  
        public void setRect()
        {
            GetRect(ref l, ref r);

            l.x -= 2; if (l.x < 1) l.x = 1;
            l.y -= 2; if (l.y < 1) l.y = 1;
            r.x += 2; if (r.x > ConstValue.SIZE) r.x = ConstValue.SIZE;
            r.y += 2; if (r.y > ConstValue.SIZE) r.y = ConstValue.SIZE;
        }

        // 0:돌, X:빈곳, #:벽이나 상대돌 
        // 000 (Open three) : 5 
        // 00X0(칸 띤 Open three) :4 
        // 000# : 2
        // 0X00# : 1
        // 돌의 상태에 따라 점수를 위와 같이 책정하였다. 
        public int IsThree(int x, int y, int nStone, int nDir)
        {
            int tx, ty;
            int cnt = 0;

            if ((nDir % 2) > 0)
                nDir -= 1;

            pRule.SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                if (pRule.IsEmpty(ref tx, ref ty, nStone, nDir + i))
                {
                    if (pRule.IsFour(tx, ty, nStone, nDir + i))
                    {
                        if (pRule.IsOpenFour(tx, ty, nStone, nDir + i) == 0)
                        {
                            if (pRule.IsLinked(tx, ty, nStone, nDir)) cnt = 2;
                            else cnt = 1;
                        }
                        else if (pRule.IsOpenFour(tx, ty, nStone, nDir + i) == 1)
                        {
                            if (pRule.IsLinked(tx, ty, nStone, nDir)) cnt = 5;
                            else cnt = 4;
                        }
                    }
                }
            }

            pRule.SetStone(x, y, (int)V1.LINE);
            return cnt;
        }

        // 삼에서 돌을 하나씩 들어냈다고 생각하면 똑같다.  
        public int IsTwo(int x, int y, int nStone, int nDir)
        {
            int tx, ty;
            int cnt = 0;
            int sum = 0;

            if ((nDir % 2) > 0)
                nDir -= 1;

            pRule.SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                if (pRule.IsEmpty(ref tx, ref ty, nStone, nDir + i))
                {
                    cnt = IsThree(tx, ty, nStone, nDir + i);
                    if (cnt > 0)
                    {
                        if (pRule.IsLinked(tx, ty, nStone, nDir))
                        {
                            if (cnt < 3) sum = 2;
                            else sum = 5;
                        }
                        else if (cnt < 3) sum = 1;
                        else sum = 4;
                    }
                }
            }
            pRule.SetStone(x, y, (int)V1.LINE);
            return sum;
        }

        // Open four가 되면 게임이 끝나는 상황이 되므로
        // 막힌4만 가지고 점수를 책정한다.
        // 돌이 2개일때(IsTwo)는 X 2
        // 돌이 3개일때(IsThree)는 X 4
        // 돌이 4개일때(IsFour)는 X 6
        public int IsFour(int x, int y, int nStone, int nDir)
        {
            int tx, ty;
            int cnt = 0;

            if ((nDir % 2) > 0)
                nDir -= 1;
            for (int i = 0; i < 2; i++)
            {
                if (pRule.IsFour(x, y, nStone, nDir + i))
                    if (pRule.IsOpenFour(x, y, nStone, nDir + i) == 0) cnt++;
            }

            if (cnt > 0)
            {
                cnt = 2;
                if (pRule.IsLinked(x, y, nStone, nDir)) cnt = 3;
            }
            return cnt;
        }

        public bool IsOpenFour(int x, int y, int nStone)
        {
            if (!IsEmpty(x, y)) return false;
            for (int i = 0; i < 8; i += 2)
            {
                if (1 == pRule.IsOpenFour(x, y, nStone, i)) return true;
            }

            return false;
        }

        public bool IsFive(int x, int y, int nStone)
        {
            if (!IsEmpty(x, y)) return false;

            if (pRule.IsFive(x, y, nStone)) return true;
            else return false;
        }

        public bool IsSix(int x, int y, int nStone)
        {
            if (!IsEmpty(x, y)) return false;

            if (pRule.IsSix(x, y, nStone)) return true;
            else return false;
        }

        public bool IsDoubleThree(int x, int y, int nStone)
        {
            if (!IsEmpty(x, y)) return false;

            if (pRule.IsDoubleThree(x, y, nStone)) return true;
            else return false;
        }

        public bool IsDoubleFour(int x, int y, int nStone)
        {
            if (!IsEmpty(x, y)) return false;

            if (pRule.IsDoubleFour(x, y, nStone)) return true;
            else return false;
        }

        public bool IsFourThree(int x, int y, int nStone)
        {
            int fCnt = 0;
            int tCnt = 0;

            if (!IsEmpty(x, y)) return false;
            for (int i = 0; i < 8; i += 2)
            {
                if (pRule.IsFour(x, y, nStone, i)) fCnt++;
                else if (pRule.IsOpenThree(x, y, nStone, i)) tCnt++;

                if (fCnt > 0 && tCnt > 0) return true;
            }

            return false;
        }

        // 각각의 포인트에서 점수를 책정하여 저장한다. 
        public void fillWeight(int myStone)
        {
            int cnt;
            int yourStone = (myStone + 1) % 2;
            fp4[] func = { IsTwo, IsThree, IsFour };

            for (int i = l.y; i <= r.y; i++)
            {
                for (int j = l.x; j <= r.x; j++)
                {
                    // IsTwo 일때는 X 2
                    // IsThree 일때는 X 4
                    // IsFour 일때는 X 6을 해서 점수를 책정 
                    for (int f = 0, k = 2; f < 3; f++, k += 2)
                    {
                        if (!IsEmpty(j, i)) continue;
                        for (int n = 0; n < 8; n += 2)
                        {
                            // 내돌과 상대돌을 동등하게 평가한다. 
                            nWeight[i, j] += k * func[f](j, i, myStone, n);
                            nWeight[i, j] += k * func[f](j, i, yourStone, n);
                        }
                    }
                }
            }
        }

        // 돌을 하나 놓아 끝날 수 있는 자리를 찾는다. 
        public bool findPutPoint(ref int x, ref int y, int nStone, fp3 fp)
        {
            for (int i = l.y; i <= r.y; i++)
            {
                for (int j = l.x; j <= r.x; j++)
                {
                    if (!IsEmpty(j, i)) continue;
                    if (fp(j, i, nStone))
                    {
                        x = j;
                        y = i;
                        return true;
                    }
                }
            }
            return false;
        }

        // 최고 점수가 같은 곳이 나올때 그 위치를 저장하여 
        // 랜덤하게 돌을 놓기 위해서  
        public void push(int x, int y)
        {
            if (top >= 9) return;
            ++top;
            p[top].x = x;
            p[top].y = y;
        }

        // 최종적으로 바둑알을 높기위해 최고값을 찾는다.
        // 최고값이 같은 곳이 있을때 랜덤하게 놓는다. 
        public void findPutOnPoint(ref int x, ref int y, int nStone)
        {
            int temp = 0;
            int max = -1;
            for (int i = l.y; i <= r.y; i++)
            {
                for (int j = l.x; j <= r.x; j++)
                {
                    if (!IsEmpty(j, i)) continue;
                    if (pRule.IsForbidden(j, i, nStone)) continue;
                    if (max < nWeight[i, j])
                    {
                        x = j;
                        y = i;
                        max = nWeight[i, j];
                        if (top >= 0) top = -1;
                        push(x, y);
                    }
                    else if (max == nWeight[i, j]) push(j, i);
                }
            }

            // max값이 두군데 이상에서 같을때 
            // 랜덤하게  둘곳을 찾는다. 
            if (++top > 1)
            {
                Random r = new Random();
                top = r.Next(1, 100) % top;
                x = p[top].x;
                y = p[top].y;
            }
        }

        // 상대가 열린3일때 막을 곳을 찾는다. 
        public virtual void FindOpenFour(int x, int y, int nStone)
        {
            int myStone = (nStone + 1) % 2;

            for (int i = l.y; i <= r.y; i++)
            {
                for (int j = l.x; j <= r.x; j++)
                {
                    if (!IsEmpty(j, i)) continue;
                    if (IsOpenFour(j, i, nStone))
                    {
                        if (isF_Three)
                        {
                            pRule.SetStone(j, i, myStone);
                            if (!IsFourThree(x, y, nStone) &&
                                !pRule.IsDoubleFour(x, y, nStone))
                                nWeight[i, j] += 200;
                            pRule.SetStone(j, i, (int)V1.LINE);
                        }
                        else
                        {
                            if (IsFourThree(j, i, nStone) ||
                                pRule.IsDoubleFour(j, i, nStone))
                                nWeight[i, j] += 500;
                        }

                        nWeight[i, j] += 200;
                    }
                }
            }
        }

        // 오목 class스에서 호출하는 함수.
        // 각 룰에 따라 금수조건이 다르기 때문에  
        // Rule마다 각기 다르게 정의되어있다. 
        public virtual void play(ref int x, ref int y, int myStone)
        {
            int yourStone = (myStone + 1) % 2;
            isO_Four = isF_Three = false;

            initWeight();
            GetRect(ref l, ref r);
            setRect();
            if (findPutPoint(ref x, ref y, myStone, IsFive)) return;
            if (findPutPoint(ref x, ref y, myStone, IsSix)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsFive)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsSix)) return;
            if (findPutPoint(ref x, ref y, myStone, IsOpenFour)) return;
            if (findPutPoint(ref x, ref y, myStone, IsFourThree)) return;
            if (findPutPoint(ref x, ref y, myStone, IsDoubleFour)) return;
            // 나의 돌이 열린4이면 바로 그 자리에 놓으면 되지만
            // 상대의 돌이 열린4일 경우 어느쪽으로 막을지가 중요하다.
            // 때문에 바로 리턴하지 않고 어느 곳을 막을 것인지 찾아봐야 한다. 
            if (findPutPoint(ref x, ref y, yourStone, IsOpenFour)) isO_Four = true;
            if (isO_Four)
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree)) isF_Three = true;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour)) isF_Three = true;
            }
            else
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree)) return;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour)) return;
            }
            fillWeight(myStone);

            if (findPutPoint(ref x,ref y, myStone, IsDoubleThree)) nWeight[y, x] += 100;
            if (findPutPoint(ref x,ref y, yourStone, IsDoubleThree)) nWeight[y, x] += 100;
            if (isO_Four) FindOpenFour(x, y, yourStone);

            findPutOnPoint(ref x, ref y, myStone);
        }
    }

    class CNORMALAI :  COMOKAI
{

        public override void play(ref int x, ref int y, int myStone)
        {
            int yourStone = (myStone + 1) % 2;

            isO_Four = isF_Three = false;
            initWeight();
            GetRect(ref l,ref r);
            setRect();

            if (findPutPoint(ref x, ref y, myStone, IsFive)) return;
            if (findPutPoint(ref x, ref y, myStone, IsSix)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsFive))
                if (!pRule.IsForbidden(x, y, myStone)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsSix))
                if (!pRule.IsForbidden( x,  y, myStone)) return;
            if (findPutPoint(ref x, ref y, myStone, IsOpenFour))
                if (!pRule.IsForbidden( x,  y, myStone)) return;
            if (findPutPoint(ref x, ref y, myStone, IsFourThree))
                if (!pRule.IsForbidden( x,  y, myStone)) return;
            if (findPutPoint(ref x, ref y, myStone, IsDoubleFour))
                if (!pRule.IsForbidden( x,  y, myStone)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsOpenFour)) isO_Four = true;
            if (isO_Four)
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree)) isF_Three = true;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour)) isF_Three = true;
            }
            else
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree))
                    if (!pRule.IsForbidden( x,  y, myStone)) return;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour))
                    if (!pRule.IsForbidden( x,  y, myStone)) return;
            }
            fillWeight(myStone);
            if (findPutPoint(ref x, ref y, yourStone, IsDoubleThree)) nWeight[y,x] = -100;

            if (isO_Four) FindOpenFour( x,  y, yourStone);
            findPutOnPoint(ref x, ref y, myStone);
        }

    };

class CRENJUBLACK : COMOKAI
{
        public void SetRenjuObject(COMOKRULE pObj)
        {
            pGomoku = pObj;
        }

        public override void play(ref int x, ref int y, int myStone)
        {
            COMOKRULE pObj;
            int yourStone = (myStone + 1) % 2;
            isO_Four = isF_Three = false;

            GetRect(ref l, ref r);
            setRect();
            initWeight();

            if (findPutPoint(ref x, ref y, myStone, IsFive)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsFive))
                if (!pRule.IsForbidden(x, y, myStone)) return;
            if (findPutPoint(ref x, ref y, yourStone, IsSix))
                if (!pRule.IsForbidden(x, y, myStone)) return;
            if (findPutPoint(ref x, ref y, myStone, IsOpenFour))
                if (!pRule.IsForbidden(x, y, myStone)) return;
            if (findPutPoint(ref x, ref y, myStone, IsFourThree))
                if (!pRule.IsForbidden(x, y, myStone)) return;
            pObj = pRule;
            pRule = pGomoku;
            if (findPutPoint(ref x, ref y, yourStone, IsOpenFour)) isO_Four = true;
            if (isO_Four)
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree)) isF_Three = true;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour)) isF_Three = true;
            }
            else
            {
                if (findPutPoint(ref x, ref y, yourStone, IsFourThree))
                    if (!pObj.IsForbidden(x, y, myStone)) return;
                if (findPutPoint(ref x, ref y, yourStone, IsDoubleFour))
                    if (!pObj.IsForbidden(x, y, myStone)) return;
            }
            fillWeight(myStone);
            if (findPutPoint(ref x, ref y, yourStone, IsDoubleThree)) nWeight[y,x] += 100;

            if (isO_Four) FindOpenFour(x, y, yourStone);
            pRule = pObj;
            findPutOnPoint(ref x, ref y, myStone);
        }

        COMOKRULE pGomoku;
};

class CRENJUWHITE :  COMOKAI
{
        public void SetRenjuObject(COMOKRULE pObj)
        {
            pRenju = pObj;
        }

        // 컴퓨터가 렌주룰 흰돌일 때는 Double four는 검사하지 않아도 되어서
        // 제 정의 하였다. 
        public override void FindOpenFour(int x, int y, int nStone)
        {
            int myStone = (nStone + 1) % 2;

            for (int i = l.y; i <= r.y; i++)
            {
                for (int j = l.x; j <= r.x; j++)
                {
                    if (!IsEmpty(j, i)) continue;
                    if (IsOpenFour(j, i, nStone))
                    {
                        if (isF_Three)
                        {
                            pRule.SetStone(j, i, myStone);
                            if (!IsFourThree(x, y, nStone)) nWeight[i,j] += 200;
                            pRule.SetStone(j, i, (int)V1.LINE);
                        }
                        else
                        {
                            if (IsFourThree(j, i, nStone)) nWeight[i,j] += 500;
                        }

                        nWeight[i,j] += 200;
                    }
                }
            }
        }

        public override void play(ref int x, ref int y, int myStone)
        {
            COMOKRULE pObj;
            int yourStone = (myStone + 1) % 2;

            isO_Four = isF_Three = false;
            GetRect(ref l, ref r);
            setRect();
            initWeight();

            if (findPutPoint(ref x, ref y, myStone,IsFive)) return;
            if (findPutPoint(ref x, ref y, myStone,IsSix)) return;
            if (findPutPoint(ref x, ref y, yourStone,IsFive)) return;
            if (findPutPoint(ref x, ref y, myStone,IsOpenFour)) return;
            if (findPutPoint(ref x, ref y, myStone,IsFourThree)) return;
            pObj = pRule;
            pRule = pRenju;
            if (findPutPoint(ref x, ref y, yourStone,IsOpenFour)) isO_Four = true;
            if (isO_Four)
            {
                if (findPutPoint(ref x, ref y, yourStone,IsFourThree)) isF_Three = true;
            }
            else
            {
                if (findPutPoint(ref x, ref y, yourStone,IsFourThree))
                    if (!pRule.IsForbidden(x, y, myStone)) return;
            }
            fillWeight(myStone);

            if (isO_Four) FindOpenFour(x, y, yourStone);
            if (findPutPoint(ref x, ref y, yourStone,IsDoubleThree)) nWeight[y,x] = -50;
            if (findPutPoint(ref x, ref y, yourStone,IsDoubleFour)) nWeight[y,x] = -50;
            if (findPutPoint(ref x, ref y, yourStone,IsSix)) nWeight[y,x] = -50;

            pRule = pObj;
            if (findPutPoint(ref x, ref y, myStone,IsDoubleThree)) nWeight[y,x] += 100;
            findPutOnPoint(ref x, ref y, myStone);
        }


        COMOKRULE pRenju;
};


}
