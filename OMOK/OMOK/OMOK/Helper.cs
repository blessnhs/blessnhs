using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public static class Helper
    {
        public static void SET_X_Y_COLOR(sbyte X, sbyte Y, byte COLOR, ref int FLAG)
        {
            FLAG = X & 0xFF;

            int temp = Y & 0xFF;
            temp = temp << 8;
            FLAG = FLAG | temp;

            int _color = FLAG >> 16;
            _color = _color | COLOR;
            _color = _color << 16;
            FLAG = FLAG | _color;
        }

        public static bool Get_X_Y_COLOR(ref sbyte X, ref sbyte Y, ref byte Color, int FLAG)
        {
            X = (sbyte)(FLAG & 0xFF);
            Y = (sbyte)(FLAG >> 8);
            Color = (byte)((FLAG >> 16) & 0x01);

            return true;
        }


        public static byte[] ToByteString(this string inStr, bool in64String = false)
        {
            if (string.IsNullOrEmpty(inStr) == true)
                return null;

            byte[] btTemp = new byte[inStr.Length * sizeof(char)];
#if UNITY_IOS
        btTemp = _cp949.GetBytes(inStr);
#else
            btTemp = System.Text.Encoding.GetEncoding(949).GetBytes(inStr);
#endif
            return btTemp;
        }

        public static string ToStr(this byte[] inByte)
        {
            if (inByte == null || inByte.Length <= 0)
                return "";

            try
            {
                string m_receive = System.Text.Encoding.GetEncoding(949).GetString(inByte);
                
                return m_receive;
            }
            catch(Exception e)
            {
                return System.Text.Encoding.Default.GetString(inByte);
            }
        }

        public static string GetLocaleImagePath(string locale)
        {
            switch(locale)
            {
                case "ko":
                    return "OMOK.Image.ko.png";
                case "ja":
                    return "OMOK.Image.jp.png";
                case "zh-TW":
                    return "OMOK.Image.tw.png";
                case "fil":
                    return "OMOK.Image.ph.png";
             
            }

            return "";
        }

        public static System.Int64 ToInteger64(this string thisString)
        {
            if (string.IsNullOrEmpty(thisString))
                return 0;

            return System.Int64.Parse(thisString, System.Globalization.CultureInfo.InvariantCulture.NumberFormat);
        }

        public static string LevelConverter(int level)
        {
            int o = 19 - level;

            if(o <= 0)
            {
               int l = Math.Abs(o) + 1;
               if (l >= 9)
                    l = 9;

                if (User.Locale != "ko")
                    return l + " HLvl";

                return l + "단";
            }

            if (User.Locale != "ko")
                return o + " Lvl";
            return o + "급";
            
        }
    }

    public static class DateTimeExtensions
    {
        public static DateTime StartOfWeek(this DateTime dt, DayOfWeek startOfWeek)
        {
            int diff = (7 + (dt.DayOfWeek - startOfWeek)) % 7;
            return dt.AddDays(-1 * diff).Date;
        }
    }
}


