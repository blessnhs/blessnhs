#include "StdAfx.h"
#include "MSG_PLAYER_QUERY.h"


#include "../../DBProxy/DBProcess.h"
#include "../../DBProxy/DBJob/RequestPlayerAuth.h"
#include "../../DBProxy/DBJob/UpdatePlayerData.h"
#include "../../DBProxy/DBProcessContainer.h"

#include "../../PLAYER/Message/MSG_PLAYER_QUERY_RES.h"

#include "../GSPacket/Protocol.h"
#include "../../PLAYER/Container/Player.h"
#include <boost/shared_ptr.hpp>
#include "../../LOG/FrontLog.h"
