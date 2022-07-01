using System;
using System.Collections.Generic;
using System.Text;

namespace OMOK
{
    public enum PlayerState
    {
        None,
        Lobby,
        Room,
    }

    public class PlayerInfo
    {
        public string NickName;
        public string PhotoPath;

        public int rank;
        
        public int win;
        public int lose;
        public int draw;
        public int score;

        public int level;

        public int ai_rule = 1;
        public int ai_mode = 2;
        public bool ai_reset_flag = false; //ai 시작 초기화 플래그


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
        public static string Locale;

        public static PlayerState state = PlayerState.None;

        public static bool IsLogin = false;

        public static bool Auto = false;

        public static bool IsEnableScreenChat = false;

    }
}
