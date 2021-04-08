// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#include <cstdint>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <list>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <rpc.h>
#include <windows.h>
#include <iostream>

#include <time.h>
#include <boost/make_shared.hpp>


#include "CLI.GS.pb.h"
// add headers that you want to pre-compile here
#include "framework.h"

#endif //PCH_H
