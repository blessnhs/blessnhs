using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK_T
{
    public static class User
    {
        public static TileStatus Color;
        public static bool IsMyTurn = false;

        public static DateTime MytrunStartTime = DateTime.Now;

        public static string MyNickName;
        public static string OppNickName;
        public static string PhotoPath;


        //acount fire base
        public static string Uid;
        public static string Token;
    }
}
