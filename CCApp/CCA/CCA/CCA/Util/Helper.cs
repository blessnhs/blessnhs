using System;
using System.IO;
using System.Linq;
using System.Text;
using Xamarin.Forms;

namespace CCA
{
    public class StreamWrapper
    {
        public MemoryStream stream = new MemoryStream();
        public int pos = 0;
        public int type = 0;
        
        public int width;//이미지 크기 가로
        public int height;//
    }

    static class Helper
    {
       
        public static byte[] ToByteString(this string inStr, bool in64String = false)
        {
            if (string.IsNullOrEmpty(inStr) == true)
                return null;

            try
            {

                byte[] btTemp = new byte[inStr.Length * sizeof(char)];
#if UNITY_IOS
        btTemp = _cp949.GetBytes(inStr);
#else
                btTemp = System.Text.Encoding.GetEncoding(949).GetBytes(inStr);
#endif
                return btTemp;
            }
            catch(Exception)
            {
                byte[] bytes = Encoding.ASCII.GetBytes(inStr);
                return bytes;
            }
        }

        public static string ToStr(this byte[] inByte)
        {
            if (inByte == null || inByte.Length <= 0)
                return "";

            try
            {
#if UNITY_IOS
        string m_receive = _cp949.GetString(inByte);
#else
                string m_receive = System.Text.Encoding.GetEncoding(949).GetString(inByte);
#endif
            return m_receive;

            }
            catch (Exception e)
            {
                return Encoding.ASCII.GetString(inByte);
            }
        }

        static public void RemoveRowGrid(Grid grid, int pos = 0)
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
