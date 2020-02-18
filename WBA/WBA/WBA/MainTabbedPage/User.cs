using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace WBA.MainTabbedPage
{
    internal static class User
    {
        static public string Username { get; set; }
        static public string Password { get; set; }

        static public UserCacheData CacheData = new UserCacheData();

    }


}

