using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{

    public class PlayerInfo
    {
        public string NickName;
        public string PhotoPath;

        public int rank;
        
        public int win;
        public int lose;
        public int draw;
        public int score;

    }

    public static class User
    {
        public static eTeam Color;
        public static bool IsMyTurn = false;

        public static DateTime MytrunStartTime = DateTime.Now;

        //내 정보
        public static PlayerInfo myInfo = new PlayerInfo();

        //상대방 정보
        public static PlayerInfo OppInfo = new PlayerInfo();

        //acount fire base
        public static string Uid;
        public static string Token;
        public static long Id;
    }
}
