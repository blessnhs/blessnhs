using OMOK_T.Views;
using System;
using System.Reflection;
using Xamarin.Forms;

namespace OMOK_T
{
    class Board : AbsoluteLayout
    {
        // Alternative sizes make the tiles a tad small.
        const int COLS = 10;         // 16
        const int ROWS = 10;         // 16

        Tile[,] tiles = new Tile[ROWS, COLS];
        int flaggedTileCount;
        bool isGameInProgress;              // on first tap
        bool isGameInitialized;             // on first double-tap
        bool isGameEnded;

        // Events to notify page.
        public event EventHandler GameStarted;
        public event EventHandler<bool> GameEnded;

        public Board()
        {
            for (int row = 0; row < ROWS; row++)
                for (int col = 0; col < COLS; col++)
                {
                    Tile tile = new Tile(row, col);
                    tile.TileStatusChanged += OnTileStatusChanged;
                    this.Children.Add(tile);
                    tiles[row, col] = tile;
                    tile.x = col;
                    tile.y = row;
                }

            SizeChanged += (sender, args) =>
                {
                    double tileWidth = this.Width / COLS;
                    double tileHeight = this.Height / ROWS;

                    foreach (Tile tile in tiles)
                    {
                        Rectangle bounds = new Rectangle(tile.Col * tileWidth,
                                                         tile.Row * tileHeight,
                                                         tileWidth, tileHeight);
                        AbsoluteLayout.SetLayoutBounds(tile, bounds);
                    }
                };

            var image = new Image();

            image.Source = ImageSource.FromResource("OMOK_T.Image.Background.png",
                                            typeof(MainPage).GetTypeInfo().Assembly);

            image.VerticalOptions = LayoutOptions.StartAndExpand;

       //     this.Children.Add(image);


           NewGameInitialize();
        }

        public void UpdateStone(int x,int y,TileStatus tileStatus)
        {
            var title = tiles[y, x];
            if(title != null)
            {
                tiles[y, x].Status = tileStatus;
            }
        }

        public void NewGameInitialize()
        {
            // Clear all the tiles.
            foreach (Tile tile in tiles)
                tile.Initialize();

            isGameInProgress = false;
            isGameInitialized = false;
            isGameEnded = false;
            this.FlaggedTileCount = 0;
        }

        public int FlaggedTileCount
        {
            set
            {
                if (flaggedTileCount != value)
                {
                    flaggedTileCount = value;
                    OnPropertyChanged();
                }
            }
            get
            {
                return flaggedTileCount;
            }
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

        void OnTileStatusChanged(object sender, TileStatus tileStatus)
        {
            if (isGameEnded)
                return;

            // With a first tile tapped, the game is now in progress.
            if (!isGameInProgress)
            {
                isGameInProgress = true;

                // Fire the GameStarted event.
                if (GameStarted != null)
                {
                 //   GameStarted(this, EventArgs.Empty);
                }
            }

            //// Update the "flagged" bug count before checking for a loss.
            //int flaggedCount = 0;

            //foreach (Tile tile in tiles)
            //    if (tile.Status == TileStatus.Flagged)
            //        flaggedCount++;

            //this.FlaggedTileCount = flaggedCount;

            //// Get the tile whose status has changed.
            //Tile changedTile = (Tile)sender;

            //// If it's exposed, some actions are required.
            //if (tileStatus == TileStatus.Exposed)
            //{
            //    if (!isGameInitialized)
            //    {
            //        DefineNewBoard(changedTile.Row, changedTile.Col);
            //        isGameInitialized = true;
            //    }

            //    if (changedTile.IsBug)
            //    {
            //        isGameInProgress = false;
            //        isGameEnded = true;

            //        // Fire the GameEnded event!
            //        if (GameEnded != null)
            //        {
            //            GameEnded(this, false);
            //        }
            //        return;
            //    }

            //    // Auto expose for zero surrounding bugs.
            //    if (changedTile.SurroundingBugCount == 0)
            //    {
            //        CycleThroughNeighbors(changedTile.Row, changedTile.Col,
            //            (neighborRow, neighborCol) =>
            //            {
            //                // Expose all the neighbors.
            //                tiles[neighborRow, neighborCol].Status = TileStatus.Exposed;
            //            });
            //    }
            //}

            //// Check for a win.
            //bool hasWon = true;

            //foreach (Tile til in tiles)
            //{
            //    if (til.IsBug && til.Status != TileStatus.Flagged)
            //        hasWon = false;

            //    if (!til.IsBug && til.Status != TileStatus.Exposed)
            //        hasWon = false;
            //}

            //// If there's a win, celebrate!
            //if (hasWon)
            //{
            //    isGameInProgress = false;
            //    isGameEnded = true;

            //    // Fire the GameEnded event!
            //    if (GameEnded != null)
            //    {
            //        GameEnded(this, true);
            //    }
            //    return;
            //}
        }


        public bool CheckPointer(int _y, int _x, TileStatus _stone)   // 현재 놓은 돌의 좌표와 색상(1or2) 정보를 받음
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
