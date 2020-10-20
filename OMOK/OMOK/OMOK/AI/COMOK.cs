using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public struct UndoInfo
    {
        public int x, y;
        public int nStone;
    };

    public class COMOK
    {
        public static int[,] arrBoard = new int[ConstValue.SIZE + 2, ConstValue.SIZE + 2];
        public static int x, y;

        // Undo와 Redo를 위한 top pointer 
        public static int uTop, rTop;
        // last 착수점 
        public static int lx, ly;

        public static  UndoInfo[] uInfo = new UndoInfo[ConstValue.SIZE * ConstValue.SIZE];
        public static UndoInfo[] rInfo = new UndoInfo[ConstValue.SIZE * ConstValue.SIZE];

        public COMOKRULE orule = new COMOKRULE();
        public COMOKRULE pRule = new COMOKRULE();
        public COMOK()
        {
            x = y = (int)ConstValue.CENTER;
            undoSet((int)V1.BLACK_STONE);
        }

        //바둑이 시작될 때 초기화 
        public void initOmok(int nStone)
        {
            initBoard(nStone);
            // 1초에 한번씩 그리는데 시차를 두기 위해서
            // 결과적으로 500msec마다 커서처럼 깜박거리게 된다. 
            uTop = rTop = -1;
        }

        //커서를 이동하기전 원래 바둑판을 복원한다. 
        public void setBoard(int x, int y)
        {
            
        }


        public int undo()
        {
            if (uTop < 0) return 0;

            rInfo[++rTop].x = lx = uInfo[uTop].x;
            rInfo[rTop].y = ly = uInfo[uTop].y;
            rInfo[rTop].nStone = uInfo[uTop--].nStone;

            arrBoard[ly, lx] = (int)V1.LINE;

            return (int)V2.CHANGE;
        }

        public int redo()
        {
            if (rTop < 0) return 0;

            uInfo[++uTop].x = lx = rInfo[rTop].x;
            uInfo[uTop].y = ly = rInfo[rTop].y;
            uInfo[uTop].nStone = rInfo[rTop--].nStone;

            arrBoard[ly, lx] = uInfo[uTop].nStone;

            return (int)V2.CHANGE;
        }

        public void undoSet(int nStone)
        {
            uInfo[++uTop].x = x;
            uInfo[uTop].y = y;
            uInfo[uTop].nStone = nStone;
            rTop = -1;
        }


        // 돌이 놓이면 배열에 흑백돌을 저장한다.
        // 처음 흑돌을 중앙에 한수 놓고 시작한다. 
        public void initBoard(int nStone)
        {
            for (int i = 0; i < ConstValue.SIZE + 2; i++)
                for (int j = 0; j < ConstValue.SIZE + 2; j++)
                    arrBoard[i, j] = (int)V1.LINE;
       
            arrBoard[(int)ConstValue.CENTER, (int)ConstValue.CENTER] = nStone;

        }

        public void initBoard()
        {
            uTop = rTop = -1;

            for (int i = 0; i < ConstValue.SIZE + 2; i++)
                for (int j = 0; j < ConstValue.SIZE + 2; j++)
                    arrBoard[i, j] = (int)V1.LINE;
        }


        // 바둑알 놓을 자리를 체크한다. 
        public bool isOccupy()
        {
            return arrBoard[y, x] != (int)V1.LINE;
        }

        // 바둑알이 착점 되면 보드에 바둑알을 그리고
        // 보드 배열에 저장한다.  
        public void saveBoard(int stone)
        {
            arrBoard[y, x] = stone;
            undoSet(stone);
        }

        // Game class에서 받아온 좌표를 저장한다. 
        public void setXY(int ax, int ay)
        {
            x = ax;
            y = ay;
        }

        public void getXY(ref int ax, ref int ay)
        {
            ax = x;
            ay = y;
        }

        // 착점을 위한함수
        // 착점이 가능한지 체크하고
        // 착점이 되면 저장을 한다.
        // 다음으로 오목인지 검사하여 결과를 리턴한다. 
        virtual public int placement(int ax, int ay, int nStone)
        {
            setXY(ax, ay);
            if (isOccupy()) return (int)V2.OCCUPIED;

            int returnValue = (int)V2.CHANGE;
            orule.SetBoard(arrBoard);
            if (orule.IsFive(x, y, nStone) || orule.IsSix(x, y, nStone)) returnValue = (int)V2.FIVEMOK;
            saveBoard(nStone);

            return returnValue;
        }
    };

    public class CBLACKSTONE : COMOK
    {

        CRENJURULE rRule = new CRENJURULE();

        public CBLACKSTONE()
        {
            pRule = rRule;
        }

        public override int placement(int ax, int ay, int nStone)
        {

            setXY(ax, ay);
            if (isOccupy()) return (int)V2.OCCUPIED;

            pRule.SetBoard(arrBoard);
            if (rRule.IsFive(x, y, nStone))
            {
                saveBoard(nStone);
                return (int)V2.FIVEMOK;
            }
            else if (pRule.IsSix(x, y, nStone)) return (int)V2.SIXMOK;
            else if (pRule.IsDoubleFour(x, y, nStone)) return (int)V2.SASA;
            else if (pRule.IsDoubleThree(x, y, nStone)) return (int)V2.SAMSAM;

            saveBoard(nStone);
            return (int)V2.CHANGE;
        }
    };

    public class CNORMAL : COMOK
    {

        CNORMALRULE nRule = new CNORMALRULE();

        public CNORMAL()
        {
            pRule = nRule;
        }

        public override int placement(int ax, int ay, int nStone)
        {
            setXY(ax, ay);
            if (isOccupy()) return (int)V2.OCCUPIED;

            pRule.SetBoard(arrBoard);
            if (pRule.IsFive(x, y, nStone) || pRule.IsSix(x, y, nStone))
            {
                saveBoard(nStone);
                return (int)V2.FIVEMOK;
            }
            else if (pRule.IsDoubleThree(x, y, nStone)) return (int)V2.SAMSAM;

            saveBoard(nStone);
            return (int)V2.CHANGE;
        }
    };

    public class CGOMOKUAI : COMOK
    {

        COMOKAI ai = new COMOKAI();

        public override int placement(int ax, int ay, int nStone)
        {
            int ret = (int)V2.CHANGE;

            ai.SetBoard(arrBoard, orule);
            ai.play(ref x,ref y, nStone);
            if (orule.IsFive(x, y, nStone) || orule.IsSix(x, y, nStone)) ret = (int)V2.FIVEMOK;

            setBoard(ax, ay);
            saveBoard(nStone);
            //pDraw().printData(x, y, CURSOR);
            //cursorView(x, y);

            Console.WriteLine("stone " + nStone.ToString() + " x " + x.ToString() + " y " + y.ToString());

            return ret;
        }
    };

    public class CJENERALAI : COMOK
    {

        CNORMALRULE nRule = new CNORMALRULE();
        CNORMALAI ai = new CNORMALAI();
        public override int placement(int ax, int ay, int nStone)
        {
            int ret = (int)V2.CHANGE;

            ai.SetBoard(arrBoard, nRule);
            ai.play(ref x, ref y, nStone);
            if (nRule.IsFive(x, y, nStone) || nRule.IsSix(x, y, nStone)) ret = (int)V2.FIVEMOK;

            setBoard(ax, ay);
            saveBoard(nStone);

            return ret;
        }
    };

    public class CRENJUBLACKAI : COMOK
    {

        CRENJUBLACK ai = new CRENJUBLACK();
        CRENJURULE rRule = new CRENJURULE();

        public override int placement(int ax, int ay, int nStone)
        {
            int ret = (int)V2.CHANGE;

            ai.SetBoard(arrBoard, rRule);
            ai.SetRenjuObject(orule);
            ai.play(ref x, ref y, nStone);
            if (rRule.IsFive(x, y, nStone)) ret = (int)V2.FIVEMOK;

            setBoard(ax, ay);
            saveBoard(nStone);
            //pDraw().printData(x, y, CURSOR);
            //cursorView(x, y);

            Console.WriteLine("stone " + nStone.ToString() + " x " + x.ToString() + " y " + y.ToString());

            return ret;
        }
    };

    public class CRENJUWHITEAI : COMOK
    {

        CRENJUWHITE ai = new CRENJUWHITE();
        CRENJURULE rRule = new CRENJURULE();

        public override int placement(int ax, int ay, int nStone)
        {
            int ret = (int)V2.CHANGE;

            ai.SetBoard(arrBoard, orule);
            ai.SetRenjuObject(rRule);
            ai.play(ref x, ref y, nStone);
            if (orule.IsFive(x, y, nStone) || orule.IsSix(x, y, nStone)) ret = (int)V2.FIVEMOK;

            setBoard(ax, ay);
            saveBoard(nStone);
            //pDraw().printData(x, y, CURSOR);
            //cursorView(x, y);

            Console.WriteLine("stone " + nStone.ToString() + " x " + x.ToString() + " y " + y.ToString());

            return ret;
        }

    };





}
