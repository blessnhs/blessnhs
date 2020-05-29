#define FIX_UWP_DOUBLE_TAPS   // Double-taps don't work well on UWP as of 2.3.0
#define FIX_UWP_NULL_CONTENT  // Set Content of Frame to null doesn't work in UWP as of 2.3.0

using System;
using Xamarin.Forms;
using OMOK.Network;

namespace OMOK
{
    static class TitleResource
    {
        static ImageSource WhiteStoneImageSource = null;
        static ImageSource BlackStoneImageSource = null;
        static ImageSource EmptyStoneImageSource = null;
        static ImageSource AimStoneImageSource = null;

        public static ImageSource GetAimStoneImageSource()
        {
            if (AimStoneImageSource == null)
                AimStoneImageSource = ImageSource.FromResource("OMOK.Image.Aim.png");
            return AimStoneImageSource;
        }

        public static ImageSource GetWhiteStoneImageSource()
        {
            if(WhiteStoneImageSource == null)
                WhiteStoneImageSource = ImageSource.FromResource("OMOK.Image.White.png");
            return WhiteStoneImageSource;
        }

        public static ImageSource GetBlackStoneImageSource()
        {
            if (BlackStoneImageSource == null)
                BlackStoneImageSource = ImageSource.FromResource("OMOK.Image.Black.png");
            return BlackStoneImageSource;
        }
        public static ImageSource GetEmptyStoneImageSource()
        {
            if (EmptyStoneImageSource == null)
                EmptyStoneImageSource = ImageSource.FromResource("OMOK.Image.Cell.png");
            return EmptyStoneImageSource;
        }
    }

    class Tile : Frame
    {
        eTeam tileStatus = eTeam.None;
        Label label;
        Image whiteImage, blackImage,emptyImage,aimImage;
        bool doNotFireEvent;

        public event EventHandler<eTeam> TileStatusChanged;

        public int x, y;

        static Tile()
        {
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
                Source = TitleResource.GetWhiteStoneImageSource(),

            };

            blackImage = new Image {
                Source = TitleResource.GetBlackStoneImageSource()
            };


            emptyImage = new Image
            {
                Source = TitleResource.GetEmptyStoneImageSource()
            };

            aimImage = new Image
            {
                Source = TitleResource.GetAimStoneImageSource()
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

        public eTeam Status {
            set {
         //       if (tileStatus != value)
                {
                    tileStatus = value;

                    switch (tileStatus)
                    {
                        case eTeam.None:
                            this.Content = emptyImage;
                            break;

                        case eTeam.White:
                            this.Content = whiteImage;
                            break;

                        case eTeam.Black:
                            this.Content = blackImage;
                            break;
                        case eTeam.Aim:
                            this.Content = aimImage;
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
            this.Status = eTeam.None;

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
            if(User.IsMyTurn == false || this.Status != eTeam.None)
            {
                return;
            }

            switch (User.Color) {
            case eTeam.None:
                this.Status = eTeam.None;
                break;

            case eTeam.Black:
                this.Status = eTeam.Black;
                break;

            case eTeam.White:
                this.Status = eTeam.White;
                break;
            }

            string Message = x + ":" + y + ":" + User.Color.ToString();

            NetProcess.SendRoomMessage(x,y, User.Color,Message);

            User.IsMyTurn = false;
        }

        void OnDoubleTap (object sender, object args)
        {
          //  this.Status = TileStatus.Exposed;
        }
    }
}
