using System;
using System.Collections.Generic;
using System.Text;

namespace Antioch
{
    internal static class User
    {
        static public string Username { get; set; }
        static public string Password { get; set; }

        static public UserCacheData CacheData = new UserCacheData();

        static public bool LoginSuccess = false;

    }
}
