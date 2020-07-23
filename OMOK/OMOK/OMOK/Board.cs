using OMOK.Views;
using System;
using System.Reflection;
using Xamarin.Forms;

namespace OMOK
{
    class Board : AbsoluteLayout
    {
        // Alternative sizes make the tiles a tad small.
        const int COLS = 14;         // 16
        const int ROWS = 14;         // 16

        public int x = COLS / 2, y = ROWS / 2;

        Tile[,] tiles = new Tile[ROWS, COLS];

        bool onedraw = false;

        public bool CheckValid(int _x,int _y)
        {
            if (_x < 0 || _y < 0 || COLS <= _x || ROWS <= _y)
                return false;

            return true;
        }

        public Board()
        {

            for (int row = 0; row < ROWS; row++)
                for (int col = 0; col < COLS; col++)
                {
                    Tile tile = new Tile(row, col);
                    this.Children.Add(tile);
                    tiles[row, col] = tile;
                    tile.x = col;
                    tile.y = row;
                    tile.parent = this;
                }

                SizeChanged += (sender, args) =>
                {
                    UpdateAim();

                    if (onedraw != false)
                        return;

                    onedraw = true;

                    double tileWidth =  this.Width / COLS;
                    double tileHeight = tileWidth;// this.Height / ROWS;

                    foreach (Tile tile in tiles)
                    {
                        Rectangle bounds = new Rectangle(tile.Col * tileWidth,
                                                         tile.Row * tileHeight,
                                                         tileWidth, tileHeight);
                        AbsoluteLayout.SetLayoutBounds(tile, bounds);

                    }
                };
         
           NewGameInitialize();
        }

        public void ClearBoardState()
        {
            x = COLS / 2;
            y = ROWS / 2;

            for (int row = 0; row < ROWS; row++)
            {
                for (int col = 0; col < COLS; col++)
                {
                    tiles[row, col].Status = eTeam.None;
                }
            }

            prevsTATE = eTeam.None;
        }

        public eTeam prevsTATE = eTeam.None;

        public void UpdateAim(int addx=0,int addy=0)
        {
            if (x + addx < 0)
            {
                return;
            }

            if (y + addy < 0)
            {
                return;
            }

            if (x + addx >= COLS)
            {
                return;
            }

            if (y + addy >= ROWS)
            {
                return;
            }

            int nx = x + addx;
            int ny = y + addy;
    
            tiles[y, x].Status = prevsTATE;

            if (tiles[ny, nx].Status == eTeam.Black)
            {
                tiles[ny, nx].Status = eTeam.Ablack;
                prevsTATE = eTeam.Black;
            }
            else if (tiles[ny, nx].Status == eTeam.White)
            {
                tiles[ny, nx].Status = eTeam.Awhite;
                prevsTATE = eTeam.White;
            }
            else
            {
               tiles[ny, nx].Status = eTeam.Aim;
                prevsTATE = eTeam.None;
            }


            x = nx;
            y = ny;
        }

        public void UpdateAimSet(int addx = 0, int addy = 0)
        {
            if (addx < 0)
            {
                return;
            }

            if (addy < 0)
            {
                return;
            }

            if (addx >= COLS)
            {
                return;
            }

            if (addy >= ROWS)
            {
                return;
            }

            int nx = addx;
            int ny = addy;

            tiles[y, x].Status = prevsTATE;

            if (tiles[ny, nx].Status == eTeam.Black)
            {
                tiles[ny, nx].Status = eTeam.Ablack;
                prevsTATE = eTeam.Black;
            }
            else if (tiles[ny, nx].Status == eTeam.White)
            {
                tiles[ny, nx].Status = eTeam.Awhite;
                prevsTATE = eTeam.White;
            }
            else
            {
                tiles[ny, nx].Status = eTeam.Aim;
                prevsTATE = eTeam.None;
            }


            x = nx;
            y = ny;
        }

        public Tile GetTile(int x,int y)
        {
           return tiles[y, x];
        }


        public void UpdateStone(int cx,int cy,eTeam tileStatus)
        {
            var title = tiles[cy, cx];
            if(title != null)
            {
                if(cy == y && cx == x)
                {
                    if (tileStatus == eTeam.Black)
                    {
                        tiles[cy, cx].Status = eTeam.Ablack;
                        prevsTATE = eTeam.Black;
                    }
                    else if (tileStatus == eTeam.White)
                    {
                        tiles[cy, cx].Status = eTeam.Awhite;
                        prevsTATE = eTeam.White;
                    }
                    else
                        tiles[cy, cx].Status = tileStatus;
                }
                else
                    tiles[cy, cx].Status = tileStatus;
            }
        }

        public void NewGameInitialize()
        {
            // Clear all the tiles.
            foreach (Tile tile in tiles)
                tile.Initialize();

        }


        
        // Not called until the first tile is double-tapped.
        void DefineNewBoard(int tappedRow, int tappedCol)
        {
            // Begin the assignment of bugs.
            Random random = new Random();
            //int bugCount = 0;

            //while (bugCount < BUGS)
            //{
            //    // Get random row and column.
            //    int row = random.Next(ROWS);
            //    int col = random.Next(COLS);

            //    // Skip it if it's already a bug.
            //    if (tiles[row, col].IsBug)
            //    {
            //        continue;
            //    }

            //    // Avoid the tappedRow & Col & surrounding ones.
            //    if (row >= tappedRow - 1 &&
            //        row <= tappedRow + 1 &&
            //        col >= tappedCol - 1 &&
            //        col <= tappedCol + 1)
            //    {
            //        continue;
            //    }

            //    // It's a bug!
            //    tiles[row, col].IsBug = true;

            //    // Calculate the surrounding bug count.
            //    CycleThroughNeighbors(row, col,
            //        (neighborRow, neighborCol) =>
            //        {
            //            ++tiles[neighborRow, neighborCol].SurroundingBugCount;
            //        });

            //    bugCount++;
            //}
        }

        void CycleThroughNeighbors(int row, int col, Action<int, int> callback)
        {
            int minRow = Math.Max(0, row - 1);
            int maxRow = Math.Min(ROWS - 1, row + 1);
            int minCol = Math.Max(0, col - 1);
            int maxCol = Math.Min(COLS - 1, col + 1);

            for (int neighborRow = minRow; neighborRow <= maxRow; neighborRow++)
                for (int neighborCol = minCol; neighborCol <= maxCol; neighborCol++)
                {
                    if (neighborRow != row || neighborCol != col)
                        callback(neighborRow, neighborCol);
                }
        }

        public bool CheckPointer(int _y, int _x, eTeam _stone)   // 현재 놓은 돌의 좌표와 색상(1or2) 정보를 받음
        {
            int x, y;
            int count;
         
            //가로
            x = _x;
            y = _y;
            count = 0;
            // x값만 감소시켜 stone과 다를때까지 감소
            while ( x > 0 && tiles[_y,x - 1].Status == _stone)
                x--;

            // x값만 증가시켜 stone과 다를때까지 증가
            while ( x < COLS && tiles[_y,x++].Status == _stone)
                count++;
         
            if(count == 5)  // x값을 5번 증가시켰을 경우 GameWin함수 호출.
                return true;

            //세로
            x = _x;
            y = _y;
            count = 0;
            // 이번에는 y값만 변화시켜 가로와 마찬가지로 오목을 체크한다.
            while ( y > 0 && tiles[y - 1,_x].Status == _stone)
                y--;
            while ( y < ROWS && tiles[y++,_x].Status == _stone)
                count++;
         
            if(count == 5)
                return true;

            //대각선↘
            x = _x;
            y = _y;
            count = 0;
            // x, y 모두 변화시켜 위와 같은 방법으로 오목을 체크.
            while ( x > 0 && y > 0 && tiles[y - 1,x - 1].Status == _stone)
            {
                y--;
                x--;
            }
         
            while ( x < COLS && y < ROWS && tiles[y++,x++].Status == _stone)
                count++;
         
            if(count == 5)
                return true;

            //대각선↙
            x = _x;
            y = _y;
            count = 0;
            // x, y 모두 변화시켜 위와 같은 방법으로 오목을 체크.
            while ( (x+1) < COLS && y > 0 && tiles[y - 1,x + 1].Status == _stone)
            {
                x++;
                y--;
            }
            while ( x >= 0 && y < ROWS && tiles[y++,x--].Status == _stone)
                count++;

            if(count == 5)
                return true;


            return false;
        }
    }
}
