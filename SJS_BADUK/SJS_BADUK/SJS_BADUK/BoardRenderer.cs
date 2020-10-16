using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace SJS_BADUK
{
    public class BoardRenderer
    {
        double screenx;
        double screeny;

        //square
        double boxwidth;
        double boxheight;

        double size = 19;  //default 19

        public AbsoluteLayout absoluteLayout;
        public BadukControl contorlLogic;

        public void Init(double _size, double _Width, ref AbsoluteLayout _absoluteLayout, BadukControl _control)
        {
            size = _size;
            absoluteLayout = _absoluteLayout;
            contorlLogic = _control;

            screeny = screenx = _Width;

            boxwidth = screenx / size;
            boxheight = screeny / size;

            var background = new Image();
            background.Source = ImageSource.FromResource("SJS_BADUK.Image.Board_1.png");

            background.Aspect = Aspect.AspectFill;
            background.VerticalOptions = LayoutOptions.FillAndExpand;
            background.HorizontalOptions = LayoutOptions.FillAndExpand;

            absoluteLayout.Children.Add(background, new Rectangle(0, 0, 1, 1), AbsoluteLayoutFlags.All);

            DrawLine();

            DrawFlowerPointStone(3, 3);
            DrawFlowerPointStone(9, 3);
            DrawFlowerPointStone(15, 3);

            DrawFlowerPointStone(3, 9);
            DrawFlowerPointStone(9, 9);
            DrawFlowerPointStone(15, 9);

            DrawFlowerPointStone(3, 15);
            DrawFlowerPointStone(9, 15);
            DrawFlowerPointStone(15, 15);

            TouchEffect touchEffect = new TouchEffect();
            touchEffect.TouchAction += OnTouchEffectAction;
            absoluteLayout.Effects.Add(touchEffect);
               
        }

        Dictionary<int, Dictionary<int, View>> board = new Dictionary<int, Dictionary<int, View>>();

        void SetViewDictionary(int x, int y, View v)
        {
            if (board.ContainsKey(x) == false)
                board[x] = new Dictionary<int, View>();

            board[x][y] = v;
        }

        View GetViewDictionary(int x, int y)
        {
            if (board.ContainsKey(x) == false)
                return null;

            return board[x][y];
        }

        public void RemoveStone(int x, int y)
        {
            var view = GetViewDictionary(x, y);

            if (view != null)
                absoluteLayout.Children.Remove(view);
        }

        public void DrawStone(int x, int y, Color color)
        {
            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 2;
            double yy = beginy + (boxwidth / 2) - boxwidth / 2;

            double xx_width = (boxwidth / 2) * 2;
            double yy_height = ((boxwidth / 2) * 2);


            if (color == Color.Black)
            {
                var Image = new ImageButton
                {
                    Source = ImageSource.FromResource("SJS_BADUK.Image.Black.png"),
                    CornerRadius = (int)(xx_width / 2)
                };

                absoluteLayout.Children.Add(Image, new Rectangle(xx, yy, xx_width, yy_height));

                SetViewDictionary(x, y, Image);
            }
            else
            {
                var Image = new ImageButton
                {
                    Source = ImageSource.FromResource("SJS_BADUK.Image.White.png"),
                    CornerRadius = (int)(xx_width / 2)
                };

                absoluteLayout.Children.Add(Image, new Rectangle(xx, yy, xx_width, yy_height));

                SetViewDictionary(x, y, Image);
            }

            UpdateAim(x, y);
        }

        public void DrawFlowerPointStone(int x, int y)
        {
            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 5;
            double yy = beginy + (boxwidth / 2) - boxwidth / 5;

            double xx_width = (boxwidth / 5) * 2;
            double yy_height = ((boxwidth / 5) * 2);
            
            var Box = new BoxView
            {
                CornerRadius = (int)(xx_width / 2),
                BackgroundColor = Color.Black
            };

            absoluteLayout.Children.Add(Box, new Rectangle(xx, yy, xx_width, yy_height));
        }

        public void ClearBoardState()
        {
            bool firstskip = false;
            List<View> chiles = new List<View>();
            foreach (var child in absoluteLayout.Children)
            {
                if (firstskip == false)
                {
                    firstskip = true;
                    continue;
                }
                chiles.Add(child);
            }

            foreach (var child in chiles)
            {
                absoluteLayout.Children.Remove(child);
            }

            DrawLine();
        }

        void DrawLayout(int x, int y, Color color)
        {
            if (prevLayout != null)
                absoluteLayout.Children.Remove(prevLayout);

            //SIZE로 등분한 길이
            double beginx = boxwidth * x;
            double beginy = boxwidth * y;

            //등분한 사각형에서 위로 살짝 올린다.
            double xx = beginx + (boxwidth / 2) - boxwidth / 4;
            double yy = beginy + (boxwidth / 2) - boxwidth / 4;

            double xx_width = (boxwidth / 4) * 2;
            double yy_height = ((boxwidth / 4) * 2);

            prevLayout = new BoxView
            {
                BackgroundColor = color
            };

            absoluteLayout.Children.Add(prevLayout, new Rectangle(xx, yy, xx_width, yy_height));
        }

        BoxView prevLayout = null;

        public void UpdateAim(int x, int y)
        {
            DrawLayout(x, y, Color.AliceBlue);
        }

        void DrawLine()
        {
            double x, y;
            x = boxwidth / 2;
            y = boxwidth / 2;

            for (int yy = 0; yy < size; yy++) //draw vertical line
            {
                absoluteLayout.Children.Add(new BoxView
                {
                    Color = Color.Black,
                }, new Rectangle(x + (yy * boxwidth), y, 1, screeny - boxwidth));

            }


            for (int xx = 0; xx < size; xx++) //draw horizontal
            {
                absoluteLayout.Children.Add(new BoxView
                {
                    Color = Color.Black,
                }, new Rectangle(x, y + (xx * boxwidth), screenx - boxwidth, 1));

            }
        }

        void FindXY(int x, int y, out int out_x, out int out_y)
        {
            out_y = out_x = -1;

            for (int by = 0; by < size; by++)
            {
                for (int bx = 0; bx < size; bx++)
                {
                    double beginx = boxwidth * bx;
                    double beginy = boxwidth * by;

                    double xx = beginx + (boxwidth / 2) - boxwidth / 3;
                    double yy = beginy + (boxwidth / 2) - boxwidth / 3;

                    double xxwidth = xx + (boxwidth / 3) * 2;
                    double yywidth = yy + (boxwidth / 3) * 2;


                    if (xx <= x && xxwidth >= x)
                    {
                        if (yy <= y && yywidth >= y)
                        {
                            out_x = bx;
                            out_y = by;
                            return;
                        }
                    }
                }
            }
        }

        void OnTouchEffectAction(object sender, TouchActionEventArgs args)
        {
            if (args.Type != TouchActionType.Pressed)
                return;

            var x = args.Location.X;
            var y = args.Location.Y;

            int outx;
            int outy;
            FindXY((int)x, (int)y, out outx, out outy);

            if (outx == -1 || outy == -1)
                return;

            contorlLogic.Board_MouseLeftButtonDown((int)outx+1, (int)outy+1);
        }
    }
}
