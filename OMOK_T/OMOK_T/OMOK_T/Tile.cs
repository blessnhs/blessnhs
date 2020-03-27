#define FIX_UWP_DOUBLE_TAPS   // Double-taps don't work well on UWP as of 2.3.0
#define FIX_UWP_NULL_CONTENT  // Set Content of Frame to null doesn't work in UWP as of 2.3.0

using System;
using Xamarin.Forms;
using OMOK_T.Network;

namespace OMOK_T
{
    public enum TileStatus
    {
        Empty,
        Black,
        White
    }

    class Tile : Frame
    {
        TileStatus tileStatus = TileStatus.Empty;
        Label label;
        Image whiteImage, blackImage,emptyImage;
        static ImageSource WhiteStoneImageSource;
        static ImageSource BlackStoneImageSource;
        static ImageSource EmptyStoneImageSource;
        bool doNotFireEvent;

        public event EventHandler<TileStatus> TileStatusChanged;

        public int x, y;

        static Tile()
        {
            WhiteStoneImageSource = ImageSource.FromResource("OMOK_T.Image.White.png");
            BlackStoneImageSource = ImageSource.FromResource("OMOK_T.Image.Black.png");
            EmptyStoneImageSource = ImageSource.FromResource("OMOK_T.Image.Cell.png");
        }

        public Tile(int row, int col)
        {
            this.Row = row;
            this.Col = col;

            //this.BackgroundColor = Color.Yellow;
            this.OutlineColor = Color.Black;
            this.Margin = 1;
            this.Padding = 1;
            this.CornerRadius = 0;

            label = new Label {
                Text = " ",
                TextColor = Color.Yellow,
                BackgroundColor = Color.White,
                HorizontalTextAlignment = TextAlignment.Center,
                VerticalTextAlignment = TextAlignment.Center,
            };

            whiteImage = new Image {
                Source = WhiteStoneImageSource,

            };

            blackImage = new Image {
                Source = BlackStoneImageSource
            };


            emptyImage = new Image
            {
                Source = EmptyStoneImageSource
            };

            TapGestureRecognizer singleTap = new TapGestureRecognizer {
                NumberOfTapsRequired = 1
            };
            singleTap.Tapped += OnSingleTap;
            this.GestureRecognizers.Add(singleTap);

            this.Content = emptyImage;

#if FIX_UWP_DOUBLE_TAPS

            if (Device.RuntimePlatform != Device.UWP) {

#endif

                TapGestureRecognizer doubleTap = new TapGestureRecognizer {
                    NumberOfTapsRequired = 2
                };
                doubleTap.Tapped += OnDoubleTap;
                this.GestureRecognizers.Add(doubleTap);

#if FIX_UWP_DOUBLE_TAPS

            }

#endif

        }

        public int Row { private set; get; }

        public int Col { private set; get; }

        public TileStatus Status {
            set {
         //       if (tileStatus != value)
                {
                    tileStatus = value;

                    switch (tileStatus)
                    {
                        case TileStatus.Empty:
                            this.Content = emptyImage;
                            break;

                        case TileStatus.White:
                            this.Content = whiteImage;
                            break;

                        case TileStatus.Black:
                            this.Content = blackImage;
                            break;
                        default:

                            break;
                    }
                }
            }
            get {
                return tileStatus;
            }
        }

        // Does not fire TileStatusChanged events.
        public void Initialize()
        {
            this.Status = TileStatus.Empty;

            this.Content = emptyImage;
        }

#if FIX_UWP_DOUBLE_TAPS

        bool lastTapSingle;
        DateTime lastTapTime;

#endif

        void OnSingleTap(object sender, object args)
        {

#if FIX_UWP_DOUBLE_TAPS

            if (Device.RuntimePlatform == Device.UWP) {
                if (lastTapSingle && DateTime.Now - lastTapTime < TimeSpan.FromMilliseconds (500)) {
                    OnDoubleTap (sender, args);
                    lastTapSingle = false;
                } else {
                    lastTapTime = DateTime.Now;
                    lastTapSingle = true;
                }
        	}

#endif
            if(User.IsMyTurn == false)
            {
                return;
            }

            switch (User.Color) {
            case TileStatus.Empty:
                this.Status = TileStatus.Empty;
                break;

            case TileStatus.Black:
                this.Status = TileStatus.Black;
                break;

            case TileStatus.White:
                this.Status = TileStatus.White;
                break;
            }

            string Message = x + ":" + y + ":" + User.Color.ToString();

            NetProcess.SendRoomMessage(Message);

            User.IsMyTurn = false;
        }

        void OnDoubleTap (object sender, object args)
        {
          //  this.Status = TileStatus.Exposed;
        }
    }
}
