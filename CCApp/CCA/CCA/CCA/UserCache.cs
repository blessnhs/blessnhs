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
        static public UserCacheData CacheData = new UserCacheData();

        static public bool LoginSuccess = false;

        static public HashSet<int> RoomIdList = new HashSet<int>();

        static public int CurrentChatViewNumber;

        static public float Version = 0;

        static public bool OnceVersionNotify = false;
        static public bool OnceUpdate = false;

        static public Dictionary<string, RegCam> CamDic = new Dictionary<string, RegCam>();


    }
}
