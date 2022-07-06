using System;
using System.Collections.Generic;
using System.Text;

namespace CCA
{
   public class RegCam
    {
        public string MachineId;
        public string MachineModel;
        public Int64 PlayerId;
    }

    public class User
    {

        static public void Clear()
        {
            LoginSuccess = false;
            CacheData = new UserCacheData();
            RoomIdList.Clear();

            PhotoPath = null;
            Uid = null;
            Token = null;
            NickName = null;
        }

       static public UserCacheData CacheData = new UserCacheData();

        static public bool LoginSuccess = false;

        static public HashSet<int> RoomIdList = new HashSet<int>();

        static public int CurrentChatViewNumber;

        static public float Version = 0;

        static public bool OnceVersionNotify = false;
        static public bool OnceUpdate = false;

        static public string PhotoPath;
        static public string Uid;
        static public string Token;
        static public string NickName;

        //내가 등록한 카메라 리스트
        static public Dictionary<string, RegCam> CamDic = new Dictionary<string, RegCam>();


    }
}
