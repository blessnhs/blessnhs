using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Xamarin.Forms;

namespace WBA
{
    static class Helper
    {
        static public void RemoveRowGrid(Grid grid,int pos = 0)
        {
            var children = grid.Children.ToList();
            foreach (var child in children)
            {
                int row = Grid.GetRow(child);
                if (row >= pos)
                {
                    grid.Children.Remove(child);
                }
            }
        }

        static public void SpliteVerseText(string input, out int verse, out string text)
        {
            verse = 1;
            text = "";

            string[] header = input.Split(' ');

            if (header.Length == 0)
                return;

            for (int i = 1; i < header.Length; i++)
            {
                text += header[i];
                text += " ";
            }
        }

        static public string DateTimeToShortTime(DateTime time)
        {
            string timestr = time.ToString("HH:mm");
            return timestr;
        }


        static public bool IsNumber(string me)
        {
            foreach (char ch in me)
            {
                if (!Char.IsDigit(ch))
                    return false;
            }

            return true;
        }

    }
}
