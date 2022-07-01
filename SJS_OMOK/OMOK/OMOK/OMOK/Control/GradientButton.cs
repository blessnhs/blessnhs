using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace OMOK.Control
{
    public class GradientButton : Button
    {
        public Color StartColor { get; set; }
        public Color EndColor { get; set; }
        public Color StartTouchColor { get; set; }
        public Color EndTouchColor { get; set; }

        public string IdField { get; set; }

        public GradientButton()
        {
            BackgroundColor = Color.Transparent;
        }
    }
}
