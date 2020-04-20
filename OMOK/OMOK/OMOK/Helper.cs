using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    static class Helper
    {
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
#if UNITY_IOS
        string m_receive = _cp949.GetString(inByte);
#else
            string m_receive = System.Text.Encoding.GetEncoding(949).GetString(inByte);
#endif
            return m_receive;
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

                return l + "단";
            }
           
           return o + "급";
            
        }
    }
}
