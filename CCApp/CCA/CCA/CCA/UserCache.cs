using System;
using System.Collections.Generic;
using System.Text;

namespace CCA
{
   public class RegCam
    {
        public string MachineId;
        public string MachineName;
        public string Ip;
        public Int64 PlayerId;
    }

    public class User
    {

        static public void Clear()
        {
            LoginSuccess = false;
            CacheData = new UserCacheData();
            RoomIdList.Clear();

            ProfileUrl = null;
            Uid = null;
            Token = null;
            Name = null;
            EMail = null;
        }

        static public bool isSwitching = false;

        static public UserCacheData CacheData = new UserCacheData();

        static public bool LoginSuccess = false;

        static public HashSet<int> RoomIdList = new HashSet<int>();

        static public int CurrentChatViewNumber;

        static public float Version = 0;

        static public bool OnceVersionNotify = false;
        static public bool OnceUpdate = false;

        static public string ProfileUrl;
        static public string Uid;
        static public string Token;
        static public string Name;
        static public string EMail;
        static public long DBId;

        //내가 등록한 카메라 리스트
        static public Dictionary<string, RegCam> CamDic = new Dictionary<string, RegCam>();


    }
}
