﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace WBA.MainTabbedPage
{
    internal class User
    {
        public string Username { get; set; }
        public string Password { get; set; }

        static public UserCacheData CacheData = new UserCacheData();

    }


}

