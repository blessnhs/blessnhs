using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public static class ConstValue
    {
        public static int SIZE = 15;
        public static int CENTER = ((SIZE + 2) / 2);
        public static int FINISH = -1;
        public static int TIED = 2;
    }


    // 바둑판은 배경은 녹색, 줄은 검은 색으로 한다.
    // 바둑돌은 흰색과 검은색으로 한다.
    enum V1 { BLACK_STONE, WHITE_STONE, CURSOR, BLACKWIN, WHITEWIN, TIE, LINE, WALL };
    enum V2 { OCCUPIED = 10, SAMSAM, SASA, SIXMOK, NOTUNDO, FIVEMOK, CHANGE, PASS };
    enum V3 { HIDE, SHOW };
    public class COMOKRULE
    {
        public static int[,] nBoard = new int[ConstValue.SIZE + 2, ConstValue.SIZE + 2];
        public COMOKRULE()
        {
            for (int i = 0; i < ConstValue.SIZE + 2; i++)
            {
                //바둑판 밖의 배열의 끝을 LINE이나 STONE이 아닌 
                //다른 숫자로 셋팅함으로써 경계임을 나타낸다. 
                nBoard[i, 0] = nBoard[i, ConstValue.SIZE + 1] = (int)V1.WALL;
                nBoard[0, i] = nBoard[ConstValue.SIZE + 1, i] = (int)V1.WALL;
            }
        }

        // 착수하기 전의 바둑판 상태를 복사한다. 
        public void SetBoard(int[,] arr)
        {
            for (int i = 1; i <= ConstValue.SIZE; i++)
                for (int j = 1; j <= ConstValue.SIZE; j++)
                    nBoard[i, j] = arr[i, j];
        }

        // x, y좌표에 돌을 놓거나 들어낸다.
        // 들어낸다는 말은 그곳에  Line을 다시 그려넣는 걸 의미한다. 
        public void SetStone(int x, int y, int nStone)
        {
            nBoard[y, x] = nStone;
        }

        // 8개의 숫자는 8방향을 나타낸다.
        // 배열의 순서대로 서, 동, 북, 남, 서북, 동남, 남서, 북동
        // (동, 서) (남 북) (서북, 동남) (남서, 북동)이 한쌍이되어
        // 가로 세로 양 대각선의 양쪽을 dx, dy로 표시한 Table 
        public void GetDirTable(ref int x, ref int y, int nDir)
        {
            //          서  동  북  남  북서 남동 남서 북동 
            int[] dx = { -1, 1, 0, 0, -1, 1, -1, 1 };
            int[] dy = { 0, 0, -1, 1, -1, 1, 1, -1 };

            x = dx[nDir];
            y = dy[nDir];
        }

        public bool IsEmpty(int x, int y)
        {
            return nBoard[y, x] == (int)V1.LINE;
        }

        // 주어진 방향으로 좌표를 옮겨 빈곳을 찾아 
        // 그곳의 좌료를 알려준다. 
        public bool IsEmpty(ref int x, ref int y, int nStone, int nDir)
        {
            int dx = 0, dy = 0;

            // 방향 Table에서 dx, dy값을 가져온다.
            // 만약 nDir이 0이라면 dx = -1, dy = 0이다. 
            GetDirTable(ref dx, ref dy, nDir);
            // x는 1 y는 0이니까 x += dx, y += dy 이 수식에 의해서 
            // x는 점점 작아저 왼쪽으로 가고, y는 변함이 없으니 한자리
            // 그래서 가로로 왼쪽에 nStone이 아닌 곳에서 멈추게 된다. 
            for (; nBoard[y,x] == nStone; x += dx, y += dy) ;

            // nStone이 아니라 해서 전부 (int)V1.LINE이라 할 수 없다.
            // 다른 돌이거나 바둑판을 벗어난 곳 일수도 있으니
            // 그곳이 (int)V1.LINE일 때만 참을 반환한다. 
            return nBoard[y, x] == (int)V1.LINE;
        }

        // 주어진 좌표에 한점을 착점하고,
        // 주어진 방향(nDir)으로 
        // 연속된 돌이 몇개인지 세어서 반환한다. 
        public int GetStoneCount(int x, int y, int nStone, int nDir)
        {
            int dx = 0, dy = 0;
            int tx = x;
            int ty = y;
            int cnt = 0;

            // 착점할 곳이 (int)V1.LINE이 아니면 착점할 수 없으니
            // 0을 return한다. 
            if (nBoard[y, x] != (int)V1.LINE) return 0;
            SetStone(x, y, nStone);

            // 방향 Table에서 dx, dy값을 가져와 
            GetDirTable(ref dx, ref dy, nDir);
            //착점한 곳에서부터 nDir방향으로 같은 돌의 갯수를 센다. 
            for (; nBoard[y, x] == nStone; x += dx, y += dy) cnt++;

            // 방향 Table이 하나로 되어있고, 
            // 순서는 항상 서동 또는 북남으로 되어 있어 
            // nDir이 홀수일 때는 -1한 방향을 검사하여야 한다.
            // 그렇지 않고 다음 방향이라고 무심코 +1한다면
            // 만약 nDir이 동(1)이라면 서(0)쪽을 검사해야 되는데
            // +1해서 2가 되면 서(0)가 아닌 북(2)을 검사하게 된다. 
            // 이를 방지하기 위해 3항 연산자를 이용해
            // 조건에 맞는 방향을 인수로 넘기게 하였다. 
            GetDirTable(ref dx, ref dy, (nDir % 2) > 0 ? nDir - 1 : nDir + 1);

            // x, y값이 위에서 연산을 하느라 처음 좌표를 벗어났기 때문에
            // 처음 좌표값에 다음 방향으로 한칸 옮겨서 그곳에서부터
            // 돌의 갯수를 센다. 
            // 위에서 돌 놓았던 곳을 세었기에 중복을 피하기 위해서다. 
            x = tx + dx;
            y = ty + dy;
            for (; nBoard[y, x] == nStone; x += dx, y += dy) cnt++;

            // 돌의 갯수를 다 세었으면 돌을 들어낸다. 
            SetStone(tx, ty, (int)V1.LINE);
            return cnt;
        }

        // 돌이 연속으로 놓아져 있는지? 
        public bool IsLinked(int x, int y, int nStone, int nDir)
        {
            int dx = 0, dy = 0;
            int tx = x;
            int ty = y;
            int cnt1 = 0;
            int cnt2 = 0;

            if (nBoard[y, x] != (int)V1.LINE)
                return false;

            SetStone(x, y, nStone);
            GetDirTable(ref dx, ref dy, nDir);
            for (; nBoard[y, x] == nStone; x += dx, y += dy) cnt1++;

            x = tx;
            y = ty;
            GetDirTable(ref dx, ref dy, (nDir % 2) > 0 ? nDir - 1 : nDir + 1);
            for (; nBoard[y, x] == nStone; x += dx, y += dy) cnt2++;
            SetStone(tx, ty, (int)V1.LINE);

            return (cnt1 == 1 || cnt2 == 1);
        }

        // 착점을 하였을때 오목인지 검사한다. 
        public bool IsFive(int x, int y, int nStone)
        {
            // 방향 Table의 값이 8이니까 8까지 검사하고
            // 증가분이 +=2인건
            // 아래 GetStoneCount()함수 안에서 양끝을 검사하기 때문이다. 
            for (int i = 0; i < 8; i += 2)
            {
                if (GetStoneCount(x, y, nStone, i) == 5) return true;
            }
            return false;
        }

        // 위와 같이 오목인지를 검사하지만,
        // nDir방향에 대해서만 검사한다.
        // 이는 Four나 OpenFour가 되는지 알아보기 위하여 
        // 빈곳에 돌을 하나씩 놓아보면서
        // 오목이 되는지 검사하기 위해서다. 
        public bool IsFive(int x, int y, int nStone, int nDir)
        {
            if (GetStoneCount(x, y, nStone, nDir) == 5) return true;
            return false;
        }

        public virtual bool IsForbidden(int x, int y, int nStone)
        {
            return false;           
        }

        // 오목을 검사하는 것과 같이 6목 이상인지 검사한다. 
        public bool IsSix(int x, int y, int nStone)
        {
            for (int i = 0; i < 8; i += 2)
            {
                if (GetStoneCount(x, y, nStone, i) >= 6) return true;
            }
            return false;
        }

        public bool IsSix(int x, int y, int nStone, int nDir)
        {
            if (GetStoneCount(x, y, nStone, nDir) >= 6) return true;
            return false;
        }

        // 착수를 했을때 4인지를 검사한다. 
        public bool IsFour(int x, int y, int nStone, int nDir)
        {
            // 원래의 좌표 x, y를 대신하기 위한 변수 
            int tx, ty;

            // 한 방향에 대하여 양끝을 검사하는데
            // 한쌍의 좌표중 첫번째부터 검사하기 위하여 
            if ((nDir % 2) > 0)
                nDir -= 1;
            // 좌표에 착점을 한다 
            SetStone(x, y, nStone);
            // 다음 양끝을 검사 
            for (int i = 0; i < 2; i++)
            {
                // 원래의 좌표를 중심으로 주어진 방향(nDir)에 대하여
                // 양쪽으로 이동하면서 빈곳을 찾아 그곳의 좌표를 얻어와야 되기에
                // 원래의 좌표가 변경이 된다. 따라서 양쪽을 검사하기위하여
                // 대신할 변수가 필요하고, 매번 아래와 같이 
                // 원래의 좌표를 받아서 검사가 진행되어야 한다. 
                tx = x;
                ty = y;
                // 빈곳이 있으면 그 좌표를 받아온다. 
                if (IsEmpty(ref tx, ref ty, nStone, nDir + i))
                {
                    // 그 빈곳에서 오목이 되는지 검사한다. 
                    if (IsFive(tx, ty, nStone, nDir + i) || IsSix(tx, ty, nStone, nDir + i))
                    {
                        // 오목이 되었다면 4이기 때문에 
                        // 돌을 들어내고 참을 리턴한다. 
                        SetStone(x, y, (int)V1.LINE);
                        return true;
                    }
                }
            }

            // 검사를 해봤지만 4가 아니라 해도 돌을 들어낸다. 
            SetStone(x, y, (int)V1.LINE);
            return false;
        }

        // 돌이 연속으로 4개이고 끝에 돌을 놓았을때 오목이 되면 
        // 열린4가 된다. 
        public int IsOpenFour(int x, int y, int nStone, int nDir)
        {
            int tx, ty;
            // 열린4가 될때는 양쪽에 돌을 놓았을때 
            // 양쪽 모두 오목이 되어야 되기때문에
            // 두곳다 4가 되는지 검사를 위한 변수 
            int sum = 0;
            if ((nDir % 2) > 0)
                nDir -= 1;

            SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                // 여기까지는 Four()함수와 같다. 
                if (IsEmpty(ref tx, ref ty, nStone, nDir + i))
                {
                    if (IsFive(tx, ty, nStone, nDir + i) || IsSix(tx, ty, nStone, nDir + i)) sum++;
                }
            }
            SetStone(x, y, (int)V1.LINE);

            // 양쪽으로 모두가 오목이 되었을때만 열린4이다. 
            if (sum == 2)
            {
                // 열린4일 때는 특이하게 한줄에 44가 될수 있다.
                // 1234567   12345678
                // 0 0 0 0   00  0 00
                // 위와 같은 상황에서 4위치에 돌이 놓이면
                // 양쪽이 모두 4가 되어 한줄에서 44가 발생한다.
                // 때문에 0000 이와같이 연속일때는 4가 하나이기 때문에
                // 검사를 하여 갯수를 정확하게 한다. 
                if (GetStoneCount(x, y, nStone, nDir) == 4) sum = 1;
            }
            // sum이 2가 아니면 열린4가 아니므로 0 
            else sum = 0;

            return sum;
        }

        public bool IsDoubleFour(int x, int y, int nStone)
        {
            int cnt = 0;

            //좌표를 중심으로 각각의 줄에대하여 4를 검사한다. 
            for (int i = 0; i < 8; i += 2)
            {
                // OpenFour()에서는 한 줄에 44가 나올 수 있으니 검사하고 
                if (IsOpenFour(x, y, nStone, i) == 2) return true;
                // 열린4도 4이니 4를 검사하여 2개 이상이면 44이다. 
                else if (IsFour(x, y, nStone, i)) cnt++;

                if (cnt >= 2) return true;
            }

            return false;
        }

        public virtual bool IsOpenThree(int x, int y, int nStone, int nDir)
        {
            int tx, ty;

            if ((nDir % 2) > 0)
                nDir -= 1;

            SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                if (IsEmpty(ref tx, ref ty, nStone, nDir + i))
                {
                    // 33은 돌을 하나 더 놓았을 때 반드시 열린4이어야 되고, 
                    // 한 점 놓을 자리가 44, 6목, 또다른 33 
                    // 즉 금수자리가 아니어야 된다.
                    if ((IsOpenFour(tx, ty, nStone, nDir) == 1))
                    {
                        // 위 조건을 만족하는 자리이면 3이므로 돌을 제거하고 참을 리턴한다. 
                        SetStone(x, y, (int)V1.LINE);
                        return true;
                    }
                }
            }

            SetStone(x, y, (int)V1.LINE);
            return false;
        }

        // 33을 검사한다. 
        public bool IsDoubleThree(int x, int y, int nStone)
        {
            int cnt = 0;

            for (int i = 0; i < 8; i += 2)
            {
                if (IsOpenThree(x, y, nStone, i)) cnt++;
                if (cnt >= 2) return true;
            }

            return false;
        }

    }

  public class CNORMALRULE :  COMOKRULE
{
        public override bool IsForbidden(int x, int y, int nStone)
        {
            if (IsDoubleThree(x, y, nStone)) return true;
            else return false;
        }

        public override bool IsOpenThree(int x, int y, int nStone, int nDir)
        {
            int tx, ty;

            if ((nDir % 2) > 0)
                nDir -= 1;

            if (IsFour(x, y, nStone, nDir)) return false;
            SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                if (IsEmpty(ref tx,ref ty, nStone, nDir + i))
                {
                    // 33은 돌을 하나 더 놓았을 때 반드시 열린4이어야 되고, 
                    // 한 점 놓을 자리가 44, 6목, 또다른 33 
                    // 즉 금수자리가 아니어야 된다.
                    if ((IsOpenFour(tx, ty, nStone, nDir) == 1) && (!IsDoubleThree(tx, ty, nStone)))
                    {
                        // 위 조건을 만족하는 자리이면 3이므로 돌을 제거하고 참을 리턴한다. 
                        SetStone(x, y, (int)V1.LINE);
                        return true;
                    }
                }
            }

            SetStone(x, y, (int)V1.LINE);
            return false;
        }

    };

    public class CRENJURULE : COMOKRULE
{
        // 금수자리를 검사한다. 
        public override bool IsForbidden(int x, int y, int nStone)
        {
            if (IsDoubleFour(x, y, nStone)) return true;
            else if (IsDoubleThree(x, y, nStone)) return true;
            else if (IsSix(x, y, nStone)) return true;
            else return false;
        }

        public bool IsFour(int x, int y, int nStone, int nDir)
        {
            // 원래의 좌표 x, y를 대신하기 위한 변수 
            int tx, ty;

            // 한 방향에 대하여 양끝을 검사하는데
            // 한쌍의 좌표중 첫번째부터 검사하기 위하여 
            if ((nDir % 2) > 0)
                nDir -= 1;
            // 좌표에 착점을 한다 
            SetStone(x, y, nStone);
            // 다음 양끝을 검사 
            for (int i = 0; i < 2; i++)
            {
                // 원래의 좌표를 중심으로 주어진 방향(nDir)에 대하여
                // 양쪽으로 이동하면서 빈곳을 찾아 그곳의 좌표를 얻어와야 되기에
                // 원래의 좌표가 변경이 된다. 따라서 양쪽을 검사하기위하여
                // 대신할 변수가 필요하고, 매번 아래와 같이 
                // 원래의 좌표를 받아서 검사가 진행되어야 한다. 
                tx = x;
                ty = y;
                // 빈곳이 있으면 그 좌표를 받아온다. 
                if (IsEmpty(ref tx,ref ty, nStone, nDir + i))
                {
                    // 그 빈곳에서 오목이 되는지 검사한다. 
                    if (IsFive(tx, ty, nStone, nDir + i))
                    {
                        // 오목이 되었다면 4이기 때문에 
                        // 돌을 들어내고 참을 리턴한다. 
                        SetStone(x, y, (int)V1.LINE);
                        return true;
                    }
                }
            }

            // 검사를 해봤지만 4가 아니라 해도 돌을 들어낸다. 
            SetStone(x, y, (int)V1.LINE);
            return false;
        }

        // 돌이 연속으로 4개이고 끝에 돌을 놓았을때 오목이 되면 
        // 열린4가 된다. 
        public int IsOpenFour(int x, int y, int nStone, int nDir)
        {
            int tx, ty;
            // 열린4가 될때는 양쪽에 돌을 놓았을때 
            // 양쪽 모두 오목이 되어야 되기때문에
            // 두곳다 4가 되는지 검사를 위한 변수 
            int sum = 0;

            if ((nDir % 2) > 0)
                nDir -= 1;

            SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                // 여기까지는 Four()함수와 같다. 
                if (IsEmpty(ref tx,ref ty, nStone, nDir + i))
                {
                    if (IsFive(tx, ty, nStone, nDir + i)) sum++;
                }
            }
            SetStone(x, y, (int)V1.LINE);

            // 양쪽으로 모두가 오목이 되었을때만 열린4이다. 
            if (sum == 2)
            {
                // 열린4일 때는 특이하게 한줄에 44가 될수 있다.
                // 1234567   12345678
                // 0 0 0 0   00  0 00
                // 위와 같은 상황에서 4위치에 돌이 놓이면
                // 양쪽이 모두 4가 되어 한줄에서 44가 발생한다.
                // 때문에 0000 이와같이 연속일때는 4가 하나이기 때문에
                // 검사를 하여 갯수를 정확하게 한다. 
                if (GetStoneCount(x, y, nStone, nDir) == 4) sum = 1;
            }
            // sum이 2가 아니면 열린4가 아니므로 0 
            else sum = 0;

            return sum;
        }

        public override bool IsOpenThree(int x, int y, int nStone, int nDir)
        {
            int tx, ty;

            if ((nDir % 2) > 0)
                nDir -= 1;

            SetStone(x, y, nStone);
            for (int i = 0; i < 2; i++)
            {
                tx = x;
                ty = y;
                if (IsEmpty(ref tx,ref ty, nStone, nDir + i))
                {
                    // 33은 돌을 하나 더 놓았을 때 반드시 열린4이어야 되고, 
                    // 한 점 놓을 자리가 44, 6목, 또다른 33 
                    // 즉 금수자리가 아니어야 된다.
                    if ((IsOpenFour(tx, ty, nStone, nDir) == 1) && (!IsForbidden(tx, ty, nStone)))
                    {
                        // 위 조건을 만족하는 자리이면 3이므로 돌을 제거하고 참을 리턴한다. 
                        SetStone(x, y, (int)V1.LINE);
                        return true;
                    }
                }
            }

            SetStone(x, y, (int)V1.LINE);
            return false;
        }
    };
}
